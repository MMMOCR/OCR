#pragma once
#include "imageutils.h"

void
to_black(SDL_Surface* surface,
         int threshold,
         long int x,
         long int y,
         long int w,
         long int h);

int
otsu_treshold(long int len, Uint32* pixels, int override_treshold);

void
multiple(long int w, long int h, SDL_Surface* surface);
