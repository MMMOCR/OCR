#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <err.h>
#include <stdint.h>
#include <stdio.h>

void
draw(SDL_Renderer* renderer, SDL_Texture* texture);

void
event_loop(SDL_Renderer* renderer, SDL_Texture* colored, SDL_Texture* grayscale);

SDL_Surface*
load_image(const char* path);

Uint32
pixel_to_grayscale(Uint32 pixel_color, SDL_PixelFormat* format);


void
surface_to_grayscale(SDL_Surface* surface);

void
back_to_black(SDL_Surface* surface, int threshold);

void
image_utils(char* filename);

