//
// Created by 12132 on 2022/1/9.
//

#include "vulkan_device.h"
#include "core/logger.h"
#include "core/kmemory.h"
#include "container/darray.h"
#include "core/kstring.h"

typedef struct vulkan_physical_device_requirements{
    b8 graphics;
    b8 present;
    b8 compute;
    b8 transfer;
    const char** device_extension_names;
    b8 sampler_anisotropy;
    b8 discrete_gpu;
}vulkan_physical_device_requirements;

typedef struct vulkan_physical_device_queue_family_info{
    u32 graphics_family_index;
    u32 present_family_index;
    u32 compute_family_index;
    u32 transfer_family_index;
}vulkan_physical_device_queue_family_info;



b8 select_physical_device(vulkan_context* context);
b8 physical_device_meet_requirements(
    VkPhysicalDevice physical_device,
    VkSurfaceKHR surface,
    const VkPhysicalDeviceProperties* properties,
    const VkPhysicalDeviceFeatures* features,
    const vulkan_physical_device_requirements* requirements,
    vulkan_physical_device_queue_family_info* out_queue_infos,
    vulkan_swapchain_support_info* out_swapchain_info
    );



b8 vulkan_device_create(vulkan_context* context)
{
    if (!select_physical_device(context)){
        return false;
    }
    return true;
}

void vulkan_device_destroy(vulkan_context * context)
{

}

b8 select_physical_device(vulkan_context* context)
{
    u32 physical_device_count = 0;
    VK_CHECK(vkEnumeratePhysicalDevices(context->instance,&physical_device_count,0));
    if(physical_device_count == 0) {
        KERROR("No Support vulkan physical device in this pc");
        return false;
    }

    VkPhysicalDevice physical_devices[physical_device_count];
    VK_CHECK(vkEnumeratePhysicalDevices(context->instance,&physical_device_count,physical_devices));

    for (u32 i =0 ;i  < physical_device_count; ++i) {
        VkPhysicalDeviceProperties properties;
        vkGetPhysicalDeviceProperties(physical_devices[i],&properties);

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(physical_devices[i],&features);

        VkPhysicalDeviceMemoryProperties memory;
        vkGetPhysicalDeviceMemoryProperties(physical_devices[i],&memory);

        vulkan_physical_device_requirements requirements;

        requirements.graphics = true;
        requirements.present = true;
        requirements.transfer =true;

        // todo check is use computer shader
        requirements.compute= true;

        requirements.sampler_anisotropy = true;
        requirements.discrete_gpu = true;

        requirements.device_extension_names = darray_create(const char* );
        darray_push(requirements.device_extension_names,&VK_KHR_SWAPCHAIN_EXTENSION_NAME);

        vulkan_physical_device_queue_family_info queue_info ={};

        b8 res =  physical_device_meet_requirements(physical_devices[i],
                                                    context->surface,
                                                    &properties,
                                                    &features,
                                                    &requirements,
                                                    &queue_info,
                                                    &context->device.swapchain_support);

        if (res) {
            KINFO("Select Device: '%s'.",properties.deviceName);
            switch (properties.deviceType) {
            case VK_PHYSICAL_DEVICE_TYPE_OTHER:KINFO("GPU type is unknown");
                break;
            case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:KINFO("GPU type is integrated");
                break;
            case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:KINFO("GPU type is discrete");
                break;
            case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:KINFO("GPU type is virtual");
                break;
            case VK_PHYSICAL_DEVICE_TYPE_CPU :KINFO("GPU type is cpu");
                break;
            }
            KINFO("GPU driver version :%d.%d.%d",
                  VK_VERSION_MAJOR(properties.driverVersion),
                  VK_VERSION_MINOR(properties.driverVersion),
                  VK_VERSION_PATCH(properties.driverVersion));

            //vulkan api version
            KINFO("Vulkan API version: %d.%d.%d",
                  VK_VERSION_MAJOR(properties.apiVersion),
                  VK_VERSION_MINOR(properties.apiVersion),
                  VK_VERSION_PATCH(properties.apiVersion));

            /// Memory info
            for (u32 j = 0; j<memory.memoryHeapCount;++j) {
                f32 memory_size_gib = ((f32)memory.memoryHeaps[j].size)/(1024.0 * 1024.0 *1024.0);
                if (memory.memoryHeaps[j].flags & VK_MEMORY_HEAP_DEVICE_LOCAL_BIT) {
                    KINFO("Local GPU memory: %.2f Gib",memory_size_gib);
                } else{
                    KINFO("Shared System Memory: %.2f Gib",memory_size_gib);
                }
            }

            context->device.physical_device = physical_devices[i];
            context->device.graphics_queue_index = queue_info.graphics_family_index;
            context->device.present_queue_index =queue_info.present_family_index;
            context->device.transfer_queue_index = queue_info.transfer_family_index;
            context->device.compute_queue_index = queue_info.compute_family_index;

            //
            context->device.properties  = properties;
            context->device.features = features;
            context->device.memory = memory;
            break;
        }
    }

    if (!context->device.physical_device){
        KINFO("No physical device was found meet requirement");
        return false;
    }
    KINFO("Physical Device Selected");
    return true;
}

