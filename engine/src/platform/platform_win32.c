//
// Created by 12132 on 2022/1/6.
//

#include "platform.h"

#if KPLATFORM_WINDOWS
#include "core/logger.h"
#include "container/darray.h"
#include "core/event.h"
#include "core/input.h"
#include <windows.h>
#include <windowsx.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>
#include "renderer/vulkan/vulkan_types.h"

LRESULT CALLBACK win32_process_message(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
    switch (message) {
        case WM_ERASEBKGND:return 1;
        case WM_CLOSE:{
            event_context data = {};
            event_fire(EVENT_CODE_APPLICATION_QUIT,0,data);
            return 0;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            
            return 0;
        case WM_SIZE: {
            /// RECT rect;
            /// GetClientRect(hwnd,&rect);
            /// u32 width = rect.right- rect.left;
            /// u32 height = rect.bottom - rect.top;

            //todo fire an event for window resize

        }break;

        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP: {
            b8 pressed  =(message == WM_KEYDOWN || message == WM_SYSKEYDOWN);
            keys key=(u16)wparam;
            /// TODO input processing
            input_process_key(key,pressed);
            return 0;
        }
        case WM_MOUSEMOVE: {
            /// mouse move
            i32 x_pos = GET_X_LPARAM(lparam);
            i32 y_pos = GET_Y_LPARAM(lparam);

            /// TODO input processing
            input_process_mouse_move(x_pos,y_pos);
        }break;

        case WM_MOUSEHWHEEL: {
            i32 z_delta = GET_WHEEL_DELTA_WPARAM(wparam);
            if (z_delta != 0) {
                z_delta = (z_delta < 0) ? -1 : 1;
                input_process_mouse_wheel(z_delta);
            }

        }break;

        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP: {
            b8 pressed = message== WM_LBUTTONDOWN || message==WM_MBUTTONDOWN || message==WM_RBUTTONDOWN;
            /// todo input process
            buttons mouse_button = BUTTON_MAX_BUTTONS;
                switch (message) {
                case WM_LBUTTONDOWN:
                case WM_LBUTTONUP:
                    mouse_button= BUTTON_LEFT;
                    break;
                case WM_RBUTTONDOWN:
                case WM_RBUTTONUP:
                    mouse_button= BUTTON_RIGHT;
                    break;
                case WM_MBUTTONDOWN:
                case WM_MBUTTONUP:
                    mouse_button= BUTTON_MIDDLE;
                    break;
            }
            if(mouse_button != BUTTON_MAX_BUTTONS){
                input_process_button(mouse_button,pressed);
            }

        }break;
    }
    return DefWindowProcA(hwnd,message,wparam,lparam);
}

typedef struct internal_state {
    HINSTANCE h_instance;
    HWND hwnd;
    VkSurfaceKHR surface;
} internal_state;

static f64 clock_frequency;
static LARGE_INTEGER start_time;

b8 platform_startup(platform_state *plat_state,
                    const char *application_name,
                    i32 x, i32 y,
                    i32 width, i32 height)
{
    plat_state->internal_state = malloc(sizeof(internal_state));
    internal_state *state = (internal_state *) (plat_state->internal_state);
    state->h_instance = GetModuleHandleA(0);

    /// SetUp and register window class
    HICON icon = LoadIconA(state->h_instance, IDI_APPLICATION);
    WNDCLASS wc;
    memset(&wc, 0, sizeof(WNDCLASS));
    wc.style = CS_DBLCLKS;
    wc.lpfnWndProc = win32_process_message;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = state->h_instance;
    wc.hIcon = icon;
    wc.hCursor = LoadCursorA(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszClassName = "DefaultWindow";

    if (!RegisterClassA(&wc)) {
        MessageBoxA(0, "window Register failed", "Error", MB_ICONEXCLAMATION | MB_OK);
        return false;
    }

    /// create window
    u32 client_x = x;
    u32 client_y = y;
    u32 client_width = width;
    u32 client_height = height;

    u32 window_x = client_x;
    u32 window_y = client_y;
    u32 window_width = client_width;
    u32 window_height = client_height;

    u32 window_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;
    u32 window_ex_style = WS_EX_APPWINDOW;

    window_style |= WS_MAXIMIZEBOX;
    window_style |= WS_MINIMIZEBOX;
    window_style |= WS_THICKFRAME;

    RECT border_rect = {0, 0, 0, 0};
    AdjustWindowRectEx(&border_rect, window_style, 0, window_ex_style);

    ///
    window_x += border_rect.left;
    window_y += border_rect.top;

    //
    window_width += border_rect.right - border_rect.left;
    window_height += border_rect.bottom - border_rect.top;

    HWND handle = CreateWindowExA(window_ex_style, "DefaultWindow",
                                  application_name, window_style,
                                  window_x, window_y, window_width, window_height,
                                  0, 0, state->h_instance, 0);

    if (handle == 0) {
        MessageBoxA(NULL, "Window creation failed,", "Error!", MB_ICONEXCLAMATION | MB_OK);
        KFATAL("Window creation failed!");
        return false;
    } else {
        state->hwnd = handle;
    }

    /// show window
    b32 should_active = 1;
    i32 should_window_command_flags = should_active ? SW_SHOW : SW_SHOWNOACTIVATE;
    ShowWindow(state->hwnd, should_window_command_flags);

    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    clock_frequency = 1.0 / (f64) frequency.QuadPart;
    QueryPerformanceCounter(&start_time);

    return true;
}

void platform_shutdown(platform_state *plat_state)
{
    internal_state *state = (internal_state *) (plat_state->internal_state);
    if (state->hwnd) {
        DestroyWindow(state->hwnd);
        state->hwnd = 0;
    }
}

b8 platform_pump_message(platform_state *state)
{
    MSG message;
    while (PeekMessageA(&message, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&message);
        DispatchMessageA(&message);
    }
    return true;
}

void *platform_allocate(u64 size, b8 aligned)
{
    return malloc(size);
}

void platform_free(void *block, b8 aligned)
{
    free(block);
}

void *platform_zero_memory(void *block, u64 size)
{
    memset(block, 0, size);
    return block;
}

void *platform_copy_memory(void *dst, const void *src, u64 size)
{
    return memcpy(dst, src, size);
}

void *platform_set_memory(void *dst, i32 value, u64 size)
{
    return memset(dst, value, size);
}

void platform_console_write(const char *message, u8 colour)
{
    HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    static u8 levels[6] = {64, 4, 6, 2, 1, 9};
    SetConsoleTextAttribute(console_handle, levels[colour]);

    OutputDebugStringA(message);
    u64 length = strlen(message);
    LPDWORD num_written = 0;
    WriteConsoleA(console_handle, message, (DWORD) length, num_written, 0);

}

void platform_console_write_error(const char *message, u8 colour)
{
    HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    static u8 levels[6] = {64, 4, 6, 2, 1, 9};
    SetConsoleTextAttribute(console_handle, levels[colour]);

    OutputDebugStringA(message);
    u64 length = strlen(message);
    LPDWORD num_written = 0;
    WriteConsoleA(console_handle, message, (DWORD) length, num_written, 0);
}

f64 platform_get_absolute_time()
{
    LARGE_INTEGER now_time;
    QueryPerformanceCounter(&now_time);
    return (f64) now_time.QuadPart * clock_frequency;
}

void platform_sleep(u64 ms)
{
    Sleep(ms);
}

void platform_get_require_extension_names(const char*** array)
{
    darray_push(*array,&"VK_KHR_win32_surface");
}

b8 platform_create_vulkan_surface(struct platform_state* plat_state,
    struct vulkan_context* context)
{
    internal_state *state = (internal_state *) (plat_state->internal_state);
    VkWin32SurfaceCreateInfoKHR create_info ={};
    create_info.sType =VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    create_info.hinstance  = state->h_instance;
    create_info.hwnd = state->hwnd;
    VkResult res = vkCreateWin32SurfaceKHR(context->instance,
                                      &create_info,
                                      context->allocator,
                                      &state->surface);
    if (res != VK_SUCCESS) {
        KFATAL("Vulkan Surface create failed");
        return false;

    }
    context->surface = state->surface;
    return true;
}
#endif

