//
// Created by 12132 on 2022/1/7.
//

#ifndef BACKEND_TESTBED_SRC_GAME_H
#define BACKEND_TESTBED_SRC_GAME_H
#include <game_types.h>
#include <defines.h>

typedef struct game_state{
    f32 delta_time;
}game_state;


b8 game_initialize(struct game* game_instance);

b8 game_update(struct game* game_instance,f32 delta_time);

b8 game_render(struct game* game_instance,f32 delta_time);

void game_on_resize(struct game* game_instance,u32 width,u32 height);


#endif //BACKEND_TESTBED_SRC_GAME_H
