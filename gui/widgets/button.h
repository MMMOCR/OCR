//
// Created by rigole on 04/12/22.
//

#ifndef SYPBC_BUTTON_H
#define SYPBC_BUTTON_H

#define MAX_BUTTON_TEXT 255
#define MAX_PATH 255

#include "../sypbc.h"
#include "../utils/animations/linear.h"
#include "../utils/colors.h"

struct sypbc_button
{
    struct sypbc *ctx;
    int x, y, width, height, round;
    char enabled;
    rgba background;
    rgba text_color;
    linear_anim anim;
    int font;
    char path[MAX_PATH];
    char text[MAX_BUTTON_TEXT];
    char pressed;
};

struct sypbc_button
sypbc_button_init(struct sypbc *ctx,
                  char *text,
                  int font,
                  int x,
                  int y,
                  int w,
                  int h,
                  int round,
                  rgba col,
                  rgba text_color);
char
sypbc_button_draw(struct sypbc_button *button);
char
sypbc_image_button_draw(struct sypbc_button *button,
                        char *image,
                        int imagesize);
char
sypbc_button_is_within(struct sypbc_button *button);

#endif // SYPBC_BUTTON_H
