//
// Created by 12132 on 2022/1/9.
//

#include "vulkan_image.h"
#include "core.h"

void vulkan_image_create (
    vulkan_context* context,
    VkImageType image_type,
    u32 width,
    u32 height,
    VkFormat format,
    VkImageTiling tilling,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags memory_flags,
    b32 create_view,
    VkImageAspectFlags view_aspect_flags,
    vulkan_image* out_image
)
{
    /// copy params
    out_image->width = width;
    out_image->height = height;

    VkImageCreateInfo imageCI ={};
    imageCI.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageCI.imageType = VK_IMAGE_TYPE_2D;
    imageCI.extent.width = width;
    imageCI.extent.height = height;
    imageCI.extent.depth = 1;           /// todo support depth
    imageCI.mipLevels = 4;            /// todo support mip mapping
    imageCI.arrayLayers = 1;          /// todo support number of layers in the image
    imageCI.format = format;
    imageCI.tiling = tilling;
    imageCI.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageCI.usage =usage;
    imageCI.samples = VK_SAMPLE_COUNT_1_BIT; /// todo configure sample count
    imageCI.sharingMode = VK_SHARING_MODE_EXCLUSIVE;   /// todo configure share mode
    VK_CHECK(vkCreateImage(context->device.logical_device,&imageCI,context->allocator,
                           &out_image->handle));

    VkMemoryRequirements memory_requirements ={};
    vkGetImageMemoryRequirements(context->device.logical_device,out_image->handle,&memory_requirements);

    i32 memory_type = context->find_memory_index(memory_requirements.memoryTypeBits,memory_flags);

    if (memory_type== -1){
        KERROR("Required memory type not found, image not valid");
    }

    VkMemoryAllocateInfo memory_info ={};
    memory_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    memory_info.allocationSize =memory_requirements.size;
    memory_info.memoryTypeIndex = memory_type;
    VK_CHECK(vkAllocateMemory(context->device.logical_device,&memory_info,
                              context->allocator,&out_image->memory));

    VK_CHECK(vkBindImageMemory(context->device.logical_device,
                               out_image->handle,
                               out_image->memory,0));  /// todo configurable offset
    //create view
    if (create_view) {
        out_image->view = 0;
        vulkan_image_view_create(context,format, out_image,view_aspect_flags);
    }

}

void vulkan_image_view_create(
    vulkan_context* context,
    VkFormat format,
    vulkan_image* image,
    VkImageAspectFlags aspect_flags)
{
    VkImageViewCreateInfo viewCI ={};
    viewCI.sType =VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewCI.viewType = VK_IMAGE_VIEW_TYPE_2D;   ///todo configurable
    viewCI.format = format;
    viewCI.image = image->handle;
    viewCI.subresourceRange.aspectMask = aspect_flags;

    /// todo make it configurable
    viewCI.subresourceRange.baseMipLevel = 0;
    viewCI.subresourceRange.levelCount =1;
    viewCI.subresourceRange.baseArrayLayer = 0;
    viewCI.subresourceRange.layerCount = 1;

    VK_CHECK(vkCreateImageView(context->device.logical_device,
                               &viewCI,context->allocator,&image->view));

}

void vulkan_image_destroy(vulkan_context* context, vulkan_image* image)
{
    if (image->view){
        vkDestroyImageView(context->device.logical_device,image->view,context->allocator);
        image->view = 0;
    }
    if (image->memory){
        vkFreeMemory(context->device.logical_device,image->memory,context->allocator);
        image->memory= 0;
    }
    if (image->handle){
        vkDestroyImage(context->device.logical_device,image->handle,context->allocator);
        image->handle= 0;
    }
    image->height= 0;
    image->width = 0;
}