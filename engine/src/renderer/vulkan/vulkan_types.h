//
// Created by 12132 on 2022/1/9.
//

#ifndef BACKEND_ENGINE_SRC_RENDERER_VULKAN_VULKAN_TYPES_H
#define BACKEND_ENGINE_SRC_RENDERER_VULKAN_VULKAN_TYPES_H
#include "defines.h"
#include "core/asserts.h"
#include <vulkan/vulkan.h>

#define VK_CHECK(expr)              \
{                                   \
    KASSERT(expr == VK_SUCCESS);    \
}

typedef struct vulkan_swapchain_support_info{
    VkSurfaceCapabilitiesKHR capabilities;
    u32 format_count;
    VkSurfaceFormatKHR* support_formats;
    uint32_t present_mode_count;
    VkPresentModeKHR* support_present_modes;
} vulkan_swapchain_support_info;

typedef struct vulkan_device {
    VkPhysicalDevice physical_device;
    VkDevice logical_device;
    i32 graphics_queue_index;
    i32 present_queue_index;
    i32 transfer_queue_index;
    i32 compute_queue_index;

    VkQueue graphics_queue;
    VkQueue present_queue;
    VkQueue transfer_queue;
    VkQueue compute_queue;

    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceMemoryProperties memory;
    vulkan_swapchain_support_info swapchain_support;
}vulkan_device;


typedef struct vulkan_context{
    VkInstance  instance;
    VkAllocationCallbacks* allocator;
    vulkan_device device;
    VkSurfaceKHR surface;

#ifdef _DEBUG
    VkDebugUtilsMessengerEXT debug_messenger;
#endif
}vulkan_context;


#endif //BACKEND_ENGINE_SRC_RENDERER_VULKAN_VULKAN_TYPES_H
