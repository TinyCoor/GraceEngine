//
// Created by 12132 on 2022/1/3.
//

#include <stdbool.h>
#include <malloc.h>
#include "renderer_fronted.h"
#include "renderer_backend.h"

static renderer_backend * backend = 0;

bool renderer_initialize(const char* app_name,struct platform_state* plat_state)
{
    backend = malloc(sizeof(renderer_backend));

    /// todo make this configurable
    renderer_backend_create(backend_type_vulkan,plat_state,backend);
    backend->frame_number = 0;

    if(backend->initialize(backend,app_name,plat_state)) {
        /// todo log
        return false;
    }

    return true;
}

void renderer_shutdown()
{
    backend->shutdown(backend);
    free(backend);
}

bool render_begin_frame(float delta_time)
{
    bool res = backend->begin_frame(backend,delta_time);
    return res;
}

bool render_end_frame(float delta_time)
{
    bool res = backend->end_frame(backend,delta_time);
    backend->frame_number++;
    return res;
}


bool renderer_draw_frame(render_packet* packet)
{
    if(render_begin_frame(packet->delta_time)){

        bool res = backend->end_frame(backend,packet->delta_time);
        if(!res){
            return false;
        }
    }
    return true;
}

void renderer_on_resize(uint16_t width,uint16_t height)
{

}



