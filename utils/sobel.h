#include <SDL2/SDL_image.h>
#include <SDL2/SDL_stdinc.h>
#include <math.h>
#include <stdlib.h>

void
convolve(Uint32* pixels, int* Gx_compute, int* Gy_compute, int i, int j, int w);

void
edges(SDL_Surface* surface);
