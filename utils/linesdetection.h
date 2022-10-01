#pragma once

#include "SDL_pixels.h"
#include "SDL_stdinc.h"
#define ANGLE 360
#define ABS(_x) _x > 0 ? _x : -_x
#define PI 3.14159265358979323846

void
draw_line(int *pixels,
          long int w,
          long int h,
          long int x1,
          long int y1,
          long int x2,
          long int y2,
          Uint32 color);

long int *
detect_lines(int *pixels, long int w, long int h, SDL_PixelFormat *format);

