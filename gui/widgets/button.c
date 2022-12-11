//
// Created by rigole on 04/12/22.
//

#include "button.h"
#include "string.h"
#include "../impl/SDLRenderer/sypbc_impl.h"

#define ANIM_END 35

vec2
text_dim(struct sypbc *ctx, char *text, int font_id)
{
    vec2 dims = { 0 };
    TTF_SizeText(ctx->fonts[font_id], text, &dims.x, &dims.y);
    return dims;
}

struct sypbc_button
sypbc_button_init(struct sypbc *ctx, char* text, int font, int x, int y, int w, int h, int round, rgba col, rgba text_color)
{
    struct sypbc_button button;
    linear_anim anim = {ANIM_END, 0.2, 0, 1, 0};

    button.ctx = ctx;
    button.x = x;
    button.y = y;
    button.width = w;
    button.height = h;
    button.enabled = 1;
    button.font = font;
    button.round = round;
    button.background = col;
    button.text_color = text_color;
    memset(button.text, 0, 255);
    memcpy(button.text, text, strlen(text));
    button.anim = anim;
    button.pressed = 0;

    return button;
}

char
sypbc_button_draw(struct sypbc_button *button)
{
    return sypbc_image_button_draw(button, NULL, 0);
}

char
sypbc_image_button_draw(struct sypbc_button *button, char *image, int imagesize)
{
    char ret = 0;

    draw_push_filled_rounded_rect(&button->ctx->draw, button->x, button->y, button->width,
                                  button->height, button->round, button->background);
    if (*button->text) {
        vec2 dims = text_dim(button->ctx, button->text, button->font);
        if (image) draw_push_image(&button->ctx->draw, image, button->x + button->width / 2 - dims.x / 2 - imagesize / 2 - 5, button->y + button->height / 2 - imagesize / 2);
        draw_push_text(&button->ctx->draw, button->text, button->x + button->width / 2 - dims.x / 2 + imagesize / 2 + (image ? 5 : 0), button->y + button->height / 2 - dims.y / 2, button->font, button->text_color);
    } else if (!*button->text && image) {
        draw_push_image(&button->ctx->draw, image, button->x + button->width / 2 - imagesize / 2, button->y + button->height / 2 - imagesize / 2);
    }

    linear_run(&button->anim);
    draw_push_filled_rounded_rect(&button->ctx->draw, button->x, button->y, button->width,
                                  button->height, button->round, color_rgba(255, 255, 255, button->anim.current));


    if (button->enabled && is_cursor_within(button->ctx->input, button->x, button->y, button->width, button->height)) {
        button->anim.target = ANIM_END;

        if (!button->pressed && is_mouse_pressed(button->ctx->input, 1)) {
            button->pressed = 1;
        } else if (button->pressed && !is_mouse_pressed(button->ctx->input, 1)) {
            button->pressed = 0;
            ret = 1;
        }
    } else {
        button->anim.target = 0;
        button->pressed = 0;
    }

    return ret;
}

char
sypbc_button_is_within(struct sypbc_button *button)
{
    return is_cursor_within(button->ctx->input, button->x, button->y, button->width, button->height);
}