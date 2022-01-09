//
// Created by 12132 on 2022/1/9.
//

#ifndef BACKEND_ENGINE_SRC_RENDERER_VULKAN_VULKAN_BACKEND_H
#define BACKEND_ENGINE_SRC_RENDERER_VULKAN_VULKAN_BACKEND_H
#include "defines.h"
#include "renderer/renderer_backend.h"
struct platform_state;

b8 vk_initialize(struct renderer_backend* backend, const char* app_name,struct platform_state* plat_state);

void vk_shutdown(struct renderer_backend* backend);

void vk_resize(struct renderer_backend* backend,u16 width,u16 height);

b8 vk_begin_frame(struct renderer_backend* backend,float delta_time);

b8 vk_end_frame(struct renderer_backend* backend,float delta_time);

#endif //BACKEND_ENGINE_SRC_RENDERER_VULKAN_VULKAN_BACKEND_H
