#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <stddef.h>
#define ANGLE 360
#define PI 3.14159265358979323846
#define ABS(_x) _x > 0 ? (_x) : -(_x)
#define ORIENT(deg, rotate) \
    (deg % 180 < 90 && deg % 180 > -90) ? moy >= 0 ? 0 : 1 : moy >= 0 ? 1 : 0
#define TRIGO(rad, deg) \
    (deg % 180 < 46 || (deg + 46) % 180 < 46) ? cos(rad) : sin(rad)
#define TRIGOINV(rad, deg) (deg % 180 < 46) ? sin(rad) : cos(rad)
#define DIFF(deg, modulo) \
    (deg % modulo < 44) ? deg % modulo : (deg % modulo) - 90
#define ANGLEF(angle) (angle % 90 > 45) ? -(90 - (angle % 90)) : (angle % 90)
#define DIAG(x, y) sqrt(x *x + y * y)
#define ISVERT(x) (x % 180 < 10 || ((x + 10) % 180) < 10) ? 1 : 0
#define ISHOR(x)                                        \
    (((x % 180) < 100 && (x % 180) > 80) ||             \
     (((x + 10) % 180) < 100 && ((x + 10) % 180) > 80)) \
      ? 1                                               \
      : 0
#define MAX(a, b) a > b ? a : b
#define MIN(a, b) a > b ? b : a
#define MAXDIFF(x, y, n) \
    (((x - y) >= 0 && (x - y) <= n) || ((y - x) >= 0 && (y - x) <= n)) ? 1 : 0

typedef struct
{
    long double *data;
    size_t row;
    size_t column;
} Matrix;

typedef struct
{
    size_t x;
    size_t y;
} Point;

typedef struct
{
    Point *arr;
    size_t size;
} Point_arr;

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

long double
compute_determinant(Matrix *matrix);
