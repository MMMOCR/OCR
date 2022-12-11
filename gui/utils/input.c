//
// Created by rigole on 10/10/22.
//

#include <stdlib.h>
#include <stdio.h>
#include "input.h"

// private

void
set_key(struct sypbc_input* input_context, int key_code, char pressed)
{
    if (!sypbc_input_init) exit(69);

    input_context->keys[key_code] = pressed;
}

void
set_mouse(struct sypbc_input* input_context, int mouse_button, char pressed)
{
    if (!sypbc_input_init) exit(69);

    input_context->mbuttons[mouse_button] = pressed;
}

void
set_mouse_pos(struct sypbc_input* input_context, int x, int y)
{
    if (!sypbc_input_init) exit(69);

    input_context->mouse_pos.x = x;
    input_context->mouse_pos.y = y;
}

// public

struct sypbc_input*
init_sypbc_input(size_t keys_size, size_t mbuttons_size)
{
    input.keys = calloc(keys_size, sizeof(char));
    input.mbuttons = calloc(mbuttons_size, sizeof(char));

    sypbc_input_init = 1;
    return &input;
}

char
register_keyboard_callback(struct sypbc_input* input_context, keyboard_callback callback)
{
    if (input_context->keyboard_hook_count >= 0xFF) {
        fprintf( stderr, "Cannot register more than 255 keyboard hooks\n");
        return 0;
    }

    input_context->keyboard_hook[input_context->keyboard_hook_count++] = callback;
}

char
is_key_pressed(struct sypbc_input* input_context, int key_code)
{
    return input_context->keys[key_code];
}

char
is_mouse_pressed(struct sypbc_input* input_context, int mouse_button)
{
    return input_context->mbuttons[mouse_button];
}

position
get_cursor_pos(struct sypbc_input* input_context)
{
    return input_context->mouse_pos;
}

char
is_cursor_within(struct sypbc_input* input_context, int x, int y, int width, int height)
{
    int mx = input_context->mouse_pos.x;
    int my = input_context->mouse_pos.y;

    return (char) (mx >= x && my >= y && mx <= x + width && my <= y + height);
}