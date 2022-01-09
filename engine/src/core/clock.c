//
// Created by 12132 on 2022/1/9.
//

#include "clock.h"
#include "platform/platform.h"

void clock_update(clock* clk)
{
    if(clk->start_time != 0) {
        clk->elapsed = platform_get_absolute_time() - clk->start_time;
    }
}

void clock_start(clock* clk)
{
    clk->start_time = platform_get_absolute_time();
    clk->elapsed = 0.0;
}

void clock_stop(clock* clk)
{
    clk->start_time = 0;
}
