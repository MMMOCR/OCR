#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <err.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define smooth_kernel_size 5
#define sigma 0.84089642
#define K 1

void
gaussian_kernel(double gauss[5][5]);

void
compute(SDL_Surface* surface, double gauss[5][5], int k, SDL_Surface* out);
