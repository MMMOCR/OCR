#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <stddef.h>
#define ANGLE 360
#define PI 3.14159265358979323846
#define ABS(_x) _x > 0 ? _x : -_x
#define ORIENT(deg, rotate) \
    (deg % 180 < 90 && deg % 180 > -90) ? moy >= 0 ? 0 : 1 : moy >= 0 ? 1 : 0
#define TRIGO(rad, deg) \
    (deg % 180 < 46 || (deg + 46) % 180 < 46) ? cos(rad) : sin(rad)
#define TRIGOINV(rad, deg) (deg % 180 < 46) ? sin(rad) : cos(rad)
#define DIFF(deg, modulo) \
    (deg % modulo < 44) ? deg % modulo : (deg % modulo) - 90
#define DIAG(x, y) sqrt(x *x + y * y)
#define ISVERT(x) (x % 180 < 20 || ((x + 20) % 180) < 20) ? 1 : 0
#define ISHOR(x)                                        \
    (((x % 180) < 110 && (x % 180) > 70) ||             \
     (((x + 20) % 180) < 110 && ((x + 20) % 180) > 70)) \
      ? 1                                               \
      : 0
#define MAX(a, b) a > b ? a : b
#define MAXDIFF(x, y, n) (((x - y) >= 0 && (x - y) <= n) || ((y - x) >= 0 && (x - y) <= n)) ? 1 : 0

typedef struct
{
    long int *array;
    size_t len;
} Points_Array;

typedef struct
{
    long int *horizontal;
    long int *vertical;
    size_t count_h;
    size_t count_v;
} Sorted_Points_Array;

void
draw_line(int *pixels,
          long int w,
          long int h,
          long int x1,
          long int y1,
          long int x2,
          long int y2,
          Uint32 color);

Points_Array *
detect_lines(SDL_Surface *surface);
