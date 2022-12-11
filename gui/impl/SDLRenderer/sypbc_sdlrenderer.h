//
// Created by rigole on 10/10/22.
//

#ifndef SYPBC_SYPBC_SDLRENDERER_H
#define SYPBC_SYPBC_SDLRENDERER_H

#include "sypbc_impl.h"
#include "SDL/SDL_image.h"
#include <err.h>

struct {
    float position[2];
    float uv[2];
    unsigned char col[4];
} typedef vertex;


static struct sypbc*
sdl_init(SDL_Surface *screen, char *current_path)
{
    sdl.screen = screen;

    if (TTF_Init() == -1){
        errx(1, "TTF_Init error: %s\n", TTF_GetError());
    }

    strcpy(context.bin_path, current_path);
    strcat(context.bin_path, "/");

    strcpy(context.images_path, current_path);
    strcat(context.images_path, "/images/");

    strcpy(context.font_path, current_path);
    strcat(context.font_path, "/fonts/");

    char font[255];
    strcpy(font, context.font_path);
    strcat(font, "Roboto-Bold.ttf");
    context.fonts[ROBOTO_BOLD45] = TTF_OpenFont(font, 45);

    strcpy(font, context.font_path);
    strcat(font, "Roboto-Regular.ttf");
    context.fonts[ROBOTO_REGULAR32] = TTF_OpenFont(font, 32);
    context.fonts[ROBOTO_REGULAR16] = TTF_OpenFont(font, 16);


    context.input = init_sypbc_input(sizeof(SDLKey), 5); // hardcoded mouse size from SDL_mouse.h
    return &context;
}

char
sdl_event_callback(struct sypbc_input* input_context, SDL_Event *event)
{
    switch(event->type) {
        case SDL_KEYDOWN:
        case SDL_KEYUP: {
            char pressed = (char) (event->type == SDL_KEYDOWN);
            SDLKey key_code = event->key.keysym.sym;

//            set_key(input_context, key_code, pressed);
//
//            for (size_t i = 0; i < input_context->keyboard_hook_count; i++) {
//                input_context->keyboard_hook[i](key_code, pressed);
//            }
//            return 1;

            return -1; // TODO: support keys LOL
        }

        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP: {
            char pressed = (char) (event->type == SDL_MOUSEBUTTONDOWN);
            Uint8 button = event->button.button;

            set_mouse(input_context, button, pressed);
            return 1;
        }

        case SDL_MOUSEMOTION: {
            set_mouse_pos(input_context, event->motion.x, event->motion.y);
            return 1;
        }
    }

    return 0;
}

vec2
get_text_dim(char *text, int font_id)
{
    vec2 dims = { 0 };
    TTF_SizeText(context.fonts[font_id], text, &dims.x, &dims.y);
    return dims;
}

static void
render_surface(int x, int y, SDL_Surface* src)
{
    SDL_Rect offset = {x, y};
    SDL_BlitSurface(src, NULL, sdl.screen, &offset);
}

void
sdl_render(struct sypbc_draw* draw_context, rgba background)
{
    boxRGBA(sdl.screen, 0, 0, (short) sdl.screen->w, (short) sdl.screen->h, background.r, background.g, background.b, background.a);

    for (size_t i = 0; i < draw_context->instruction_count; i++) {
        instruction cmd = draw_context->draw_list[i];
        switch (cmd.type) {
            case DRAW_RECT: {
                struct sypbc_draw_rect *inst = cmd.inst;
                rect(sdl.screen, inst->x, inst->y, inst->width, inst->height, inst->round, inst->color);
                break;
            }
            case DRAW_FILLED_RECT: {
                struct sypbc_draw_filled_rect *inst = cmd.inst;
                fill_rect(sdl.screen, inst->x, inst->y, inst->width, inst->height, inst->round, inst->color);
                break;
            }
            case DRAW_LINE: {
                struct sypbc_draw_line *inst = cmd.inst;
                line(sdl.screen, inst->x1, inst->y1, inst->x2, inst->y2, inst->thickness, inst->color);
                break;
            }
            case DRAW_CIRCLE: {
                struct sypbc_draw_circle *inst = cmd.inst;
                circle(sdl.screen, inst->x, inst->y, inst->radius, inst->color);
                break;
            }
            case DRAW_FILLED_CIRCLE: {
                struct sypbc_draw_filled_circle *inst = cmd.inst;
                filled_circle(sdl.screen, inst->x, inst->y, inst->radius, inst->color);
                break;
            }
            case DRAW_SCISSORS: {
                struct sypbc_scissors *inst = cmd.inst;
                scissors(sdl.screen, &sdl.clip, inst->x, inst->y, inst->width, inst->height);
                break;
            }
            case DRAW_TEXT: {
                struct sypbc_text *inst = cmd.inst;
                SDL_Color c = {inst->color.g, inst->color.b,inst->color.b};
                SDL_Surface *txt = TTF_RenderText_Blended(context.fonts[inst->font_id], inst->text, c);
                render_surface(inst->x, inst->y, txt);
                SDL_FreeSurface(txt);
                break;
            }
            case DRAW_IMAGE: {
                struct sypbc_draw_image *inst = cmd.inst;
                SDL_Surface *img;
                if (!inst->raw) {
                    char img_path[255];
                    strcpy(img_path, context.images_path);
                    strcat(img_path, inst->path);

                    img = IMG_Load(img_path);
                } else img = IMG_Load(inst->path);

                render_surface(inst->x, inst->y, img);
                SDL_FreeSurface(img);
                break;
            }
            default:
                errx(1, "unsupported instruction %d\n", cmd.type);
        }
    }

    SDL_UpdateRect(sdl.screen, 0, 0, 0, 0);
    SDL_Flip( sdl.screen );
    free_draw(draw_context);
}

void
sdl_destroy()
{
    // TODO: free everything here LOL (we're closing anyway :clown:)
}

#endif //SYPBC_SYPBC_SDLRENDERER_H
