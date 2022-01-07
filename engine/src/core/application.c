//
// Created by 12132 on 2022/1/7.
//

#include "application.h"
#include "logger.h"
#include "kmemory.h"
#include "../game_types.h"
#include "../platform/platform.h"

typedef struct application_state{
    game* game_inst;
    b8 is_running;
    b8 is_suspend;
    platform_state platform;
    i16 width;
    i16 height;
    f64 last_time;

}application_state;

static b8 initialized = false;
static application_state app_state;

b8 application_create(game* game_inst)
{
    if(initialized) {
        KERROR("application_create failed call more than once.");
        return false;
    }

    app_state.game_inst = game_inst;

    /// todo
    initialize_logging();

    /// todo remove
    KDEBUG("This is test: %f", 3.1415926);
    KINFO("This is test: %f",  3.1415926);
    KERROR("This is test: %f", 3.1415926);
    KWARN("This is test: %f",  3.1415926);
    KTRACE("This is test: %f", 3.1415926);

    app_state.is_running = true;
    app_state.is_suspend = false;

    if(!platform_startup(&app_state.platform,
                       game_inst->app_config.name,
                       game_inst->app_config.start_pos_x,
                       game_inst->app_config.start_pos_y,
                       game_inst->app_config.start_width,
                       game_inst->app_config.start_height)) {
        return false;
    }

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
    KINFO(get_memory_usage_str());
    while (app_state.is_running){
        if( !platform_pump_message(&app_state.platform)){
            app_state.is_running = false;

        }
        if(!app_state.is_suspend){
            if(!app_state.game_inst->update(app_state.game_inst,(f32)0)) {
                KFATAL("Game Update Failed, shutdown...");
                app_state.is_running = false;
                break;
            }

            if(!app_state.game_inst->render(app_state.game_inst,(f32)0)){
                KFATAL("Render Failed, shutdown...");
                app_state.is_running = false;
                break;
            }

        }
    }

    app_state.is_running =false;
    platform_shutdown(&app_state.platform);
    return true;

}
