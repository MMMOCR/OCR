//
// Created by rigole on 04/12/22.
//

#include "osapi.h"

#include <time.h>

unsigned long long
get_time()
{
    struct timespec ts;
    unsigned long long ticks = 0;
    clock_gettime(CLOCK_REALTIME, &ts);
    ticks = ts.tv_nsec / 1000000;
    ticks += ts.tv_sec * 1000;
    return ticks;
}