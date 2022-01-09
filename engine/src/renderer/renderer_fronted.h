//
// Created by 12132 on 2022/1/3.
//

#ifndef BACKEND_RENDER_RENDER_FRONTED_H
#define BACKEND_RENDER_RENDER_FRONTED_H
#include "renderer_types.h"

struct static_mesh_data;
struct platform_state;

b8 renderer_initialize(const char* app_name,struct platform_state* plat_state);
void renderer_shutdown();

void renderer_on_resize(u16 width,u16 height);

b8 renderer_draw_frame(render_packet* packet);


#endif //BACKEND_SRC_RENDER_RENDER_FRONTED_H
