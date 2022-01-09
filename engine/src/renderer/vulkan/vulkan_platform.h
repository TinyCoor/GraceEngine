//
// Created by 12132 on 2022/1/9.
//

#ifndef BACKEND_ENGINE_SRC_RENDERER_VULKAN_VULKAN_PLATFORM_H
#define BACKEND_ENGINE_SRC_RENDERER_VULKAN_VULKAN_PLATFORM_H
#include "defines.h"

void platform_get_require_extension_names(const char*** array);

b8  platform_create_vulkan_surface(struct platform_state* plat_state,
    struct vulkan_context* context);



#endif //BACKEND_ENGINE_SRC_RENDERER_VULKAN_VULKAN_PLATFORM_H
