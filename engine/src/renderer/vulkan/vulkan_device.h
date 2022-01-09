//
// Created by 12132 on 2022/1/9.
//

#ifndef BACKEND_ENGINE_SRC_RENDERER_VULKAN_VULKAN_DEVICE_H
#define BACKEND_ENGINE_SRC_RENDERER_VULKAN_VULKAN_DEVICE_H

#include "vulkan_types.h"

b8 vulkan_device_create(vulkan_context* context);

void vulkan_device_destroy(vulkan_context * context);

void vulkan_device_query_swapchain_support(
    VkPhysicalDevice physical_device,
    VkSurfaceKHR surface,
    vulkan_swapchain_support_info* out_swapchain_info);

#endif //BACKEND_ENGINE_SRC_RENDERER_VULKAN_VULKAN_DEVICE_H