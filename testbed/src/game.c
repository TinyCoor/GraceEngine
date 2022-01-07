//
// Created by 12132 on 2022/1/7.
//
#include "game.h"
#include <core/logger.h>

b8 game_initialize(struct game* game_instance)
{
    KDEBUG("game_initialize() called!");
    return true;
}

b8 game_update(struct game* game_instance,f32 delta_time)
{
//    KDEBUG("game_update() called!");
    return true;
}

b8 game_render(struct game* game_instance,f32 delta_time)
{
//    KDEBUG("game_render() called!");
    return true;
}

void game_on_resize(struct game* game_instance,u32 width,u32 height)
{
//    KDEBUG("game_on_resize() called!");
}