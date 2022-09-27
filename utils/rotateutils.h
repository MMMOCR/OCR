#pragma once
#include <SDL2/SDL.h>

SDL_Surface *
load_image(char file[]);

static inline Uint8 *
pixel_ref(SDL_Surface *surf, unsigned x, unsigned y);

Uint32
get_pixel(SDL_Surface *surface, unsigned x, unsigned y);

void
put_pixel(SDL_Surface *surface, unsigned x, unsigned y, Uint32 pixel);

void
fill_surface(SDL_Surface *surface, Uint32 pixel);

SDL_Surface *
rotate_image(SDL_Surface *image, double angle);
