//
// Created by 12132 on 2022/1/9.
//

#ifndef BACKEND_ENGINE_SRC_RENDERER_VULKAN_VULKAN_COMMAND_BUFFER_H
#define BACKEND_ENGINE_SRC_RENDERER_VULKAN_VULKAN_COMMAND_BUFFER_H

#include "vulkan_types.h"

void vulkan_command_buffer_allocate (
    vulkan_context* context,
    VkCommandPool pool,
    b8 is_primary,
    vulkan_command_buffer* command_buffer);

void vulkan_command_buffer_free(
    vulkan_context* context,
    VkCommandPool pool,
    vulkan_command_buffer* command_buffer);

void vulkan_command_buffer_begin (
    vulkan_command_buffer* command_buffer,
    b8 is_single_use,
    b8 is_renderpass_continue,
    b8 is_simultaneous_use);

void vulkan_command_buffer_end(vulkan_command_buffer* command_buffer);

void vulkan_command_buffer_update_submitted(vulkan_command_buffer* command_buffer);

void vulkan_command_buffer_reset(vulkan_command_buffer* command_buffer);

void vulkan_command_buffer_allocate_and_begin_single_use(
    vulkan_context* context,
    VkCommandPool pool,
    vulkan_command_buffer* command_buffer);

void vulkan_command_buffer_end_single_use (
    vulkan_context* context,
    VkCommandPool pool,
    vulkan_command_buffer* command_buffer,
    VkQueue queue);





#endif //BACKEND_ENGINE_SRC_RENDERER_VULKAN_VULKAN_COMMAND_BUFFER_H
