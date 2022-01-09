//
// Created by 12132 on 2022/1/9.
//

#ifndef BACKEND_ENGINE_SRC_RENDERER_VULKAN_VULKAN_IMAGE_H
#define BACKEND_ENGINE_SRC_RENDERER_VULKAN_VULKAN_IMAGE_H
#include "vulkan_types.h"

///todo make it a struct
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
    vulkan_image* out_image);

void vulkan_image_view_create(
    vulkan_context* context,
    VkFormat format,
    vulkan_image* image,
    VkImageAspectFlags aspect_flags);

void vulkan_image_destroy(vulkan_context* context, vulkan_image* image);

#endif //BACKEND_ENGINE_SRC_RENDERER_VULKAN_VULKAN_IMAGE_H
