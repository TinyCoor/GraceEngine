//
// Created by 12132 on 2022/1/9.
//

#ifndef BACKEND_ENGINE_SRC_CORE_CLOCK_H
#define BACKEND_ENGINE_SRC_CORE_CLOCK_H
#include "defines.h"
typedef struct clock{
    f64 start_time;
    f64 elapsed;
}clock;


void clock_update(clock* clk);

void clock_start(clock* clk);

void clock_stop(clock* clk);



#endif //BACKEND_ENGINE_SRC_CORE_CLOCK_H
