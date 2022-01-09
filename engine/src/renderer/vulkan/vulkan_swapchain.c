//
// Created by 12132 on 2022/1/9.
//

#include "vulkan_swapchain.h"
#include "vulkan_device.h"
#include "vulkan_image.h"
#include "core.h"


static void create(vulkan_context* context, u32 width,u32 height, vulkan_swapchain * swapchain);
static void destroy(vulkan_context* context,vulkan_swapchain * swapchain);

void vulkan_swapchain_create (
    vulkan_context* context,
    u32 width,
    u32 height,
    vulkan_swapchain* out_swapchain)
{
    create(context,width,height,out_swapchain);
}

void vulkan_swapchain_recreate(
    vulkan_context* context,
    u32 width,
    u32 height,
    vulkan_swapchain* swapchain)
{
    create(context,width,height,swapchain);
}

void vulkan_swapchain_destroy(
    vulkan_context* context,
    vulkan_swapchain* swapchain)
{
    destroy(context,swapchain);
}


b8 vulkan_swapchain_next_image_index(
    vulkan_context* context,
    vulkan_swapchain* swapchain,
    u64 timeout_ns,
    VkSemaphore image_available_semaphore,
    VkFence fence,
    u32* out_image_index)
{
    VkResult result = vkAcquireNextImageKHR(
        context->device.logical_device,
        swapchain->handle,
        timeout_ns,
        image_available_semaphore,
        fence,
        out_image_index
        );
    if (result == VK_ERROR_OUT_OF_DATE_KHR) {
        vulkan_swapchain_recreate(context,
                                  context->frame_buffer_width,
                                  context->frame_buffer_height,swapchain);
        return false;
    } else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR){
        KFATAL("Failed to acquire swapchain image");
        return false;
    }

    return true;

}

void vulkan_swapchain_present (
    vulkan_context* context,
    vulkan_swapchain* swapchain,
    VkQueue graphics_queue,
    VkQueue present_queue,
    VkSemaphore render_complete_semaphore,
    u32 present_index)
{
    VkPresentInfoKHR present;
    present.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    present.waitSemaphoreCount = 1;
    present.pWaitSemaphores= &render_complete_semaphore;
    present.swapchainCount = 1;
    present.pSwapchains = &swapchain->handle;
    present.pImageIndices = &present_index;
    present.pResults = 0;
    VkResult result =  vkQueuePresentKHR(graphics_queue,&present);
    if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR){
        vulkan_swapchain_recreate(context,context->frame_buffer_width,
                                  context->frame_buffer_height,swapchain);
    } else if(result != VK_SUCCESS) {
        KFATAL("failed  Present Swapchain Image ");
    }
}

