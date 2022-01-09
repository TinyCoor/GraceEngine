//
// Created by 12132 on 2022/1/9.
//

#ifndef BACKEND_ENGINE_SRC_RENDERER_VULKAN_VULKAN_TYPES_H
#define BACKEND_ENGINE_SRC_RENDERER_VULKAN_VULKAN_TYPES_H
#include "defines.h"
#include "core/asserts.h"
#include <vulkan/vulkan.h>

#define VK_CHECK(expr)                  \
 do {                                   \
    KASSERT(expr == VK_SUCCESS);        \
}while(0)

typedef struct vulkan_swapchain_support_info {
    VkSurfaceCapabilitiesKHR capabilities;
    u32 format_count;
    VkSurfaceFormatKHR* support_formats;
    uint32_t present_mode_count;
    VkPresentModeKHR* support_present_modes;
} vulkan_swapchain_support_info;

typedef enum vulkan_command_buffer_state{
    COMMAND_BUFFER_STATE_READY,
    COMMAND_BUFFER_STATE_RECORDING,
    COMMAND_BUFFER_STATE_IN_RENDER_PASS,
    COMMAND_BUFFER_STATE_RECORDING_ENDED,
    COMMAND_BUFFER_STATE_SUBMITTED,
    COMMAND_BUFFER_STATE_NOT_ALLOCATED,
}vulkan_command_buffer_state;

typedef struct vulkan_command_buffer {
    VkCommandBuffer handle;

    vulkan_command_buffer_state state;

}vulkan_command_buffer;

typedef enum vulkan_render_pass_state{
    READY,
    RECORDING,
    IN_RENDER_PASS,
    RECORDED_END,
    SUBMITTED,
    NOT_ALLOCATED,

} vulkan_render_pass_state;

typedef struct vulkan_renderpass{
    VkRenderPass handle;
    f32 x,y,w,h;
    f32 r,g,b,a;
    f32 depth;
    u32 stencil;
    vulkan_render_pass_state state;
}vulkan_renderpass;


typedef struct vulkan_image{
    VkImage handle;
    VkDeviceMemory memory;
    VkImageView view;
    u32 width;
    u32 height;

} vulkan_image;

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
    VkCommandPool graphics_command_pool;

    VkFormat depth_format;

    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;
    VkPhysicalDeviceMemoryProperties memory;
    vulkan_swapchain_support_info swapchain_support;
}vulkan_device;

typedef struct vulkan_swapchain {
    VkSurfaceFormatKHR  image_format;
    u8 max_frame_in_flight;
    VkSwapchainKHR handle;
    u32 image_count;
    VkImage* images;
    VkImageView*  views;
    vulkan_image depth_attachment; /// todo move to frame_buffer
} vulkan_swapchain;


typedef struct vulkan_context{
    VkInstance  instance;
    VkAllocationCallbacks* allocator;
    vulkan_device device;
    VkSurfaceKHR surface;

    ///当前帧缓冲的大小
    u32 frame_buffer_width;
    u32 frame_buffer_height;

    vulkan_swapchain swapchain;
    u32 image_index;
    u32 current_frame;
    b8 recreating_swapchain;

    vulkan_renderpass main_renderpass;

    /// darray
    vulkan_command_buffer* graphics_command_buffers;

#ifdef _DEBUG
    VkDebugUtilsMessengerEXT debug_messenger;
#endif

    i32 (*find_memory_index)(u32 type_filter,u32 properties);
}vulkan_context;




#endif //BACKEND_ENGINE_SRC_RENDERER_VULKAN_VULKAN_TYPES_H
