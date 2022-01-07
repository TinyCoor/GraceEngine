//
// Created by 12132 on 2022/1/7.
//

#ifndef BACKEND_ENGINE_SRC_CORE_APPLICATION_H
#define BACKEND_ENGINE_SRC_CORE_APPLICATION_H

#include "../defines.h"

typedef struct game game;

typedef struct application_config{
    i16 start_pos_x;
    i16 start_pos_y;
    i16 start_width;
    i16 start_height;
    char* name;
}application_config;


b8 application_create(game* game_inst);

b8 application_run();



#endif //BACKEND_ENGINE_SRC_CORE_APPLICATION_H
