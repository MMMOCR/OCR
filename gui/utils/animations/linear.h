//
// Created by rigole on 04/12/22.
//

#ifndef SYPBC_LINEAR_H
#define SYPBC_LINEAR_H


struct {
    float target, speed, current;
    char state;
    unsigned long long time;
} typedef linear_anim;

void linear_run(linear_anim *anim);
char linear_has_finished(linear_anim *anim);

#endif //SYPBC_LINEAR_H
