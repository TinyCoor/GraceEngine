//
// Created by 12132 on 2022/1/3.
//

#ifndef BACKEND_SRC_RENDER_RENDERER_BACKEND_H
#define BACKEND_SRC_RENDER_RENDERER_BACKEND_H

#include "renderer_types.h"

struct platform_state;

b8 renderer_backend_create(renderer_backend_type type,
                             struct platform_state* plat_state,
                             renderer_backend* out_render_backend);

void renderer_backend_destroy(renderer_backend* backend);



#endif //BACKEND_SRC_RENDER_RENDERER_BACKEND_H
