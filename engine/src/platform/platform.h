//
// Created by 12132 on 2022/1/6.
//

#ifndef BACKEND_ENGINE_SRC_PLATFORM_PLATFORM_H
#define BACKEND_ENGINE_SRC_PLATFORM_PLATFORM_H
#include "../defines.h"

typedef struct platform_state{
    void* internal_state;
}platform_state;

b8 platform_startup(platform_state* plat_state,
                    const char* application_name,
                    i32 x, i32 y,
                    i32 width, i32 height);

void platform_shutdown(platform_state* state);

b8 platform_pump_message(platform_state* state);

void* platform_allocate(u64 size,b8 aligned);
void  platform_free(void* block,b8 aligned);
void* platform_zero_memory(void* block,u64 size);
void* platform_copy_memory(void* dst,const void* src,u64 size);
void* platform_set_memory(void* dst,i32 value,u64 size);

void platform_console_write(const char* message,u8 colour);
void platform_console_write_error(const char* message,u8 colour);

f64 platform_get_absolute_time();

void platform_sleep(u64 ms);





#endif //BACKEND_ENGINE_SRC_PLATFORM_PLATFORM_H