b8 physical_device_meet_requirements(
    VkPhysicalDevice physical_device,
    VkSurfaceKHR surface,
    const VkPhysicalDeviceProperties* properties,
    const VkPhysicalDeviceFeatures* features,
    const vulkan_physical_device_requirements* requirements,
    vulkan_physical_device_queue_family_info* out_queue_infos,
    vulkan_swapchain_support_info* out_swapchain_info
)
{
    out_queue_infos->compute_family_index = -1;
    out_queue_infos->graphics_family_index = -1;
    out_queue_infos->present_family_index = -1;
    out_queue_infos->transfer_family_index = -1;
    if (requirements->discrete_gpu) {
        if (properties->deviceType !=VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            KINFO("Device is not a discrete gpu and one is required skip it");
            return false;
        }
    }

    u32 queue_family_count = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device,&queue_family_count,0);
    VkQueueFamilyProperties queue_families[queue_family_count];
    vkGetPhysicalDeviceQueueFamilyProperties(physical_device,&queue_family_count,queue_families);

    ///
    KINFO("Graphics | Present | Compute | Transfer | Name");
    u8 min_transfer_score = 255; // 打分
    for (u32 i= 0;  i<queue_family_count ; i++) {
        u8 current_score= 0;

        /// Graphics
        if (queue_families[i].queueFlags & VK_QUEUE_GRAPHICS_BIT){
            out_queue_infos->graphics_family_index = 1;
            ++current_score;
        }

        /// compute
        if (queue_families[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
            out_queue_infos->compute_family_index = 1;
            ++current_score;
        }

        /// transfer
        if (queue_families[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
            out_queue_infos->transfer_family_index = 1;
            ++current_score;
        }
        VkBool32 support_present = VK_FALSE;
        vkGetPhysicalDeviceSurfaceSupportKHR(physical_device,i,surface,&support_present);
        if(support_present){
            out_queue_infos->present_family_index = i;
        }
    }
    KINFO("      %d |      %d |      %d |       %d | %s",
          out_queue_infos->graphics_family_index,
          out_queue_infos->present_family_index,
          out_queue_infos->compute_family_index,
          out_queue_infos->transfer_family_index,
          properties->deviceName);

    if (
        (!requirements->graphics || (requirements->graphics && out_queue_infos->graphics_family_index != -1)) &&
        (!requirements->compute || (requirements->compute && out_queue_infos->compute_family_index != -1))    &&
        (!requirements->transfer || (requirements->transfer && out_queue_infos->transfer_family_index != -1)) &&
        (!requirements->present || (requirements->present && out_queue_infos->present_family_index != -1))
        ){
        KINFO("Device meets requirements");
        KTRACE("Graphics Family Index : %i",out_queue_infos->graphics_family_index);
        KTRACE("Compute Family Index : %i",out_queue_infos->compute_family_index);
        KTRACE("Transfer Family Index : %i",out_queue_infos->transfer_family_index);
        KTRACE("Present Family Index : %i",out_queue_infos->present_family_index);
        /// Query swapchain support
        vulkan_device_query_swapchain_support(physical_device,surface,out_swapchain_info);
        if (out_swapchain_info->format_count  <1 || out_swapchain_info->present_mode_count < 1) {
            if (out_swapchain_info->support_formats) {
                kfree(out_swapchain_info->support_formats,
                      sizeof(VkSurfaceFormatKHR) *out_swapchain_info->format_count,
                      MEMORY_TAG_RENDERER);
            }
            if (out_swapchain_info->support_present_modes) {
                kfree(out_swapchain_info->support_formats,
                      sizeof(VkPresentInfoKHR) *out_swapchain_info->present_mode_count,
                      MEMORY_TAG_RENDERER);
            }
            KINFO("Required swapchain support not present,skip it");
            return false;
        }
        if (requirements->device_extension_names) {
            u32 available_extension_count =0;
            VkExtensionProperties* available_extensions = 0;
            VK_CHECK(vkEnumerateDeviceExtensionProperties(
                physical_device,
                0,
                &available_extension_count,
                0
                ));

            if (available_extension_count != 0) {
                available_extensions = kallocate(sizeof(VkExtensionProperties)* available_extension_count,MEMORY_TAG_RENDERER);
                VK_CHECK(vkEnumerateDeviceExtensionProperties(
                    physical_device,
                    0,
                    &available_extension_count,
                    available_extensions
                ));

                /// todo refractor it to function

                u32 required_extension_count = darray_length(requirements->device_extension_names);
                for (u32 i =0 ; i < required_extension_count; ++i) {
                    b8 found = false;
                    for (u32 j =0 ; j < available_extension_count; ++j) {
                        if (string_equal(requirements->device_extension_names[i],available_extensions[j].extensionName)){
                            found = true;
                            break;
                        }
                    }
                    if (!found) {
                        KINFO("require extension not found: '%s',skipping",requirements->device_extension_names[i]);
                        kfree(available_extensions, sizeof(VkExtensionProperties) *available_extension_count,MEMORY_TAG_RENDERER);
                        return false;
                    }
                }
            }
            kfree(available_extensions, sizeof(VkExtensionProperties) *available_extension_count,MEMORY_TAG_RENDERER);
        }

        /// samplerAnisotropy
        if (requirements->sampler_anisotropy && !features->samplerAnisotropy) {
            KINFO("Device not support samplerAnisotropy,skip");
            return false;
        }
        /// meet all requirement
        return true;
    }
    return false;
}

void vulkan_device_query_swapchain_support(
    VkPhysicalDevice physical_device,
    VkSurfaceKHR surface,
    vulkan_swapchain_support_info* out_swapchain_info)
{
    VK_CHECK(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device,
                                                       surface,
                                                       &out_swapchain_info->capabilities));

    VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device,
                                                  surface,
                                                  &out_swapchain_info->format_count,
                                         0));

    if (out_swapchain_info->format_count !=0) {
        if (out_swapchain_info->support_formats) {
            out_swapchain_info->support_formats = kallocate(sizeof(VkSurfaceFormatKHR)* out_swapchain_info->format_count,MEMORY_TAG_RENDERER);
        }
        VK_CHECK(vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device,
                                                      surface,
                                                      &out_swapchain_info->format_count,
                                                      out_swapchain_info->support_formats));
    }

    VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device,
                                                       surface,
                                                       &out_swapchain_info->present_mode_count,
                                                       0));
    if (out_swapchain_info->present_mode_count != 0) {
        if (out_swapchain_info->support_present_modes) {
            out_swapchain_info->support_present_modes = kallocate(sizeof(VkPresentModeKHR)* out_swapchain_info->present_mode_count,MEMORY_TAG_RENDERER);
        }

        VK_CHECK(vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device,
                                                           surface,
                                                           &out_swapchain_info->present_mode_count,
                                                           out_swapchain_info->support_present_modes));

    }



}
