
set(platform_src )

if (CMAKE_SYSTEM_NAME MATCHES "Window")
    list(APPEND
            platform_src src/platform/platform_win32.c)
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    list(APPEND platform_src
            src/platform/platform_linux.c)
endif ()
find_package(Vulkan REQUIRED)

set(render_src src/renderer/vulkan/vulkan_backend.c)

message(STATUS ${CMAKE_BUILD_TYPE})


if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    message(STATUS "Debug build")
    add_compile_definitions("_DEBUG")
elseif(${CMAKE_BUILD_TYPE} STREQUAL "Release")
    message(STATUS "Release build")
endif ()

set(HEADERS
        src/core/logger.h
        src/core/asserts.h
        src/core/input.h
        src/core/kstring.h
        src/core/event.h
        src/core/clock.h
        src/core.h

        src/core/clock.c
        src/core/application.c
        src/core/kmemory.c
        src/core/event.c
        src/core/input.c
        src/core/kstring.c

        src/platform/platform.h
        ${platform_src}
        src/container/darray.h
        src/container/darray.c

        src/renderer/renderer_types.h
        src/renderer/renderer_fronted.h
        src/renderer/renderer_fronted.c
        src/renderer/renderer_backend.h
        src/renderer/renderer_backend.c

        src/renderer/vulkan/vulkan_backend.h
        src/renderer/vulkan/vulkan_types.h
        src/renderer/vulkan/vulkan_device.h
        src/renderer/vulkan/vulkan_swapchain.h
        src/renderer/vulkan/vulkan_image.h
        src/renderer/vulkan/vulkan_renderpass.h
        src/renderer/vulkan/vulkan_command_buffer.h

        src/renderer/vulkan/vulkan_backend.c
        src/renderer/vulkan/vulkan_device.c
        src/renderer/vulkan/vulkan_swapchain.c
        src/renderer/vulkan/vulkan_image.c
        src/renderer/vulkan/vulkan_renderpass.c
        src/renderer/vulkan/vulkan_command_buffer.c
        ${render_src}

        )

set(SRCS src/core/logger.c)

add_library(engine ${SRCS} ${HEADERS})
target_link_libraries(engine PUBLIC Vulkan::Vulkan)

target_include_directories(engine PUBLIC src)
