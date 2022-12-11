//
// Created by rigole on 10/10/22.
//

#ifndef SYPBC_INPUT_H
#define SYPBC_INPUT_H

#include <stddef.h>

typedef void (*keyboard_callback)(int, char);

struct
{
    int x;
    int y;
} typedef position;

struct sypbc_input
{
    char* keys;
    char* mbuttons;
    position mouse_pos;

    keyboard_callback keyboard_hook[0xFF];
    size_t keyboard_hook_count;
};

static struct sypbc_input input; // input is unique
static char sypbc_input_init = 0;

// private
void
set_key(struct sypbc_input* input_context, int key_code, char pressed);
void
set_mouse(struct sypbc_input* input_context, int mouse_button, char pressed);
void
set_mouse_pos(struct sypbc_input* input_context, int x, int y);

// public
struct sypbc_input*
init_sypbc_input(size_t keys_size, size_t mouse_size);

char
register_keyboard_callback(struct sypbc_input* input_context,
                           keyboard_callback callback);
char
is_key_pressed(struct sypbc_input* input_context, int key_code);
char
is_mouse_pressed(struct sypbc_input* input_context, int mouse_button);
position
get_cursor_pos(struct sypbc_input* input_context);
char
is_cursor_within(struct sypbc_input* input_context,
                 int x1,
                 int y1,
                 int width,
                 int height);

#endif // SYPBC_INPUT_H
