#include "entry.h"
#include "game.h"
#include <malloc.h>

b8 create_game(game* out_game)
{
    out_game->app_config.name="app";
    out_game->app_config .start_pos_x =100 ;
    out_game->app_config.start_pos_y= 100;
    out_game->app_config.start_width = 1920;
    out_game->app_config.start_height = 1080;

    out_game->initialize = game_initialize ;
    out_game->render= game_render;
    out_game->update = game_update;
    out_game->on_resize = game_on_resize;

    /// todo memory subsystem
    out_game->state=kallocate(sizeof(game_state),MEMORY_TAG_GAME);
    return true;
}

