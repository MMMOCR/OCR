//
// Created by rigole on 10/10/22.
//

#ifndef SYPBC_SYPBC_H
#define SYPBC_SYPBC_H

#include "utils/input.h"
#include "instructions.h"

#define FONT_COUNT 3

enum Fonts {
    ROBOTO_BOLD45,
    ROBOTO_REGULAR32,
    ROBOTO_REGULAR16
};

struct sypbc {
    struct sypbc_input *input;
    struct sypbc_draw draw;
    void *fonts[FONT_COUNT];
    char bin_path[255];
    char images_path[255];
    char font_path[255];
};

struct {
    int x, y, w, z;
} typedef vec4;

struct {
    int x, y;
} typedef vec2;

static struct sypbc context;

#endif //SYPBC_SYPBC_H
