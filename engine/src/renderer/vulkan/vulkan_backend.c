//
// Created by 12132 on 2022/1/9.
//

#include "vulkan_backend.h"
#include "vulkan_device.h"
#include "core/logger.h"
#include "core/kstring.h"
#include "container/darray.h"
#include "vulkan_platform.h"

static vulkan_context context;
static VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData);

b8 vk_initialize(struct renderer_backend* backend, const char* app_name,struct platform_state* plat_state)
{
    //TODO custom allocator
    context.allocator = 0;

    VkApplicationInfo app_info = {};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pNext = NULL;
    app_info.pApplicationName = app_name;
    app_info.pEngineName = "No Engine";
    app_info.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    app_info.apiVersion = VK_MAKE_VERSION(1, 2, 0);

    const char **instance_required_extensions = darray_create(const char *);
    darray_push(instance_required_extensions,&VK_KHR_SURFACE_EXTENSION_NAME);   /// Generic surface extension
    platform_get_require_extension_names(&instance_required_extensions);
#ifdef _DEBUG
    darray_push(instance_required_extensions,&VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    KDEBUG("Required Instance Extensions:");
    u64 length= darray_length(instance_required_extensions);
    for (u64 i = 0; i < length ;++i) {
        KDEBUG(instance_required_extensions[i]);
    }
#endif

    const char** instance_required_layer_names =0;
    u16 instance_required_layer_count = 0;

#ifdef _DEBUG
    KINFO("Validation layers enabled, Enumerating ...");
    instance_required_layer_names = darray_create(const char*);
    darray_push(instance_required_layer_names,&"VK_LAYER_KHRONOS_validation");
    instance_required_layer_count = darray_length(instance_required_layer_names);

    u32 available_count = 0;
    vkEnumerateInstanceLayerProperties(&available_count,0);
    VkLayerProperties* available_layers = darray_reserve(VkLayerProperties,available_count);
    vkEnumerateInstanceLayerProperties(&available_count,available_layers);

    for (u32 i = 0; i < instance_required_layer_count; ++i) {
        KINFO("Search for layer: %s... ",instance_required_layer_names[i]);
        b8 found =false;
        for (u32 j = 0; j < available_count; ++j) {
            if (string_equal(instance_required_layer_names[i],available_layers[j].layerName)){
                found = true;
                KINFO("Found.");
                break;
            }
        }
        if (!found) {
            KFATAL("Required validation layer is missing: %s",instance_required_layer_names[i]);
            return false;
        }
    }
    KINFO("All required validation layers are present");
#endif

    VkInstanceCreateInfo instanceCI = {};
    instanceCI.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCI.pApplicationInfo = &app_info;
    instanceCI.enabledExtensionCount = darray_length(instance_required_extensions);
    instanceCI.ppEnabledExtensionNames = instance_required_extensions;
    instanceCI.enabledLayerCount = instance_required_layer_count;
    instanceCI.ppEnabledLayerNames = instance_required_layer_names;

    VkResult res = vkCreateInstance(&instanceCI, context.allocator, &context.instance);
    if (res != VK_SUCCESS) {
        KERROR("vkCreateInstance Failed error code: %u", res);
        return false;
    }
    KINFO("Vulkan Instance created.");

#ifdef _DEBUG
    KDEBUG("Create vulkan debugger");
    context.debug_messenger = 0;
    u32 log_severity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
                        VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT ;///
    VkDebugUtilsMessengerCreateInfoEXT debugCI={};
    debugCI.sType =VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugCI.pfnUserCallback = vk_debug_callback;
    debugCI.messageSeverity = log_severity;
    debugCI.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
        VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
    PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(context.instance,"vkCreateDebugUtilsMessengerEXT");
    KASSERT_MSG(func,"failed to create debug messenger");
    func(context.instance,&debugCI,context.allocator,&context.debug_messenger);
    KINFO("vulkan debug messenger created");
#endif
    /// create surface
    if (!platform_create_vulkan_surface(plat_state,&context)) {
        KERROR("create surface failed");
        return false;
    }


    if (!vulkan_device_create(&context)) {
        KERROR("Failed to create device");
        return false;
    }

    KINFO("vulkan render init successfully");
    return true;
}

void vk_shutdown(struct renderer_backend* backend)
{
#ifdef _DEBUG
    KDEBUG("Destroying vulkan debugger...");
    if (context.debug_messenger) {
        PFN_vkDestroyDebugUtilsMessengerEXT func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(
            context.instance,
            "vkDestroyDebugUtilsMessengerEXT"
            );
        func(context.instance,context.debug_messenger,context.allocator);
        context.debug_messenger = 0;
    }
#endif

    vulkan_device_destroy(&context);

    KDEBUG("Destroy Surface");
    if (context.surface){
        vkDestroySurfaceKHR(context.instance,context.surface,context.allocator);
        context.surface = 0;
    }


    KDEBUG("Destroy vulkan instance...");
    vkDestroyInstance(context.instance,context.allocator);
    context.instance = 0;
    context.allocator = 0;
}

void vk_resize(struct renderer_backend* backend,u16 width,u16 height)
{

}

b8 vk_begin_frame(struct renderer_backend* backend,float delta_time)
{
    return true;
}

b8 vk_end_frame(struct renderer_backend* backend,float delta_time)
{
    return true;
}

VKAPI_ATTR VkBool32 VKAPI_CALL vk_debug_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    switch (messageSeverity) {
    default:
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
        KERROR(pCallbackData->pMessage);break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
        KWARN(pCallbackData->pMessage);break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
        KINFO(pCallbackData->pMessage);break;
    case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
        KTRACE(pCallbackData->pMessage);break;
    }
    return VK_FALSE;
}

