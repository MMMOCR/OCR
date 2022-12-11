//
// Created by rigole on 01/11/22.
//

#ifndef SYPBC_INSTRUCTIONS_H
#define SYPBC_INSTRUCTIONS_H

#define MAX_INSTRUCTIONS 4096

#include "utils/colors.h"
#include "utils/text.h"

#include <stddef.h>

enum sypbc_instruction
{
    DRAW_RECT,
    DRAW_FILLED_RECT,
    DRAW_LINE,
    DRAW_CIRCLE,
    DRAW_FILLED_CIRCLE,
    DRAW_SCISSORS,
    DRAW_TEXT,
    DRAW_IMAGE
};

struct
{
    enum sypbc_instruction type;
    void *inst;
} typedef instruction;

struct sypbc_draw_rect
{
    int x, y, width, height;
    rgba color;
    int round;
};

struct sypbc_draw_filled_rect
{
    int x, y, width, height;
    rgba color;
    int round;
};

struct sypbc_draw_line
{
    int x1, y1, x2, y2, thickness;
    rgba color;
};

struct sypbc_draw_circle
{
    int x, y, radius;
    rgba color;
};

struct sypbc_draw_filled_circle
{
    int x, y, radius;
    rgba color;
};

struct sypbc_text
{
    int x, y;
    char text[255];
    int font_id;
    //    unsigned char *font_data;
    //    int size;
    rgba color;
};

struct sypbc_draw_image
{
    int x, y;
    char raw;
    char path[255];
};

struct sypbc_scissors
{
    int x, y, width, height;
};

struct sypbc_draw
{
    instruction
      draw_list[MAX_INSTRUCTIONS]; // maximum of MAX_INSTRUCTIONS commands
    size_t instruction_count;
};

void
draw_push_rect(struct sypbc_draw *draw_context,
               int x,
               int y,
               int width,
               int height,
               rgba color);
void
draw_push_filled_rect(struct sypbc_draw *draw_context,
                      int x,
                      int y,
                      int width,
                      int height,
                      rgba color);
void
draw_push_rounded_rect(struct sypbc_draw *draw_context,
                       int x,
                       int y,
                       int width,
                       int height,
                       int round,
                       rgba color);
void
draw_push_filled_rounded_rect(struct sypbc_draw *draw_context,
                              int x,
                              int y,
                              int width,
                              int height,
                              int round,
                              rgba color);
void
draw_push_line(struct sypbc_draw *draw_context,
               int x1,
               int y1,
               int x2,
               int y2,
               int thickness,
               rgba color);
void
draw_push_circle(struct sypbc_draw *draw_context,
                 int x,
                 int y,
                 int radius,
                 rgba color);
void
draw_push_filled_circle(struct sypbc_draw *draw_context,
                        int x,
                        int y,
                        int radius,
                        rgba color);
void
draw_push_scissors(struct sypbc_draw *draw_context,
                   int x,
                   int y,
                   int width,
                   int height);
void
draw_push_text(struct sypbc_draw *draw_context,
               char *text,
               int x,
               int y,
               int font_id,
               rgba color);
void
draw_push_image(struct sypbc_draw *draw_context, char *path, int x, int y);
void
draw_push_raw_image(struct sypbc_draw *draw_context, char *path, int x, int y);

void
free_draw(struct sypbc_draw *draw_context);

#endif // SYPBC_INSTRUCTIONS_H
