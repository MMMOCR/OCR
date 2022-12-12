#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <err.h>
#include <stdint.h>
#include <stdio.h>

int
is_empty(SDL_Surface* image)
{
    int height = image->h;
    int width = image->w;
    int* pixels = (int*)image->pixels;
    long double sum = 0;
    size_t kafesum = 0;
    size_t ratiox = (width * 0.4);
    size_t ratioy = (height * 0.4);
    size_t i, j;
    long double count = 0;
    for (i = ratioy; i < height - ratioy; i++) {
        for (j = ratiox; j < width - ratiox; j++) {
            if (pixels[i * width + j] != -1) kafesum+=255;
            else sum += 255;
            count++;
        }
    }
    return (sum * 100 / (count * 255)) >= 100;
}
