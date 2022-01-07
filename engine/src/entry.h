//
// Created by 12132 on 2022/1/7.
//

#ifndef BACKEND_ENGINE_SRC_ENTRY_H
#define BACKEND_ENGINE_SRC_ENTRY_H

#include "core/application.h"
#include "core/logger.h"
#include "game_types.h"
#include "core/kmemory.h"

extern b8 create_game(game* out_game);

int main(void)
{
    /// init memory subsystem
    initialize_memory();

    /// init subsystem
    initialize_logging();

    game game_inst;
    if(!create_game(&game_inst)){
        KFATAL("could not create game.");
        return -1;
    }

    if(!game_inst.render || !game_inst.update || !game_inst.initialize || !game_inst.on_resize){
        KFATAL("The game function pointer must be initialized.");
        return -2;
    }

    /// init
    if(!application_create(&game_inst)){
        KFATAL("create application failed.");
        return -3;
    }

    if(!application_run()){
        KINFO("Application not shutdown gracefully");
        return -4;
    }

    shutdown_memory();
    return 0;
}



#endif //BACKEND_ENGINE_SRC_ENTRY_H
