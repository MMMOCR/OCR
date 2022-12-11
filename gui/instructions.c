//
// Created by rigole on 03/12/22.
//

#include "instructions.h"
#include <stdlib.h>
#include <string.h>

void
draw_push_rect(struct sypbc_draw *draw_context, int x, int y, int width, int height, rgba color)
{
    struct sypbc_draw_rect *rect = malloc(sizeof(struct sypbc_draw_rect));
    rect->x = x;
    rect->y = y;
    rect->width = width;
    rect->height = height;
    rect->round = 0;
    rect->color = color;

    instruction inst = { DRAW_RECT, (void*) rect };
    draw_context->draw_list[draw_context->instruction_count++] = inst;
}


void
draw_push_filled_rect(struct sypbc_draw *draw_context, int x, int y, int width, int height, rgba color)
{
    struct sypbc_draw_filled_rect *rect = malloc(sizeof(struct sypbc_draw_filled_rect));
    rect->x = x;
    rect->y = y;
    rect->width = width;
    rect->height = height;
    rect->round = 0;
    rect->color = color;

    instruction inst = { DRAW_FILLED_RECT, (void*) rect };
    draw_context->draw_list[draw_context->instruction_count++] = inst;
}

void
draw_push_rounded_rect(struct sypbc_draw *draw_context, int x, int y, int width, int height, int round, rgba color)
{
    struct sypbc_draw_rect *rect = malloc(sizeof(struct sypbc_draw_rect));
    rect->x = x;
    rect->y = y;
    rect->width = width;
    rect->height = height;
    rect->color = color;
    rect->round = round;

    instruction inst = { DRAW_RECT, (void*) rect };
    draw_context->draw_list[draw_context->instruction_count++] = inst;
}

void
draw_push_filled_rounded_rect(struct sypbc_draw *draw_context, int x, int y, int width, int height, int round, rgba color)
{
    struct sypbc_draw_filled_rect *rect = malloc(sizeof(struct sypbc_draw_filled_rect));
    rect->x = x;
    rect->y = y;
    rect->width = width;
    rect->height = height;
    rect->color = color;
    rect->round = round;

    instruction inst = { DRAW_FILLED_RECT, (void*) rect };
    draw_context->draw_list[draw_context->instruction_count++] = inst;
}

void
draw_push_line(struct sypbc_draw *draw_context, int x1, int y1, int x2, int y2, int thickness, rgba color)
{
    struct sypbc_draw_line *line = malloc(sizeof(struct sypbc_draw_line));
    line->x1 = x1;
    line->y1 = y1;
    line->x2 = x2;
    line->y2 = y2;
    line->thickness = thickness;
    line->color = color;

    instruction inst = { DRAW_LINE, (void*) line };
    draw_context->draw_list[draw_context->instruction_count++] = inst;
}

void
draw_push_circle(struct sypbc_draw *draw_context, int x, int y, int radius, rgba color)
{
    struct sypbc_draw_circle *circle = malloc(sizeof(struct sypbc_draw_circle));
    circle->x = x;
    circle->y = y;
    circle->color = color;
    circle->radius = radius;

    instruction inst = { DRAW_CIRCLE, (void*) circle };
    draw_context->draw_list[draw_context->instruction_count++] = inst;
}

void
draw_push_filled_circle(struct sypbc_draw *draw_context, int x, int y, int radius, rgba color)
{
    struct sypbc_draw_filled_circle *circle = malloc(sizeof(struct sypbc_draw_filled_circle));
    circle->x = x;
    circle->y = y;
    circle->color = color;
    circle->radius = radius;

    instruction inst = { DRAW_FILLED_CIRCLE, (void*) circle };
    draw_context->draw_list[draw_context->instruction_count++] = inst;
}

void
draw_push_scissors(struct sypbc_draw *draw_context, int x, int y, int width, int height)
{
    struct sypbc_scissors *scissors = malloc(sizeof(struct sypbc_scissors));
    scissors->x = x;
    scissors->y = y;
    scissors->width = width;
    scissors->height = height;

    instruction inst = { DRAW_SCISSORS, (void*) scissors };
    draw_context->draw_list[draw_context->instruction_count++] = inst;
}

void
draw_push_text(struct sypbc_draw *draw_context, char *text, int x, int y, int font_id, rgba color)
{
    struct sypbc_text *text_inst = malloc(sizeof(struct sypbc_text));
    text_inst->x = x;
    text_inst->y = y;
    memset(text_inst->text, 0, 255);
    memcpy(text_inst->text, text, strlen(text));
    text_inst->font_id = font_id;
//    text_inst->font_data = font->data;
//    text_inst->size = font->size;
    text_inst->color = color;

    instruction inst = { DRAW_TEXT, (void*) text_inst };
    draw_context->draw_list[draw_context->instruction_count++] = inst;
}

void
draw_push_raw_image(struct sypbc_draw *draw_context, char *path, int x, int y)
{
    struct sypbc_draw_image *image = malloc(sizeof(struct sypbc_draw_image));
    image->x = x;
    image->raw = 1;
    image->y = y;
    memset(image->path, 0, 255);
    memcpy(image->path, path, strlen(path));

    instruction inst = { DRAW_IMAGE, (void*) image };
    draw_context->draw_list[draw_context->instruction_count++] = inst;
}

void
draw_push_image(struct sypbc_draw *draw_context, char *path, int x, int y)
{
    struct sypbc_draw_image *image = malloc(sizeof(struct sypbc_draw_image));
    image->x = x;
    image->raw = 0;
    image->y = y;
    memset(image->path, 0, 255);
    memcpy(image->path, path, strlen(path));

    instruction inst = { DRAW_IMAGE, (void*) image };
    draw_context->draw_list[draw_context->instruction_count++] = inst;
}

void
free_draw(struct sypbc_draw *draw_context)
{
    for (size_t i = 0; i < draw_context->instruction_count; i++) free(draw_context->draw_list[i].inst);
    draw_context->instruction_count = 0;
}