#pragma once
#include <SDL.h>

Uint8 *
pixel_ref(SDL_Surface *surf, size_t x, size_t y);

Uint32
get_pixel(SDL_Surface *surface, size_t x, size_t y);

void
change_pixel(SDL_Surface *surface, size_t x, size_t y, Uint32 pixel);

void
fill_surface(SDL_Surface *surface, Uint32 pixel);

SDL_Surface *
rotate_image(SDL_Surface *image, double angle);