void create(vulkan_context* context, u32 width,u32 height, vulkan_swapchain * swapchain)
{
    VkExtent2D swapchain_extent = {.width = width,.height = height};
    swapchain->max_frame_in_flight = 2;  /// double buffer todo support tri buffer

    b8 found = false;
    for (u32 i =0; i < context->device.swapchain_support.format_count; ++i){
        VkSurfaceFormatKHR format = context->device.swapchain_support.support_formats[i];
        //// perferred formats
        if (format.format == VK_FORMAT_B8G8R8A8_UNORM && format.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR){
            swapchain->image_format = format;
            found = true;
            break;
        }
    }

    if (!found){
        swapchain->image_format  = context->device.swapchain_support.support_formats[0];
    }

    VkPresentModeKHR present_mode = VK_PRESENT_MODE_FIFO_KHR;
    for (u32 i =0; i < context->device.swapchain_support.present_mode_count; ++i) {
        VkPresentModeKHR mode =context->device.swapchain_support.support_present_modes[i];
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR){
            present_mode = mode;
            break;
        }
    }

    /// Requery swapchain support
    vulkan_device_query_swapchain_support(
        context->device.physical_device,
        context->surface,
        &context->device.swapchain_support);

    if (context->device.swapchain_support.capabilities.currentExtent.width != UINT32_MAX){
        swapchain_extent = context->device.swapchain_support.capabilities.currentExtent;
    }

    VkExtent2D min = context->device.swapchain_support.capabilities.minImageExtent;
    VkExtent2D max = context->device.swapchain_support.capabilities.maxImageExtent;
    swapchain_extent.width = KCLAMP(swapchain_extent.width,min.width,max.width);
    swapchain_extent.height = KCLAMP(swapchain_extent.height,min.height,max.height);

    u32 image_count = context->device.swapchain_support.capabilities.minImageCount +1;
    if (context->device.swapchain_support.capabilities.maxImageCount > 0 &&
        image_count > context->device.swapchain_support.capabilities.maxImageCount) {
        image_count = context->device.swapchain_support.capabilities.maxImageCount;
    }

    /// swapchain create info
    VkSwapchainCreateInfoKHR swapchainCI={};
    swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCI.minImageCount = image_count;
    swapchainCI.imageExtent = swapchain_extent;
    swapchainCI.surface= context->surface;
    swapchainCI.imageFormat = swapchain->image_format.format;
    swapchainCI.imageColorSpace = swapchain->image_format.colorSpace;
    swapchainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCI.imageArrayLayers = 1 ;

    if (context->device.graphics_queue_index != context->device.present_queue_index){
        u32 queueFamilyIndex[] = {
            (u32)context->device.graphics_queue_index,
            (u32)context->device.present_queue_index
        };
        swapchainCI.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainCI.queueFamilyIndexCount =2;   /// 不同的queue 共享 只有sharemode == VK_SHARING_MODE_CONCURRENT 才有效
        swapchainCI.pQueueFamilyIndices = queueFamilyIndex;
    } else {
        swapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainCI.queueFamilyIndexCount = 0;
        swapchainCI.pQueueFamilyIndices = 0;
    }

    swapchainCI.preTransform = context->device.swapchain_support.capabilities.currentTransform;
    swapchainCI.compositeAlpha=VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCI.presentMode = present_mode;
    swapchainCI.clipped= VK_TRUE;
    swapchainCI.oldSwapchain = 0;

    VK_CHECK(vkCreateSwapchainKHR(context->device.logical_device,&swapchainCI,
                                  context->allocator,&swapchain->handle));


    context->current_frame = 0;

    /// get swapchain image
    swapchain->image_count = 0;
    VK_CHECK(vkGetSwapchainImagesKHR(context->device.logical_device,
                                     swapchain->handle,
                                     &swapchain->image_count,
                            0));
    if (!swapchain->images) {
        swapchain->images = (VkImage*) kallocate(sizeof(VkImage) * swapchain->image_count,MEMORY_TAG_RENDERER);
    }
    if (!swapchain->views) {
        swapchain->views = (VkImageView*) kallocate(sizeof(VkImageView) * swapchain->image_count,MEMORY_TAG_RENDERER);
    }

    VK_CHECK(vkGetSwapchainImagesKHR(context->device.logical_device,
                                     swapchain->handle,
                                     &swapchain->image_count,
                                     swapchain->images));

    /// create ImageView
    for (u32 i =0 ; i < swapchain->image_count; ++i) {
        VkImageViewCreateInfo viewCI;
        viewCI.sType =VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        viewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;
        viewCI.image = swapchain->images[i];
        viewCI.format = swapchain->image_format.format;
        viewCI.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        viewCI.subresourceRange.baseArrayLayer = 0;
        viewCI.subresourceRange.layerCount = 1;
        viewCI.subresourceRange.baseMipLevel = 0;
        viewCI.subresourceRange.levelCount = 1;
        VK_CHECK(vkCreateImageView(context->device.logical_device,
                                   &viewCI,context->allocator,
                                   &swapchain->views[i]));
    }

    if (!vulkan_device_detect_depth_format(&context->device)){
        context->device.depth_format = VK_FORMAT_UNDEFINED;
        KFATAL("Failed to find supported format!");
    }

    /// create depth image
    vulkan_image_create(context,
                        VK_IMAGE_TYPE_2D,
                        swapchain_extent.width,
                        swapchain_extent.height,
                        context->device.depth_format,
                        VK_IMAGE_TILING_OPTIMAL,
                        VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
                        VK_MEMORY_HEAP_DEVICE_LOCAL_BIT,
                        true,
                        VK_IMAGE_ASPECT_DEPTH_BIT,
                        &swapchain->depth_attachment);

    KINFO("create swapchain successfully");


}

static void destroy(vulkan_context* context,vulkan_swapchain * swapchain)
{
    vulkan_image_destroy(context,&swapchain->depth_attachment);
    kzero_memory(&swapchain->depth_attachment, sizeof(vulkan_image));

    for (u32 i=0; i < swapchain->image_count;++i){
        vkDestroyImageView(context->device.logical_device,swapchain->views[i],context->allocator);
        swapchain->views[i] = 0;
    }

    kfree(swapchain->views, sizeof(VkImageView) * swapchain->image_count,MEMORY_TAG_RENDERER);
    swapchain->views = 0;

    vkDestroySwapchainKHR(context->device.logical_device,
                          swapchain->handle,
                          context->allocator);


    kfree(swapchain->images, sizeof(VkImage) *swapchain->image_count,MEMORY_TAG_RENDERER);
    swapchain->images = 0;
    swapchain->image_count =0;
    swapchain->handle = 0;
    swapchain->max_frame_in_flight = 0;

    KINFO("swapchain destroy");

}