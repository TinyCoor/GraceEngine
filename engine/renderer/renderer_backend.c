//
// Created by 12132 on 2022/1/3.
//
#include <stdbool.h>
#include "renderer_backend.h"

bool renderer_backend_create(renderer_backend_type type,
                             struct platform_state* plat_state,
                             renderer_backend* out_render_backend)
{
    out_render_backend->plat_state = plat_state;

    if (type == backend_type_vulkan) {
        /// todo fill


        return true;
    }


    return false;
}


void renderer_backend_destroy(renderer_backend* backend)
{
    backend->plat_state = 0;
    backend->initialize = 0;
    backend->shutdown = 0;
    backend->begin_frame = 0;
    backend->end_frame = 0;
    backend->resize = 0;
}
