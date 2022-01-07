//
// Created by 12132 on 2022/1/7.
//

#ifndef BACKEND_ENGINE_SRC_GAME_TYPES_H
#define BACKEND_ENGINE_SRC_GAME_TYPES_H
#include "core/application.h"

typedef struct game{
    application_config app_config;

    b8 (*initialize)(struct game* game_instance);

    b8 (*update)(struct game* game_instance,f32 delta_time);

    b8 (*render)(struct game* game_instance,f32 delta_time);

    void (*on_resize)(struct game* game_instance,u32 width,u32 height);

    void* state;
}game;




#endif //BACKEND_ENGINE_SRC_GAME_TYPES_H
