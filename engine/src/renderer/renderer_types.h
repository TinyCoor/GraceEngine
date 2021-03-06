//
// Created by 12132 on 2022/1/3.
//

#ifndef BACKEND_TYPES_H
#define BACKEND_TYPES_H

#include "defines.h"

typedef enum renderer_backend_type {
    backend_type_vulkan,
    backend_type_opengl,
    backend_type_directx,
}renderer_backend_type;

typedef struct renderer_backend{
    struct platform_state* plat_state;
    u64 frame_number;

    b8 (*initialize)(struct renderer_backend* backend, const char* app_name,struct platform_state* plat_state);

    void (*shutdown)(struct renderer_backend* backend);

    void (*resize)(struct renderer_backend* backend,u16 width,u16 height);

    b8 (*begin_frame)(struct renderer_backend* backend,float delta_time);

    b8 (*end_frame)(struct renderer_backend* backend,float delta_time);

}renderer_backend;

/// todo move to render backend
typedef struct render_packet{
    float delta_time;

}render_packet;



#endif //BACKEND_SRC_TYPES_H
