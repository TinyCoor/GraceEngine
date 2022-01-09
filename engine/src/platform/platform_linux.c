//
// Created by 12132 on 2022/1/7.
//
#include "platform.h"


//#if KPLATFORM_LINUX
#include "defines.h"
#include "core/input.h"
#include "core/event.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <xcb.h>
#include <x11/keysym.h>
#include <x11/XKBlib.h>
#include <x11/xlib.h>
#include <x11/xlib-xcb.h>
#include <sys/time.h>

#include "../core/logger.h"

#if _POSIX_C_SOURCE >= 199309L
#include <time.h>
#else
#include <unistd.h>
#endif

typedef struct internal_state{
    Display* display;
    xcb_connection_t* connection;
    xcb_window_t* window;
    xcb_screen_t* screen;
    xcb_atom_t wm_protocols;
    xcb_atom_t wm_delete_win;
}internal_state;

b8 platform_startup(platform_state* plat_state,
                    const char* application_name,
                    i32 x, i32 y,
                    i32 width, i32 height)
{
    plat_state->internal_state = malloc(sizeof(internal_state));
    internal_state *state = (internal_state *) (plat_state->internal_state);

    state->display = xOpenDisplay(NULL);

    XAutoRepeatOff(state->display);

    state->connection = XGetXCBConnection(state->display);

    if(xcb_connection_has_error(state->connection)){
        KFATAL("Failed to connect to server via xcb.");
        return false;
    }

    const struct xcb_setup_t* setup = xcb_get_setup(state->connection);

    xcb_screent_iterator_t it = xcb_setup_root_iterator(setup)
    int screen_p = 0;
    for(i32 s= screen_p;s >0 ;s--){
        xcb_screen_next(&it);
    }

    state->screen =it.data;

    state->window = xcb_generate_id(state->connection);

    u32 event_mask =XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;

    u32 event_values = XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
            XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE |
            XCB_EVENT_MASK_EXPOSURE | XCB_EVENT_MASK_POINTER_MOTION |
            XCB_EVENT_MASK_STRUCTURE_NOTIFY;

    u32 value_list[] = {state->screen->black_pixel, event_values};

    xcb_void_cookie_t* cookie = xcb_create_window(
        state->connection,
        XCB_COPY_FROM_PARENT,
        state->window;
        state->screen->root,
        x,y,width,height,
        0,XCB_WINDOW_CLASS_INPUT_OUTPUT,
        state->screen->root_visual,
        event_mask,
        value_list
        );

    xcb_change_property(
        state->connection,
        XCB_PROP_MODE_REPLACE,
        state->window,
        XCB_ATOM_WM_NAME,
        XCB_ATOM_STRING,
        8,
        strlen(application_name),
        application_name
        );

    xcb_internal_atom_cookie_t wm_delete_cookie = xcb_intern_atom(
        state->connection,
        0,
        strlen("WM_DELETE_WINDOW"),
        "WM_DELETE_WINDOW"
        );
    xcb_internal_atom_cookie_t wm_protocols_cookie = xcb_intern_atom(
        state->connection,
        0,
        strlen("WM_PROTOCOLS"),
        "WM_PROTOCOLS"
        );

     xcb_internal_atom_reply_t wm_delete_reply = xcb_intern_atom_reply(
        state->connection,
        0,
        wm_delete_cookie ,
        NULL
        );

    xcb_internal_atom_reply_t wm_protocols_reply = xcb_intern_atom_reply(
        state->connection,
        0,
        wm_protocols_cookie
       NULL
        );

    state->wm_delete_win = wm_delete_reply->atom;
    state->wm_protocols = wm_protocols_reply->atom;

      xcb_change_property(
        state->connection,
        XCB_PROP_MODE_REPLACE,
        state->window,
        wm_protocols_reply->atom,
        4,32,1,&wm_delete_reply->atom
        );

    xcb_map_window(state->connection,state->window);

    i32 stream_result = xcb_flush(state->connection);
    if(stream_result <= 0) {
        KFATAL("An error occurred when flush strean: %d",stream_result);
        return false;
    }

    return true;
}

void platform_shutdown(platform_state* state)
{
    internal_state *state = (internal_state *) (plat_state->internal_state);

    XAutoRepeatOn(state->display);

    xcb_destroy_window(state->connection,state->window);

}

b8 platform_pump_message(platform_state* plat_state)
{
    internal_state *state = (internal_state *) (plat_state->internal_state);

    xcb_generic_event_t *event;
    xcb_client_message_event_t* cm;
    b8 quit_flagged = false;
    while(event != 0) {
        event = xcb_poll_for_event(state->connection);
        if(event == 0){
            break;
        }

        switch(event->response_type & ~0x80) {
            case XCB_KEY_PRESS:
            case XCB_KEY_RELEASE:{
                xcb_key_press_event_t* t = (xcb_key_press_event_t*)event;
                b8 pressed = event->response_type = XCB_KEY_PRESS;
                xcb_keycode_t code = kb_event->detail;
                KeySym keysym= XkbKeycodeToKeySym(state->display,
                                            (KeyCode)code,
                                            0,
                                            code & ShiftMask ? 1: 0);

                keys  key = translate_keycode(keysym);

                // input
                input_process_key(key,pressed);
            }break;
            case XCB_BUTTON_PRESS:
            case XCB_BUTTON_RELEASE:{

            }break;
            case XCB_MOTION_NOTIFY:{

            }break;
            case XCB_CONFIGURE_NOTIIFY{

            }break;
            case XCB_CLINET_MESSAGE:{
                cm = (xcb_clinet_message_t*)event;

                if(cm->data32[0] == state->wm_delete_win){
                    quit_flagged = true;
                }
            }break;
            default:
                break
        }
        free(event);
    }
    return !quit_flagged;
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


void platform_console_write(const char* message,u8 colour)
{
    const char* colour_strings[] ={"0;41", "1;31", "1;33", "1;32", "1;34", "1;30"};
    printf("\033[%sm%s\033[0m",colour_strings[colour],message);
}

void platform_console_write_error(const char* message,u8 colour)
{
    const char* colour_strings[] ={"0;41", "1;31", "1;33", "1;32", "1;34", "1;30"};
    printf("\033[%sm%s\033[0m",colour_strings[colour],message);
}

f64 platform_get_absolute_time()
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    return now.tv_sec + now.tv_nsec * 0.000000001
}


void platform_sleep(u64 ms)
{
#if _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = ms/1000;
    ts.tv_nsec = (ms % 1000) * 1000 * 1000;
    nanosleep(&ts,0);
#else
    if(ms >= 1000){
        sleep(ms/1000);
    }
    usleep((ms % 1000) *1000);
#endif

}

void platform_get_require_extension_names(const char*** array)
{
    darray_push(array,&"VK_KHR_xcb_surface");
}

///todo impl linux surface

//#endif
