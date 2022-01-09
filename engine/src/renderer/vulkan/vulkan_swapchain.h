//
// Created by 12132 on 2022/1/9.
//

#ifndef BACKEND_ENGINE_SRC_RENDERER_VULKAN_VULKAN_SWAPCHAIN_H
#define BACKEND_ENGINE_SRC_RENDERER_VULKAN_VULKAN_SWAPCHAIN_H
#include "vulkan_types.h"

void vulkan_swapchain_create(
    vulkan_context* context,
    u32 width,
    u32 height,
    vulkan_swapchain* out_swapchain);

void vulkan_swapchain_recreate(
    vulkan_context* context,
    u32 width,
    u32 height,
    vulkan_swapchain* swapchain);

void vulkan_swapchain_destroy(
    vulkan_context* context,
    vulkan_swapchain* swapchain);

b8 vulkan_swapchain_next_image_index(
    vulkan_context* context,
    vulkan_swapchain* swapchain,
    u64 timeout,
    VkSemaphore image_available_semaphore,
    VkFence fence,
    u32* out_image_index);

void vulkan_swapchain_present (
    vulkan_context* context,
    vulkan_swapchain* swapchain,
    VkQueue graphics_queue,
    VkQueue present_queue,
    VkSemaphore render_complete_semaphore,
    u32 present_index);





#endif //BACKEND_ENGINE_SRC_RENDERER_VULKAN_VULKAN_SWAPCHAIN_H
