//
// Created by rigole on 04/12/22.
//

#include "linear.h"
#include "../osapi.h"

void
linear_run(linear_anim *anim)
{
    if (anim->state) {
        if (anim->time < get_time() + 100L) {
            anim->current = (anim->current * (1. - anim->speed) + anim->target * anim->speed);
            anim->time = get_time();
        }
    }
}

char
linear_has_finished(linear_anim *anim)
{
    return anim->current < anim->target ? anim->current > anim->target - 1 : anim->current < anim->target + 1;
}