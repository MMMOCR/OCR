#pragma once
#include <SDL2/SDL.h>

SDL_Surface*
load_image(char file[]);
void
rotate_image(SDL_Surface* image, double angle);
