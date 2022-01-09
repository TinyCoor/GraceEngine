//
// Created by 12132 on 2022/1/3.
//

#include "renderer_fronted.h"
#include "renderer_backend.h"
#include "core/kmemory.h"
#include "core/logger.h"
#include "defines.h"

static renderer_backend * backend = 0;

b8 renderer_initialize(const char* app_name,struct platform_state* plat_state)
{
    backend = kallocate(sizeof(renderer_backend),MEMORY_TAG_RENDERER);

    /// todo make this configurable
    renderer_backend_create(backend_type_vulkan,plat_state,backend);
    backend->frame_number = 0;

    if(!backend->initialize(backend,app_name,plat_state)) {
        KFATAL("Render Backend create failed. shutdown ... ");
        return false;
    }

    return true;
}

void renderer_shutdown()
{
    backend->shutdown(backend);
    kfree(backend, sizeof(renderer_backend),MEMORY_TAG_RENDERER);
}

b8 render_begin_frame(float delta_time)
{
    b8 res = backend->begin_frame(backend,delta_time);
    return res;
}

b8 render_end_frame(float delta_time)
{
    b8 res = backend->end_frame(backend,delta_time);
    backend->frame_number++;
    return res;
}

b8 renderer_draw_frame(render_packet* packet)
{
    if(render_begin_frame(packet->delta_time)){

        ///
        b8 res = backend->end_frame(backend,packet->delta_time);
        if(!res){
            KERROR("render end frame failed. application shutdown...");
            return false;
        }
    }
    return true;
}

void renderer_on_resize(u16 width, u16 height)
{
    backend->resize(backend,width,height);
}



