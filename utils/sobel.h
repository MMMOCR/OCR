#include <SDL2/SDL_stdinc.h>
#include <math.h>
#include <SDL2/SDL_stdinc.h>
#include <stdlib.h>
#include <SDL2/SDL_image.h>

void
convolve(Uint32* pixels, int* Gx_compute, int* Gy_compute, int i, int j, int w);

void
edges(SDL_Surface* surface);
