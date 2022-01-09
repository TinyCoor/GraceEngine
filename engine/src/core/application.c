//
// Created by 12132 on 2022/1/7.
//

#include "application.h"
#include "logger.h"
#include "kmemory.h"
#include "game_types.h"
#include "platform/platform.h"
#include "core/event.h"
#include "core/clock.h"
#include "renderer/renderer_fronted.h"
#include "input.h"

typedef struct application_state{
    game* game_inst;
    b8 is_running;
    b8 is_suspend;
    platform_state platform;
    i16 width;
    i16 height;
    clock clk;
    f64 last_time;

}application_state;

static b8 initialized = false;
static application_state app_state;

b8 application_on_event(u16 code,void* sender,void* listener_inst,event_context context);
b8 application_on_key(u16 code,void* sender,void* listener_inst,event_context context);


b8 application_create(game* game_inst)
{
    if(initialized) {
        KERROR("application_create failed call more than once.");
        return false;
    }

    app_state.game_inst = game_inst;

    /// todo
    initialize_logging();

    // init input system
    input_initialize();

    /// todo remove
    KDEBUG("This is test1: %f", 3.1415926);
    KINFO("This is test2: %f",  3.1415926);
    KERROR("This is test3: %f", 3.1415926);
    KWARN("This is test4: %f",  3.1415926);
    KTRACE("This is test5: %f", 3.1415926);

    app_state.is_running = true;
    app_state.is_suspend = false;

    if(!event_initialize()){
        KERROR("Event Subsystem init failed Application stop");
        return false;
    }

    event_register(EVENT_CODE_APPLICATION_QUIT,0,application_on_event);
    event_register(EVENT_CODE_KEY_RELEASED,0,application_on_key);
    event_register(EVENT_CODE_KEY_PRESSED, 0,application_on_key);


    if(!platform_startup(&app_state.platform,
                       game_inst->app_config.name,
                       game_inst->app_config.start_pos_x,
                       game_inst->app_config.start_pos_y,
                       game_inst->app_config.start_width,
                       game_inst->app_config.start_height)) {
        return false;
    }

    /// init render
    if(!renderer_initialize(game_inst->app_config.name,&app_state.platform)){
        KFATAL("Failed to initialized render aborting...");
        return false;
    }

    /// init game
    if(!app_state.game_inst->initialize(app_state.game_inst)){
        KERROR("Failed to init game.");
        return false;
    }

    app_state.game_inst->on_resize(app_state.game_inst,app_state.width,app_state.height);

    initialized= true;
    return true;
}


b8 application_run()
{
    clock_start(&app_state.clk);
    clock_update(&app_state.clk);

    app_state.last_time = app_state.clk.elapsed;
    f64 running_time = 0;
    f64 frame_count = 0;
    u8 target_frame_seconds = 1.0 / 60;


    KINFO(get_memory_usage_str());
    while (app_state.is_running){
        if(!platform_pump_message(&app_state.platform)){
            app_state.is_running = false;

        }
        if(!app_state.is_suspend){
            clock_update(&app_state.clk);
            f64 current_time = app_state.clk.elapsed;
            f64 delta = (current_time- app_state.last_time);
            f64 frame_start_time = platform_get_absolute_time();

            if(!app_state.game_inst->update(app_state.game_inst,(f32)delta)) {
                KFATAL("Game Update Failed, shutdown...");
                app_state.is_running = false;
                break;
            }

            if(!app_state.game_inst->render(app_state.game_inst,(f32)delta)){
                KFATAL("Render Failed, shutdown...");
                app_state.is_running = false;
                break;
            }

            /// TODO refractor
            render_packet  packet;
            packet.delta_time = delta;
            renderer_draw_frame(&packet);


            f64 frame_end_time = platform_get_absolute_time();
            f64 frame_elapsed_time =frame_end_time - frame_start_time;
            running_time += frame_elapsed_time;
            f64 remaining_seconds= target_frame_seconds - frame_elapsed_time;

            if(remaining_seconds > 0){
                u64 remaining_ms = (remaining_seconds * 1000);

                b8 limit_frames = false;
                //// give back to os
                if(remaining_ms > 0 && limit_frames) {
                    platform_sleep(remaining_ms - 1);
                }
                frame_count++;
            }

            input_update(delta);

            app_state.last_time = current_time;

        }
    }

    app_state.is_running =false;

    event_unregister(EVENT_CODE_APPLICATION_QUIT,0,application_on_event);
    event_unregister(EVENT_CODE_KEY_RELEASED,0,application_on_key);
    event_unregister(EVENT_CODE_KEY_PRESSED, 0,application_on_key);

    input_shutdown();
    event_shutdown();

    renderer_shutdown();

    platform_shutdown(&app_state.platform);

    return true;

}

b8 application_on_event(u16 code,void* sender,void* listener_inst,event_context context)
{
    switch (code) {
        case EVENT_CODE_APPLICATION_QUIT:{
            KINFO("EVENT_CODE_APPLICATION_QUIT pressed shutdown.\n");
            app_state.is_running = false;
        }
    }
    return false;
}

b8 application_on_key(u16 code,void* sender,void* listener_inst,event_context context)
{
    if(code == EVENT_CODE_KEY_PRESSED) {
        u16 key_code = context.data.u16[0];
        if(key_code == KEY_ESCAPE){
            event_context data ={};
            event_fire(EVENT_CODE_APPLICATION_QUIT,0,data);
            return true;
        } else if(key_code ==KEY_A) {
            KDEBUG("Explicit - A pressed");
        }else {
            KDEBUG("%c key is pressed",key_code);
        }
    } else if(code == EVENT_CODE_KEY_RELEASED) {
        u16 key_code = context.data.u16[0];
        if (key_code == KEY_B){
            KDEBUG("Explicit - B released");
        } else {
            KDEBUG("%c key is released",key_code);
        }
    }
    return false;
}
