#include "linesdetection.h"

#include "SDL_rect.h"
#include "gaussian_blur.h"
#include "imageutils.h"
#include "rotateutils.h"
#include "sobel.h"

#include <SDL.h>
#include <SDL_image.h>
#include <math.h>
#include <stdlib.h>

void
draw_line(int *pixels,
          long int w,
          long int h,
          long int x1,
          long int y1,
          long int x2,
          long int y2,
          Uint32 color)
{
    int i, dx, dy, maxmove;
    int d, dinc1, dinc2;
    int x, xinc1, xinc2;
    int y, yinc1, yinc2;

    dx = x1 > x2 ? x1 - x2 : x2 - x1;
    dy = y1 > y2 ? y1 - y2 : y2 - y1;

    if (dx >= dy) {
        maxmove = dx + 1;
        d = (2 * dy) - dx;
        dinc1 = 2 * dy;
        dinc2 = (dy - dx) * 2;
        xinc1 = 1;
        xinc2 = 1;
        yinc1 = 0;
        yinc2 = 1;
    } else {
        maxmove = dy + 1;
        d = (2 * dx) - dy;
        dinc1 = 2 * dx;
        dinc2 = (dx - dy) * 2;
        xinc1 = 0;
        xinc2 = 1;
        yinc1 = 1;
        yinc2 = 1;
    }

    if (x1 > x2) {
        xinc1 = -xinc1;
        xinc2 = -xinc2;
    }

    if (y1 > y2) {
        yinc1 = -yinc1;
        yinc2 = -yinc2;
    }

    x = x1;
    y = y1;

    for (i = 0; i < maxmove; ++i) {
        if (x >= 0 && x < w && y >= 0 && y < h) pixels[y * w + x] = color;
        if (d < 0) {
            d += dinc1;
            x += xinc1;
            y += yinc1;
        } else {
            d += dinc2;
            x += xinc2;
            y += yinc2;
        }
    }
}

void
restrict_array2(Points_Array *arr, size_t h, size_t *mat)
{
    size_t diff, diffangle, save_i, save_j, save_k, save_l;
    for (size_t i = 0; i < arr->len; i += 2) {
        if (arr->array[i] == 0) { continue; }
        for (size_t k = i + 2; k < arr->len; k += 2) {
            if (arr->array[i] == 0) { continue; }
            if (arr->array[k] == 0) { continue; }

            save_i = arr->array[i];
            save_j = arr->array[i + 1];
            save_k = arr->array[k];
            save_l = arr->array[k + 1];

            diff = ABS((long int) (arr->array[i] - arr->array[k]));
            diffangle = ABS((long int) ((180 + arr->array[i + 1]) % 180 -
                                        (180 + arr->array[k + 1]) % 180));
            if (diffangle > 100) { diffangle = 180 - diffangle; }
            // size_t diffangle2 =
            // ABS((180 -
            // ABS((long int) (180 -
            // ((180 + arr->array[k + 1]) % 180 -
            // (180 + arr->array[i + 1]) % 180)))));

            if (diff < 0.03 * h && diffangle <= 4) {
                if (mat[save_i * ANGLE + save_j] >
                    mat[save_k * ANGLE + save_l]) {
                    arr->array[i] = arr->array[i];
                    arr->array[i + 1] = arr->array[i + 1];
                    arr->array[k] = 0;
                    arr->array[k + 1] = 0;
                } else {
                    arr->array[i] = arr->array[k];
                    arr->array[i + 1] = arr->array[k + 1];
                    arr->array[k] = 0;
                    arr->array[k + 1] = 0;
                }
            }
        }
    }
}

void
restrict_array(Points_Array *arr, size_t h, size_t *mat)
{
    size_t diff, diffangle, save_i, save_j, save_k, save_l;
    for (size_t i = 0; i < arr->len; i += 2) {
        if (arr->array[i] == 0) { continue; }
        for (size_t k = i + 2; k < arr->len; k += 2) {
            if (arr->array[k] == 0) { continue; }

            save_i = arr->array[i];
            save_j = arr->array[i + 1];
            save_k = arr->array[k];
            save_l = arr->array[k + 1];

            diff = ABS((long int) (arr->array[i] - arr->array[k]));
            diffangle = ABS((long int) ((180 + arr->array[i + 1]) % 180 -
                                        (180 + arr->array[k + 1]) % 180));

            if (diff < 0.05 * h && diffangle <= 4) {
                if (mat[save_i * ANGLE + save_j] >
                    mat[save_k * ANGLE + save_l]) {
                    arr->array[i] = arr->array[i];
                    arr->array[i + 1] = arr->array[i + 1];
                    arr->array[k] = 0;
                    arr->array[k + 1] = 0;
                } else {
                    arr->array[i] = arr->array[k];
                    arr->array[i + 1] = arr->array[k + 1];
                    arr->array[k] = 0;
                    arr->array[k + 1] = 0;
                }
            }
        }
    }
}

void
clean_array(Points_Array *arr)
{
    size_t l = 0;
    size_t k;
    for (size_t i = 0; i < arr->len; i += 2) {
        if (arr->array[i] == 0) {
            k = i + 2;
            while (k < arr->len && arr->array[k] == 0) {
                k += 2;
            }
            if (k == arr->len) { break; }
            arr->array[i] = arr->array[k];
            arr->array[i + 1] = arr->array[k + 1];
            arr->array[k] = 0;
            arr->array[k + 1] = 0;
            l += 2;
        } else {
            l += 2;
        }
    }
    arr->len = l;
    arr->array = realloc(arr->array, arr->len * sizeof(long int));
}

SDL_Surface *
detect_lines_and_rotate(int *pixels,
                        long int w,
                        long int h,
                        SDL_PixelFormat *format)
{
    long unsigned int diag;
    long unsigned int *mat;
    long int k;
    long unsigned int max_size;
    // long int x0, y0, x1, y1, x2, y2;
    size_t counter = 0;
    // float m, n;
    Uint8 r, g, b;
    SDL_Surface *surface;
    Points_Array *arr = calloc(1, sizeof(Points_Array));

    diag = sqrt(w * w + h * h);
    mat = calloc(diag * ANGLE, sizeof(long unsigned int));

    max_size = w > h ? w : h;

    if (mat == NULL) {
        printf("error\n");
        return NULL;
    }

    for (size_t j = 0; j < (size_t) h; j++) {
        for (size_t i = 0; i < (size_t) w; i++) {
            SDL_GetRGB(pixels[j * w + i], format, &r, &g, &b);
            if (r == 0 && g == 0 && b == 0) {
                for (size_t theta = 0; theta < ANGLE; theta++) {
                    k = (long unsigned int) ((double) j *
                                               cos((double) theta * PI / 180) +
                                             (double) i *
                                               sin((double) theta * PI / 180));
                    if (0 <= k && k < (long int) max_size) {
                        mat[k * ANGLE + theta]++;
                    }
                }
            }
        }
    }

    arr->len = 400;
    arr->array = calloc(400 * 2, sizeof(long int));

    // size_t max = 0;

    for (size_t i = 1; i < max_size; i++) {
        for (size_t j = 0; j < ANGLE; j++) {
            if (mat[i * ANGLE + j] > (unsigned long int) (w + h) / 7) {
                // m = sin(j * PI / 180);
                // n = cos(j * PI / 180);
                // x0 = m * i;
                // y0 = n * i;
                // x1 = x0 + 2 * w * (-n);
                // y1 = y0 + 2 * h * (m);
                // x2 = x0 - 2 * w * (-n);
                // y2 = y0 - 2 * h * (m);
                // draw_line(pixels, w, h, x1, y1, x2, y2,
                //     SDL_MapRGB(format, 255, 0, 0));
                if (arr->len <= counter) {
                    arr->len += 400;
                    arr->array =
                      realloc(arr->array, sizeof(long int) * arr->len);
                    if (!arr->array) { return NULL; }
                }
                arr->array[counter++] = i;
                arr->array[counter++] = j;
            }
        }
    }

    arr->array = realloc(arr->array, sizeof(long int) * counter);
    if (!arr->array) { return NULL; }
    arr->len = counter;

    restrict_array(arr, h, mat);
    clean_array(arr);

    // float m, n;
    // int x0, y0, x1, y1, x2, y2;

    // for (size_t i = 0; i < arr->len; i += 2) {
    //     printf("r: %lu,theta: %lu\n", arr->array[i],
    //            arr->array[i + 1]);
    //     m = sin(arr->array[i + 1] * PI / 180);
    //     n = cos(arr->array[i + 1] * PI / 180);
    //     x0 = m * arr->array[i];
    //     y0 = n * arr->array[i];
    //     x1 = x0 + 2 * w * (-n);
    //     y1 = y0 + 2 * h * (m);
    //     x2 = x0 - 2 * w * (-n);
    //     y2 = y0 - 2 * h * (m);
    //     draw_line(pixels, w, h, x1, y1, x2, y2,
    //               SDL_MapRGB(format, 0, 255, 0));
    // }
    //
    // SDL_Surface * tt_surface = SDL_CreateRGBSurfaceFrom(
    //   (void *) pixels, w, h, 32, format->BytesPerPixel * w, format->Rmask,
    //   format->Gmask, format->Bmask, format->Amask);
    // IMG_SavePNG(tt_surface, "./test8.png");

    double moy = 0;

    for (size_t i = 0; i < (size_t) arr->len; i += 2) {
        // printf("%i, %i\n", arr->array[i + 1], ANGLEF(arr->array[i + 1]));
        moy += ANGLEF(arr->array[i + 1]);
    }

    moy /= (double) arr->len / 2;

    // printf("%f\n", moy);

    surface = SDL_CreateRGBSurfaceFrom(
      (void *) pixels, w, h, 32, format->BytesPerPixel * w, format->Rmask,
      format->Gmask, format->Bmask, format->Amask);
    surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ARGB8888, 0);
    if (moy != 0) { surface = rotate_image(surface, -moy); }

    IMG_SavePNG(surface, "./test3.png");

    free(arr->array);
    free(arr);
    free(mat);

    return surface;
}

Points_Array *
detect_lines(SDL_Surface *surface)
{
    long int w = surface->w;
    long int h = surface->h;
    int *pixels = surface->pixels;
    SDL_PixelFormat *format = surface->format;
    long unsigned int diag;
    long unsigned int *mat;
    long int k;
    long unsigned int max_size;
    // long int x0, y0, x1, y1, x2, y2;
    size_t counter = 0;
    // float m, n;
    Uint8 r, g, b;
    SDL_Surface *end_surface;
    Points_Array *arr = calloc(1, sizeof(Points_Array));

    SDL_LockSurface(surface);
    diag = sqrt(w * w + h * h);
    mat = calloc(diag * ANGLE, sizeof(long unsigned int));

    max_size = w > h ? w : h;

    if (mat == NULL) {
        printf("error\n");
        return NULL;
    }

    for (size_t j = 0; j < (size_t) h; j++) {
        for (size_t i = 0; i < (size_t) w; i++) {
            SDL_GetRGB(pixels[j * w + i], format, &r, &g, &b);
            if (r == 0 && g == 0 && b == 0) {
                for (size_t theta = 0; theta < ANGLE; theta++) {
                    k = (long unsigned int) ((double) j *
                                               cos((double) theta * PI / 180) +
                                             (double) i *
                                               sin((double) theta * PI / 180));
                    if (0 <= k && k < (long int) max_size) {
                        mat[k * ANGLE + theta]++;
                    }
                }
            }
        }
    }

    arr->len = 800;
    arr->array = calloc(800 * 2, sizeof(long int));

    // size_t max = 0;

    for (size_t i = 1; i < max_size; i++) {
        for (size_t j = 0; j < ANGLE; j++) {
            if (mat[i * ANGLE + j] > (unsigned long int) (w + h) / 7) {
                // m = sin(j * PI / 180);
                // n = cos(j * PI / 180);
                // x0 = m * i;
                // y0 = n * i;
                // x1 = x0 + 2 * w * (-n);
                // y1 = y0 + 2 * h * (m);
                // x2 = x0 - 2 * w * (-n);
                // y2 = y0 - 2 * h * (m);
                // draw_line(pixels, w, h, x1, y1, x2, y2,
                // SDL_MapRGB(format, 0, 255, 0));
                if (arr->len <= counter) {
                    arr->len += 800;
                    arr->array =
                      realloc(arr->array, arr->len * sizeof(long int));
                    if (!arr->array) { return NULL; }
                }
                arr->array[counter++] = i;
                arr->array[counter++] = j;
            }
        }
    }

    //    for (size_t i = 0; i < counter; i += 2) {
    //        printf("r: %lu,theta: %lu\n", arr->array[i], arr->array[i + 1]);
    //    }

    //    printf("##########################################################\n");

    arr->array = realloc(arr->array, sizeof(long int) * counter);
    if (!arr->array) { return NULL; }
    arr->len = counter;

    restrict_array2(arr, h, mat);
    clean_array(arr);

    // float m, n;
    // int x0, y0, x1, y1, x2, y2;
    //
    for (size_t i = 0; i < arr->len; i += 2) {
        //        printf("r: %lu,theta: %lu\n", arr->array[i],
        //               arr->array[i + 1]);
        // m = sin(arr->array[i + 1] * PI / 180);
        // n = cos(arr->array[i + 1] * PI / 180);
        // x0 = m * arr->array[i];
        // y0 = n * arr->array[i];
        // x1 = x0 + 2 * w * (-n);
        // y1 = y0 + 2 * h * (m);
        // x2 = x0 - 2 * w * (-n);
        // y2 = y0 - 2 * h * (m);
        // draw_line(pixels, w, h, x1, y1, x2, y2,
        // SDL_MapRGB(format, 0, 255, 0));
    }

    SDL_Surface *tt_surface = SDL_CreateRGBSurfaceFrom(
      (void *) pixels, w, h, 32, format->BytesPerPixel * w, format->Rmask,
      format->Gmask, format->Bmask, format->Amask);
    IMG_SavePNG(tt_surface, "./test8.png");

    end_surface = SDL_CreateRGBSurfaceFrom(
      (void *) pixels, w, h, 32, format->BytesPerPixel * w, format->Rmask,
      format->Gmask, format->Bmask, format->Amask);

    IMG_SavePNG(end_surface, "./test5.png");
    SDL_FreeSurface(end_surface);
    SDL_UnlockSurface(surface);
    SDL_FreeSurface(tt_surface);

    free(mat);

    return arr;
}

Sorted_Points_Array *
sort_array(Points_Array *arr)
{
    char flag;
    size_t index;
    float m, n;
    long unsigned int x0, y0, x1, y1;
    Sorted_Points_Array *sorted_arr = calloc(1, sizeof(Sorted_Points_Array));
    sorted_arr->horizontal = calloc(arr->len, sizeof(long int));
    sorted_arr->vertical = calloc(arr->len, sizeof(long int));
    for (size_t i = 0; i < arr->len; i += 2) {
        if (ISVERT(arr->array[i + 1])) {
            flag = 0;
            for (size_t j = 0; j < sorted_arr->count_v; j += 2) {
                n = cos(arr->array[i + 1] * PI / 180);
                y0 = n * arr->array[i];
                n = cos(sorted_arr->vertical[j + 1] * PI / 180);
                y1 = n * sorted_arr->vertical[j];
                if (MAXDIFF((long int) y1, (long int) y0, 20)) {
                    flag = 1;
                    index = j;
                }
            }
            if (!flag) {
                sorted_arr->vertical[sorted_arr->count_v++] = arr->array[i];
                sorted_arr->vertical[sorted_arr->count_v++] = arr->array[i + 1];
            } else {
                sorted_arr->vertical[2 * index] =
                  (sorted_arr->vertical[2 * index] + arr->array[i]) / 2;
                sorted_arr->vertical[2 * index + 1] =
                  (sorted_arr->vertical[2 * index + 1] + arr->array[i + 1]) / 2;
            }
        } else if (ISHOR(arr->array[i + 1])) {
            flag = 0;
            for (size_t j = 0; j < sorted_arr->count_h; j += 2) {
                m = sin(arr->array[i + 1] * PI / 180);
                x0 = m * arr->array[i];
                m = sin(sorted_arr->horizontal[j + 1] * PI / 180);
                x1 = m * sorted_arr->horizontal[j];
                if (MAXDIFF((long int) x1, (long int) x0, 20)) {
                    flag = 1;
                    index = j;
                }
            }
            if (!flag) {
                sorted_arr->horizontal[sorted_arr->count_h++] = arr->array[i];
                sorted_arr->horizontal[sorted_arr->count_h++] =
                  arr->array[i + 1];
            } else {
                sorted_arr->horizontal[2 * index] =
                  (sorted_arr->horizontal[2 * index] + arr->array[i]) / 2;
                sorted_arr->horizontal[2 * index + 1] =
                  (sorted_arr->horizontal[2 * index + 1] + arr->array[i + 1]) /
                  2;
            }
        }
    }

    sorted_arr->vertical =
      realloc(sorted_arr->vertical, sizeof(long int) * sorted_arr->count_v);
    if (!sorted_arr->vertical) { return NULL; }
    sorted_arr->horizontal =
      realloc(sorted_arr->horizontal, sizeof(long int) * sorted_arr->count_h);
    if (!sorted_arr->horizontal) { return NULL; }

    return sorted_arr;
}

int
parametricIntersect(double r1,
                    double t1,
                    double r2,
                    double t2,
                    long int *x,
                    long int *y)
{
    double ct1 = cosf(t1 * PI / 180);
    double st1 = sinf(t1 * PI / 180);
    double ct2 = cosf(t2 * PI / 180);
    double st2 = sinf(t2 * PI / 180);
    double d = ct1 * st2 - st1 * ct2; // determinant de la matrice
    if (ABS(d) >= 0.1f) {
        *y = (long int) ((st2 * r1 - st1 * r2) / d);
        *x = (long int) ((-ct2 * r1 + ct1 * r2) / d);
        return (1);
    } else { // lignes paralelles
        return (0);
    }
}

Point_arr_o *
get_intersection_points(Points_Array *array,
                        long int w,
                        long int h,
                        SDL_Surface *surf)
{
    long int offset = 0;
    IMG_SavePNG(surf, "test11.png");
    Point_arr_o *pa = calloc(1, sizeof(Point_arr_o));
    pa->hg = calloc(100, sizeof(Point));
    pa->hd = calloc(100, sizeof(Point));
    pa->bg = calloc(100, sizeof(Point));
    pa->bd = calloc(100, sizeof(Point));
    pa->hge = calloc(200, sizeof(size_t));
    pa->hde = calloc(200, sizeof(size_t));
    pa->bge = calloc(200, sizeof(size_t));
    pa->bde = calloc(200, sizeof(size_t));
    Point po;
    int *pixels = surf->pixels;
    long int x, y;
    int tl1 = 0, tr1 = 0, bl1 = 0, br1 = 0, tl2 = 0, tr2 = 0, bl2 = 0, br2 = 0;
    SDL_PixelFormat *format = surf->format;
    Uint8 r, g, b;
    size_t m1 = 0, m2 = 0, m3 = 0, m4 = 0;
    while (pa->bgs == 0 || pa->bds == 0 || pa->hds == 0 || pa->hgs == 0) {
        pa->bgs = 0;
        pa->bds = 0;
        pa->hgs = 0;
        pa->hds = 0;
        for (size_t p = 0; p < array->len - 2; p += 2) {
            for (size_t q = p + 2; q < array->len; q += 2) {
                if (parametricIntersect(array->array[p], array->array[p + 1],
                                        array->array[q], array->array[q + 1],
                                        &x, &y)) {
                    if (x >= 0 && x < w && y >= 0 && y < h) {
                        tr1 = 0;
                        tl1 = 0;
                        br1 = 0;
                        bl1 = 0;
                        tr2 = 0;
                        tl2 = 0;
                        br2 = 0;
                        bl2 = 0;
                        m1 = x;
                        m2 = y;
                        m4 = 0;

                        SDL_GetRGB(pixels[y * w + x], format, &r, &g, &b);
                        // if (r == 255 && g == 255 && b == 255) {
                        for (long int i = x - 5; i < x + 6; i++) {
                            for (long int j = y - 5; j < y + 6; j++) {
                                if (i >= 0 && i < w && j >= 0 && j < h) {
                                    SDL_GetRGB(pixels[j * w + i], format, &r,
                                               &g, &b);
                                    if (r == 0 && b == 0 && g == 0) {
                                        m3 = 0;
                                        for (long int k = i - 1; k < i + 2;
                                             k++) {
                                            for (long int l = j - 1; l < j + 2;
                                                 l++) {
                                                if (k >= 0 && k < w && l >= 0 &&
                                                    l < h) {
                                                    SDL_GetRGB(
                                                      pixels[l * w + k], format,
                                                      &r, &g, &b);
                                                    if (r == 0 && b == 0 &&
                                                        g == 0) {
                                                        m3 += 1;
                                                    }
                                                }
                                            }
                                        }
                                        if (m3 > m4 &&
                                            (ABS(i - x)) <=
                                              (long int) (ABS(i - m1)) &&
                                            (ABS(j - y)) && (ABS(j - m2))) {
                                            m1 = i;
                                            m2 = j;
                                            m4 = m3;
                                        }
                                    }
                                }
                            }
                        }
                        // }

                        // printf("m1: %lu, m2: %lu, m3: %lu, m4: %lu\n", m1,
                        // m2,m3, m4);

                        x = m1;
                        y = m2;
                        for (long int xx = x - 20 - offset; xx < x - 10; xx++) {
                            for (long int yy = y - 20; yy < y - 10; yy++) {
                                if (xx >= 0 && xx < w && yy >= 0 && yy < h) {
                                    size_t t1 = ABS(
                                      (yy *
                                         cosf(array->array[p + 1] * PI / 180) +
                                       xx *
                                         sinf(array->array[p + 1] * PI / 180)) -
                                      array->array[p]);
                                    size_t t2 = ABS(
                                      (yy *
                                         cosf(array->array[q + 1] * PI / 180) +
                                       xx *
                                         sinf(array->array[q + 1] * PI / 180)) -
                                      array->array[q]);
                                    if (t1 <= 4) {
                                        SDL_GetRGB(pixels[yy * w + xx], format,
                                                   &r, &g, &b);
                                        if (r == 0 && b == 0 && g == 0) {
                                            tl1 += 1;
                                        }
                                    }
                                    if (t2 <= 4) {
                                        SDL_GetRGB(pixels[yy * w + xx], format,
                                                   &r, &g, &b);
                                        if (r == 0 && b == 0 && g == 0) {
                                            tl2 += 1;
                                        }
                                    }
                                }
                            }
                        }
                        for (long int xx = x - 10; xx < x; xx++) {
                            for (long int yy = y - 20 - offset; yy < y - 10;
                                 yy++) {
                                if (xx >= 0 && xx < w && yy >= 0 && yy < h) {
                                    size_t t1 = ABS(
                                      (yy *
                                         cosf(array->array[p + 1] * PI / 180) +
                                       xx *
                                         sinf(array->array[p + 1] * PI / 180)) -
                                      array->array[p]);
                                    size_t t2 = ABS(
                                      (yy *
                                         cosf(array->array[q + 1] * PI / 180) +
                                       xx *
                                         sinf(array->array[q + 1] * PI / 180)) -
                                      array->array[q]);

                                    if (t1 <= 4) {
                                        SDL_GetRGB(pixels[yy * w + xx], format,
                                                   &r, &g, &b);
                                        if (r == 0 && b == 0 && g == 0) {
                                            tl1 += 1;
                                        }
                                    }
                                    if (t2 <= 4) {
                                        SDL_GetRGB(pixels[yy * w + xx], format,
                                                   &r, &g, &b);
                                        if (r == 0 && b == 0 && g == 0) {
                                            tl2 += 1;
                                        }
                                    }
                                }
                            }
                        }
                        for (long int xx = x + 1; xx < x + 11; xx++) {
                            for (long int yy = y - 20 - offset; yy < y - 10;
                                 yy++) {
                                if (xx >= 0 && xx < w && yy >= 0 && yy < h) {
                                    size_t t1 = ABS(
                                      (yy *
                                         cosf(array->array[p + 1] * PI / 180) +
                                       xx *
                                         sinf(array->array[p + 1] * PI / 180)) -
                                      array->array[p]);
                                    size_t t2 = ABS(
                                      (yy *
                                         cosf(array->array[q + 1] * PI / 180) +
                                       xx *
                                         sinf(array->array[q + 1] * PI / 180)) -
                                      array->array[q]);
                                    if (t1 <= 4) {
                                        SDL_GetRGB(pixels[yy * w + xx], format,
                                                   &r, &g, &b);
                                        if (r == 0 && b == 0 && g == 0) {
                                            tr1 += 1;
                                        }
                                    }
                                    if (t2 <= 4) {
                                        SDL_GetRGB(pixels[yy * w + xx], format,
                                                   &r, &g, &b);
                                        if (r == 0 && b == 0 && g == 0) {
                                            tr2 += 1;
                                        }
                                    }
                                }
                            }
                        }
                        for (long int xx = x + 11; xx < x + 21 + offset; xx++) {
                            for (long int yy = y - 20 - offset; yy < y - 10;
                                 yy++) {
                                if (xx >= 0 && xx < w && yy >= 0 && yy < h) {
                                    size_t t1 = ABS(
                                      (yy *
                                         cosf(array->array[p + 1] * PI / 180) +
                                       xx *
                                         sinf(array->array[p + 1] * PI / 180)) -
                                      array->array[p]);
                                    size_t t2 = ABS(
                                      (yy *
                                         cosf(array->array[q + 1] * PI / 180) +
                                       xx *
                                         sinf(array->array[q + 1] * PI / 180)) -
                                      array->array[q]);
                                    if (t1 <= 4) {
                                        SDL_GetRGB(pixels[yy * w + xx], format,
                                                   &r, &g, &b);
                                        if (r == 0 && b == 0 && g == 0) {
                                            tr1 += 1;
                                        }
                                    }
                                    if (t2 <= 4) {
                                        SDL_GetRGB(pixels[yy * w + xx], format,
                                                   &r, &g, &b);
                                        if (r == 0 && b == 0 && g == 0) {
                                            tr2 += 1;
                                        }
                                    }
                                }
                            }
                        }
                        for (long int xx = x - 20 - offset; xx < x - 10; xx++) {
                            for (long int yy = y - 10; yy < y; yy++) {
                                if (xx >= 0 && xx < w && yy < h) {
                                    size_t t1 = ABS(
                                      (yy *
                                         cosf(array->array[p + 1] * PI / 180) +
                                       xx *
                                         sinf(array->array[p + 1] * PI / 180)) -
                                      array->array[p]);
                                    size_t t2 = ABS(
                                      (yy *
                                         cosf(array->array[q + 1] * PI / 180) +
                                       xx *
                                         sinf(array->array[q + 1] * PI / 180)) -
                                      array->array[q]);
                                    if (t1 <= 4) {
                                        SDL_GetRGB(pixels[yy * w + xx], format,
                                                   &r, &g, &b);
                                        if (r == 0 && b == 0 && g == 0) {
                                            tl1 += 1;
                                        }
                                    }
                                    if (t2 <= 4) {
                                        SDL_GetRGB(pixels[yy * w + xx], format,
                                                   &r, &g, &b);
                                        if (r == 0 && b == 0 && g == 0) {
                                            tl2 += 1;
                                        }
                                    }
                                }
                            }
                        }
                        for (long int xx = x + 11; xx < x + 21 + offset; xx++) {
                            for (long int yy = y - 10; yy < y; yy++) {
                                if (xx >= 0 && xx < w && yy >= 0 && yy < h) {
                                    size_t t1 = ABS(
                                      (yy *
                                         cosf(array->array[p + 1] * PI / 180) +
                                       xx *
                                         sinf(array->array[p + 1] * PI / 180)) -
                                      array->array[p]);
                                    size_t t2 = ABS(
                                      (yy *
                                         cosf(array->array[q + 1] * PI / 180) +
                                       xx *
                                         sinf(array->array[q + 1] * PI / 180)) -
                                      array->array[q]);
                                    if (t1 <= 4) {
                                        SDL_GetRGB(pixels[yy * w + xx], format,
                                                   &r, &g, &b);
                                        if (r == 0 && b == 0 && g == 0) {
                                            tr1 += 1;
                                        }
                                    }
                                    if (t2 <= 4) {
                                        SDL_GetRGB(pixels[yy * w + xx], format,
                                                   &r, &g, &b);
                                        if (r == 0 && b == 0 && g == 0) {
                                            tr2 += 1;
                                        }
                                    }
                                }
                            }
                        }
                        for (long int xx = x - 20 - offset; xx < x - 10; xx++) {
                            for (long int yy = y + 1; yy < y + 11; yy++) {
                                if (xx >= 0 && xx < w && yy >= 0 && yy < h) {
                                    size_t t1 = ABS(
                                      (yy *
                                         cosf(array->array[p + 1] * PI / 180) +
                                       xx *
                                         sinf(array->array[p + 1] * PI / 180)) -
                                      array->array[p]);
                                    size_t t2 = ABS(
                                      (yy *
                                         cosf(array->array[q + 1] * PI / 180) +
                                       xx *
                                         sinf(array->array[q + 1] * PI / 180)) -
                                      array->array[q]);
                                    if (t1 <= 4) {
                                        SDL_GetRGB(pixels[yy * w + xx], format,
                                                   &r, &g, &b);
                                        if (r == 0 && b == 0 && g == 0) {
                                            bl1 += 1;
                                        }
                                    }
                                    if (t2 <= 4) {
                                        SDL_GetRGB(pixels[yy * w + xx], format,
                                                   &r, &g, &b);
                                        if (r == 0 && b == 0 && g == 0) {
                                            bl2 += 1;
                                        }
                                    }
                                }
                            }
                        }
                        for (long int xx = x + 11; xx < x + 21 + offset; xx++) {
                            for (long int yy = y + 1; yy < y + 11; yy++) {
                                if (xx >= 0 && xx < w && yy >= 0 && yy < h) {
                                    size_t t1 = ABS(
                                      (yy *
                                         cosf(array->array[p + 1] * PI / 180) +
                                       xx *
                                         sinf(array->array[p + 1] * PI / 180)) -
                                      array->array[p]);
                                    size_t t2 = ABS(
                                      (yy *
                                         cosf(array->array[q + 1] * PI / 180) +
                                       xx *
                                         sinf(array->array[q + 1] * PI / 180)) -
                                      array->array[q]);
                                    if (t1 <= 4) {
                                        SDL_GetRGB(pixels[yy * w + xx], format,
                                                   &r, &g, &b);
                                        if (r == 0 && b == 0 && g == 0) {
                                            br1 += 1;
                                        }
                                    }
                                    if (t2 <= 4) {
                                        SDL_GetRGB(pixels[yy * w + xx], format,
                                                   &r, &g, &b);
                                        if (r == 0 && b == 0 && g == 0) {
                                            br2 += 1;
                                        }
                                    }
                                }
                            }
                        }
                        for (long int xx = x - 20 - offset; xx < x - 10; xx++) {
                            for (long int yy = y + 11; yy < y + 21 + offset;
                                 yy++) {
                                if (xx >= 0 && xx < w && yy >= 0 && yy < h) {
                                    size_t t1 = ABS(
                                      (yy *
                                         cosf(array->array[p + 1] * PI / 180) +
                                       xx *
                                         sinf(array->array[p + 1] * PI / 180)) -
                                      array->array[p]);
                                    size_t t2 = ABS(
                                      (yy *
                                         cosf(array->array[q + 1] * PI / 180) +
                                       xx *
                                         sinf(array->array[q + 1] * PI / 180)) -
                                      array->array[q]);
                                    if (t1 <= 4) {
                                        SDL_GetRGB(pixels[yy * w + xx], format,
                                                   &r, &g, &b);
                                        if (r == 0 && b == 0 && g == 0) {
                                            bl1 += 1;
                                        }
                                    }
                                    if (t2 <= 4) {
                                        SDL_GetRGB(pixels[yy * w + xx], format,
                                                   &r, &g, &b);
                                        if (r == 0 && b == 0 && g == 0) {
                                            bl2 += 1;
                                        }
                                    }
                                }
                            }
                        }
                        for (long int xx = x - 10; xx < x; xx++) {
                            for (long int yy = y + 11; yy < y + 21 + offset;
                                 yy++) {
                                if (xx >= 0 && xx < w && yy >= 0 && yy < h) {
                                    size_t t1 = ABS(
                                      (yy *
                                         cosf(array->array[p + 1] * PI / 180) +
                                       xx *
                                         sinf(array->array[p + 1] * PI / 180)) -
                                      array->array[p]);
                                    size_t t2 = ABS(
                                      (yy *
                                         cosf(array->array[q + 1] * PI / 180) +
                                       xx *
                                         sinf(array->array[q + 1] * PI / 180)) -
                                      array->array[q]);
                                    if (t1 <= 4) {
                                        SDL_GetRGB(pixels[yy * w + xx], format,
                                                   &r, &g, &b);
                                        if (r == 0 && b == 0 && g == 0) {
                                            bl1 += 1;
                                        }
                                    }
                                    if (t2 <= 4) {
                                        SDL_GetRGB(pixels[yy * w + xx], format,
                                                   &r, &g, &b);
                                        if (r == 0 && b == 0 && g == 0) {
                                            bl2 += 1;
                                        }
                                    }
                                }
                            }
                        }
                        for (long int xx = x + 1; xx < x + 11; xx++) {
                            for (long int yy = y + 11; yy < y + 21 + offset;
                                 yy++) {
                                if (xx >= 0 && xx < w && yy >= 0 && yy < h) {
                                    size_t t1 = ABS(
                                      (yy *
                                         cosf(array->array[p + 1] * PI / 180) +
                                       xx *
                                         sinf(array->array[p + 1] * PI / 180)) -
                                      array->array[p]);
                                    size_t t2 = ABS(
                                      (yy *
                                         cosf(array->array[q + 1] * PI / 180) +
                                       xx *
                                         sinf(array->array[q + 1] * PI / 180)) -
                                      array->array[q]);
                                    if (t1 <= 4) {
                                        SDL_GetRGB(pixels[yy * w + xx], format,
                                                   &r, &g, &b);
                                        if (r == 0 && b == 0 && g == 0) {
                                            br1 += 1;
                                        }
                                    }
                                    if (t2 <= 4) {
                                        SDL_GetRGB(pixels[yy * w + xx], format,
                                                   &r, &g, &b);
                                        if (r == 0 && b == 0 && g == 0) {
                                            br2 += 1;
                                        }
                                    }
                                }
                            }
                        }
                        for (long int xx = x + 11; xx < x + 21 + offset; xx++) {
                            for (long int yy = y + 11; yy < y + 21 + offset;
                                 yy++) {
                                if (xx >= 0 && xx < w && yy >= 0 && yy < h) {
                                    size_t t1 = ABS(
                                      (yy *
                                         cosf(array->array[p + 1] * PI / 180) +
                                       xx *
                                         sinf(array->array[p + 1] * PI / 180)) -
                                      array->array[p]);
                                    size_t t2 = ABS(
                                      (yy *
                                         cosf(array->array[q + 1] * PI / 180) +
                                       xx *
                                         sinf(array->array[q + 1] * PI / 180)) -
                                      array->array[q]);
                                    if (t1 <= 4) {
                                        SDL_GetRGB(pixels[yy * w + xx], format,
                                                   &r, &g, &b);
                                        if (r == 0 && b == 0 && g == 0) {
                                            br1 += 1;
                                        }
                                    }
                                    if (t2 <= 4) {
                                        SDL_GetRGB(pixels[yy * w + xx], format,
                                                   &r, &g, &b);
                                        if (r == 0 && b == 0 && g == 0) {
                                            br2 += 1;
                                        }
                                    }
                                }
                            }
                        }

                        // printf("oskour tl1: %lu, tr1: %lu, bl1: %lu, br1:
                        // %lu, tl2: %lu, tr2: %lu, bl2: %lu, br2: %lu, x: %lu,
                        // y: %lu\n", tl1, tr1, bl1, br1, tl2, tr2, bl2, br2, x,
                        // y);
                        if (!((tl1 > 5 && br1 > 5) || (bl1 > 5 && tr1 > 5) ||
                              (tl2 > 5 && br2 > 5) || (tr2 > 5 && bl2 > 5))) {
                            // printf("isok\n");

                            // close to the border
                            if ((ABS(x - w)) <= 15 && (ABS(y - h)) <= 15) {
                                po.x = x;
                                po.y = y;
                                pa->bde[pa->bds * 4] = array->array[p];
                                pa->bde[1 + pa->bds * 4] = array->array[p + 1];
                                pa->bde[2 + pa->bds * 4] = array->array[q];
                                pa->bde[3 + pa->bds * 4] = array->array[q + 1];
                                pa->bd[pa->bds++] = po;
                            } else if ((ABS(x - w)) <= 15 &&
                                       (ABS(y - 0)) <= 15) {
                                po.x = x;
                                po.y = y;
                                pa->hde[pa->hds * 4] = array->array[p];
                                pa->hde[1 + pa->hds * 4] = array->array[p + 1];
                                pa->hde[2 + pa->hds * 4] = array->array[q];
                                pa->hde[3 + pa->hds * 4] = array->array[q + 1];
                                pa->hd[pa->hds++] = po;
                            } else if ((ABS(x - 0)) <= 15 &&
                                       (ABS(y - 0)) <= 15) {
                                po.x = x;
                                po.y = y;
                                pa->hge[pa->hgs * 4] = array->array[p];
                                pa->hge[1 + pa->hgs * 4] = array->array[p + 1];
                                pa->hge[2 + pa->hgs * 4] = array->array[q];
                                pa->hge[3 + pa->hgs * 4] = array->array[q + 1];
                                pa->hg[pa->hgs++] = po;
                            } else if ((ABS(x - 0)) <= 15 &&
                                       (ABS(y - h)) <= 15) {
                                po.x = x;
                                po.y = y;
                                pa->bge[pa->bgs * 4] = array->array[p];
                                pa->bge[1 + pa->bgs * 4] = array->array[p + 1];
                                pa->bge[2 + pa->bgs * 4] = array->array[q];
                                pa->bge[3 + pa->bgs * 4] = array->array[q + 1];
                                pa->bg[pa->bgs++] = po;
                            }
                            // opposate side
                            else if (br1 > 5 && tl2 > 5 && bl1 < 5 && tl1 < 5 &&
                                     tr1 < 5 && bl2 < 5 && tr2 < 5 && bl2 < 5) {
                                // printf("(%lu, %lu)\n", x, y);
                                po.x = x;
                                po.y = y;
                                pa->hde[pa->hds * 4] = array->array[p];
                                pa->hde[1 + pa->hds * 4] = array->array[p + 1];
                                pa->hde[2 + pa->hds * 4] = array->array[q];
                                pa->hde[3 + pa->hds * 4] = array->array[q + 1];
                                pa->hd[pa->hds++] = po;
                            } else if (bl1 > 5 && tr2 > 5 && tl1 < 5 &&
                                       br1 < 5 && tr1 < 5 && bl2 < 5 &&
                                       br2 < 5 && tl2 < 5) {
                                // printf("(%lu, %lu)\n", x, y);
                                po.x = x;
                                po.y = y;
                                pa->bde[pa->bds * 4] = array->array[p];
                                pa->bde[1 + pa->bds * 4] = array->array[p + 1];
                                pa->bde[2 + pa->bds * 4] = array->array[q];
                                pa->bde[3 + pa->bds * 4] = array->array[q + 1];
                                pa->bd[pa->bds++] = po;
                            } else if (tr1 > 5 && bl2 > 5 && bl1 < 5 &&
                                       tr2 < 5) {
                                // printf("(%lu, %lu)\n", x, y);
                                po.x = x;
                                po.y = y;
                                if (!(ISVERT(array->array[p]))) {
                                    pa->hge[pa->hgs * 4] = array->array[p];
                                    pa->hge[1 + pa->hgs * 4] =
                                      array->array[p + 1];
                                    pa->hge[2 + pa->hgs * 4] = array->array[q];
                                    pa->hge[3 + pa->hgs * 4] =
                                      array->array[q + 1];
                                    pa->hg[pa->hgs++] = po;
                                } else {
                                    pa->bde[pa->bds * 4] = array->array[p];
                                    pa->bde[1 + pa->bds * 4] =
                                      array->array[p + 1];
                                    pa->bde[2 + pa->bds * 4] = array->array[q];
                                    pa->bde[3 + pa->bds * 4] =
                                      array->array[q + 1];
                                    pa->bd[pa->bds++] = po;
                                }
                            } else if (tl1 > 5 && br2 > 5 && bl1 < 5 &&
                                       br1 < 5 && tr1 < 5 && bl2 < 5 &&
                                       tr2 < 5 && tl2 < 5) {
                                // printf("(%lu, %lu)\n", x, y);
                                po.x = x;
                                po.y = y;
                                pa->hde[pa->hds * 4] = array->array[p];
                                pa->hde[1 + pa->hds * 4] = array->array[p + 1];
                                pa->hde[2 + pa->hds * 4] = array->array[q];
                                pa->hde[3 + pa->hds * 4] = array->array[q + 1];
                                pa->hd[pa->hds++] = po;
                            }
                            // corner classico
                            else if (tl1 > 5 && tr2 > 5 && br1 < 5 && tr1 < 5 &&
                                     br2 < 5 && bl2 < 5 && tl2 < 5) {
                                // printf("(%lu, %lu)\n", x, y);
                                po.x = x;
                                po.y = y;
                                pa->bde[pa->bds * 4] = array->array[p];
                                pa->bde[1 + pa->bds * 4] = array->array[p + 1];
                                pa->bde[2 + pa->bds * 4] = array->array[q];
                                pa->bde[3 + pa->bds * 4] = array->array[q + 1];
                                pa->bd[pa->bds++] = po;
                            } else if (tl1 > 5 && bl2 > 5 && bl1 < 5 &&
                                       br1 < 5 && tr1 < 5 && br2 < 5 &&
                                       tr2 < 5 && tl2 < 5) {
                                // printf("(%lu, %lu)\n", x, y);
                                po.x = x;
                                po.y = y;
                                pa->hde[pa->hds * 4] = array->array[p];
                                pa->hde[1 + pa->hds * 4] = array->array[p + 1];
                                pa->hde[2 + pa->hds * 4] = array->array[q];
                                pa->hde[3 + pa->hds * 4] = array->array[q + 1];
                                pa->hd[pa->hds++] = po;
                            } else if (tr1 > 5 && tr2 > 5 && br1 < 5 &&
                                       tl1 < 5 && bl1 < 5 && tl2 < 5 &&
                                       bl2 < 5 && br2 < 5) {
                                // printf("(%lu, %lu)\n", x, y);
                                po.x = x;
                                po.y = y;
                                pa->bde[pa->bds * 4] = array->array[p];
                                pa->bde[1 + pa->bds * 4] = array->array[p + 1];
                                pa->bde[2 + pa->bds * 4] = array->array[q];
                                pa->bde[3 + pa->bds * 4] = array->array[q + 1];
                                pa->bd[pa->bds++] = po;
                            } else if (br1 > 5 && br2 > 5 && bl1 < 5 &&
                                       tl1 < 5 && tr1 < 5 && tl2 < 5 &&
                                       tr2 < 5 && bl2 < 5) {
                                // printf("(%lu, %lu)\n", x, y);
                                po.x = x;
                                po.y = y;
                                pa->hge[pa->hgs * 4] = array->array[p];
                                pa->hge[1 + pa->hgs * 4] = array->array[p + 1];
                                pa->hge[2 + pa->hgs * 4] = array->array[q];
                                pa->hge[3 + pa->hgs * 4] = array->array[q + 1];
                                pa->hg[pa->hgs++] = po;
                            } else if (bl1 > 5 && bl2 > 5 && tl1 < 5 &&
                                       br1 < 5 && tr1 < 5 && br2 < 5 &&
                                       tr2 < 5 && tl2 < 5) {
                                // printf("(%lu, %lu)\n", x, y);
                                po.x = x;
                                po.y = y;
                                pa->hde[pa->hds * 4] = array->array[p];
                                pa->hde[1 + pa->hds * 4] = array->array[p + 1];
                                pa->hde[2 + pa->hds * 4] = array->array[q];
                                pa->hde[3 + pa->hds * 4] = array->array[q + 1];
                                pa->hd[pa->hds++] = po;
                            } else if (tl1 > 5 && tl2 > 5 && bl1 < 5 &&
                                       br1 < 5 && tr1 < 5 && br2 < 5 &&
                                       tr2 < 5 && bl2 < 5) {
                                // printf("(%lu, %lu)\n", x, y);
                                po.x = x;
                                po.y = y;
                                pa->bde[pa->bds * 4] = array->array[p];
                                pa->bde[1 + pa->bds * 4] = array->array[p + 1];
                                pa->bde[2 + pa->bds * 4] = array->array[q];
                                pa->bde[3 + pa->bds * 4] = array->array[q + 1];
                                pa->bd[pa->bds++] = po;
                            } else if (tr1 > 5 && br2 > 5 && bl1 < 5 &&
                                       br1 < 5 && tl1 < 5 && tl2 < 5 &&
                                       tr2 < 5 && bl2 < 5) {
                                // printf("(%lu, %lu)\n", x, y);
                                po.x = x;
                                po.y = y;
                                pa->bge[pa->bgs * 4] = array->array[p];
                                pa->bge[1 + pa->bgs * 4] = array->array[p + 1];
                                pa->bge[2 + pa->bgs * 4] = array->array[q];
                                pa->bge[3 + pa->bgs * 4] = array->array[q + 1];
                                pa->bg[pa->bgs++] = po;
                            } else if (tr1 > 5 && tl2 > 5 && bl1 < 5 &&
                                       br1 < 5 && tl1 < 5 && br2 < 5 &&
                                       tr2 < 5 && bl2 < 5) {
                                // printf("(%lu, %lu)\n", x, y);
                                po.x = x;
                                po.y = y;
                                pa->bde[pa->bds * 4] = array->array[p];
                                pa->bde[1 + pa->bds * 4] = array->array[p + 1];
                                pa->bde[2 + pa->bds * 4] = array->array[q];
                                pa->bde[3 + pa->bds * 4] = array->array[q + 1];
                                pa->bd[pa->bds++] = po;
                            } else if (br1 > 5 && bl2 > 5 && bl1 < 5 &&
                                       tl1 < 5 && tr1 < 5 && br2 < 5 &&
                                       tr2 < 5 && tl2 < 5) {
                                // printf("(%lu, %lu)\n", x, y);
                                po.x = x;
                                po.y = y;
                                pa->hge[pa->hgs * 4] = array->array[p];
                                pa->hge[1 + pa->hgs * 4] = array->array[p + 1];
                                pa->hge[2 + pa->hgs * 4] = array->array[q];
                                pa->hge[3 + pa->hgs * 4] = array->array[q + 1];
                                pa->hg[pa->hgs++] = po;
                            } else if (br1 > 5 && tr2 > 5 && bl1 < 5 &&
                                       tl1 < 5 && tr1 < 5 && br2 < 5 &&
                                       bl2 < 5 && tl2 < 5) {
                                // printf("(%lu, %lu)\n", x, y);
                                po.x = x;
                                po.y = y;
                                pa->bge[pa->bgs * 4] = array->array[p];
                                pa->bge[1 + pa->bgs * 4] = array->array[p + 1];
                                pa->bge[2 + pa->bgs * 4] = array->array[q];
                                pa->bge[3 + pa->bgs * 4] = array->array[q + 1];
                                pa->bg[pa->bgs++] = po;
                            } else if (bl1 > 5 && br2 > 5 && tl1 < 5 &&
                                       br1 < 5 && tr1 < 5 && bl2 < 5 &&
                                       tr2 < 5 && tl2 < 5) {
                                // printf("(%lu, %lu)\n", x, y);
                                po.x = x;
                                po.y = y;
                                if (!(ISVERT(array->array[p]))) {
                                    pa->hde[pa->hds * 4] = array->array[p];
                                    pa->hde[1 + pa->hds * 4] =
                                      array->array[p + 1];
                                    pa->hde[2 + pa->hds * 4] = array->array[q];
                                    pa->hde[3 + pa->hds * 4] =
                                      array->array[q + 1];
                                    pa->hd[pa->hds++] = po;
                                } else {
                                    pa->hge[pa->hgs * 4] = array->array[p];
                                    pa->hge[1 + pa->hgs * 4] =
                                      array->array[p + 1];
                                    pa->hge[2 + pa->hgs * 4] = array->array[q];
                                    pa->hge[3 + pa->hgs * 4] =
                                      array->array[q + 1];
                                    pa->hg[pa->hgs++] = po;
                                }
                            } else if (bl1 > 5 && tl2 > 5 && tl1 < 5 &&
                                       br1 < 5 && tr1 < 5 && br2 < 5 &&
                                       tr2 < 5 && bl2 < 5) {
                                // printf("(%lu, %lu)\n", x, y);
                                po.x = x;
                                po.y = y;
                                pa->hde[pa->hds * 4] = array->array[p];
                                pa->hde[1 + pa->hds * 4] = array->array[p + 1];
                                pa->hde[2 + pa->hds * 4] = array->array[q];
                                pa->hde[3 + pa->hds * 4] = array->array[q + 1];
                                pa->hd[pa->hds++] = po;
                            }
                        }
                    }
                }
            }
        }
        offset += 5;
        // printf("bd: %lu, bg: %lu, hg: %lu, hd: %lu\n", pa->bds, pa->bgs,
        // pa->hgs, pa->hds); for (size_t i = 0; i < pa->bgs; i++) { printf("bg
        // x: %lu, y: %lu\n", pa->bg[i].x, pa->bg[i].y);
        // }
        // for (size_t i = 0; i < pa->bds; i++) {
        // printf("bd x: %lu, y: %lu\n", pa->bd[i].x, pa->bd[i].y);
        // }
        // for (size_t i = 0; i < pa->hgs; i++) {
        // printf("hg x: %lu, y: %lu\n", pa->hg[i].x, pa->hg[i].y);
        // }
        // for (size_t i = 0; i < pa->hds; i++) {
        // printf("hd x: %lu, y: %lu\n", pa->hd[i].x, pa->hd[i].y);
        // }
        // errx(2, "suce");
    }

    // printf("bd: %lu, bg: %lu, hg: %lu, hd: %lu\n", pa->bds, pa->bgs, pa->hgs,
    // pa->hds);

    pa->bg = realloc(pa->bg, sizeof(Point) * pa->bgs);
    pa->hg = realloc(pa->hg, sizeof(Point) * pa->hgs);
    pa->hd = realloc(pa->hd, sizeof(Point) * pa->hds);
    pa->bd = realloc(pa->bd, sizeof(Point) * pa->bds);

    return pa;
}

SDL_Surface *
clean_suface(SDL_Surface *s, SDL_Surface *ss)
{
    int *pixels = s->pixels;
    int *pixelse = ss->pixels;
    SDL_Surface *out = ss;
    size_t nx = 0, ny = 0, nxx = 0, nyy = 0, nxxx = 0, nyyy = 0;
    Uint8 r = 0, g = 0, b = 0;
    int flag = 0;
    for (size_t i = 0; i < 20; i++) {
        nx = 0;
        for (long int j = 0; j < s->w; j++) {
            SDL_GetRGB(pixels[i * s->w + j], s->format, &r, &g, &b);
            if (r == 0 && g == 0 && b == 0) { nx++; }
        }
        if (nx >= 0.7 * s->w) {
            flag = 1;
        } else if (flag) {
            nyy = i;
            break;
        }
    }
    flag = 0;
    for (size_t j = 0; j < 20; j++) {
        ny = 0;
        for (long int i = 0; i < s->h; i++) {
            SDL_GetRGB(pixels[i * s->w + j], s->format, &r, &g, &b);
            if (r == 0 && g == 0 && b == 0) { ny++; }
        }
        if (ny >= 0.7 * s->h) {
            flag = 1;
        } else if (flag) {
            nxx = j;
            break;
        }
    }
    flag = 0;
    for (int i = s->h - 20; i < s->h; i++) {
        nx = 0;
        for (int j = 0; j < s->w; j++) {
            SDL_GetRGB(pixels[i * s->w + j], s->format, &r, &g, &b);
            if (r == 0 && g == 0 && b == 0) { nx++; }
        }
        if (nx >= 0.7 * s->w) {
            flag = 1;
        } else if (flag) {
            nyyy = s->w - i;
            break;
        }
    }
    flag = 0;
    for (int j = s->w - 20; j < s->w; j++) {
        ny = 0;
        for (int i = 0; i < s->h; i++) {
            SDL_GetRGB(pixels[i * s->w + j], s->format, &r, &g, &b);
            if (r == 0 && g == 0 && b == 0) { ny++; }
        }
        if (ny >= 0.7 * s->h) {
            flag = 1;
        } else if (flag) {
            nxxx = s->w - j;
            break;
        }
    }

    if (nxx != 0 || nyy != 0 || nxxx != 0 || nyyy != 0) {
        out = SDL_CreateRGBSurfaceWithFormat(
          0, s->w - nxx - nxxx, s->h - nyy - nyyy, 32, s->format->format);
        int *pi = out->pixels;
        for (size_t i = nyy; i < s->h - nyyy; i++) {
            for (size_t j = nxx; j < s->w - nxxx; j++) {
                pi[(i - nyy) * out->w + (j - nxx)] = pixelse[i * s->w + j];
            }
        }
        SDL_FreeSurface(ss);
    }
    SDL_FreeSurface(s);
    return out;
}

void
split_image(SDL_Surface *image, SDL_Surface *oute)
{
    SDL_Surface *surf, *surfe;
    size_t w = image->w, h = image->h, ww = w / 9, hh = h / 9;
    int *pixels = (int *) image->pixels;
    int *pixels_4 = (int *) oute->pixels;
    int *pixels_2, *pixels_3;
    char path[12] = { '.', '/', 'c', '_', 0, '_', 0, '.', 'p', 'n', 'g', 0 };
    for (size_t i = 0; i < 9; i++) {
        for (size_t j = 0; j < 9; j++) {
            surf = SDL_CreateRGBSurfaceWithFormat(
              0, (size_t) w / 9, (size_t) h / 9, 32, image->format->format);
            surfe = SDL_CreateRGBSurfaceWithFormat(
              0, (size_t) w / 9, (size_t) h / 9, 32, image->format->format);
            pixels_2 = (int *) surf->pixels;
            pixels_3 = (int *) surfe->pixels;
            for (size_t k = i * hh; k < (i + 1) * hh; k++) {
                for (size_t l = j * ww; l < (j + 1) * ww; l++) {
                    pixels_2[(k - i * hh) * ww + (l - j * ww)] =
                      pixels[k * image->w + l];
                    pixels_3[(k - i * hh) * ww + (l - j * ww)] =
                      pixels_4[k * image->w + l];
                }
            }
            surfe = clean_suface(surf, surfe);
            path[4] = i + 0x30;
            path[6] = j + 0x30;
            IMG_SavePNG(surf, path);
            SDL_FreeSurface(surfe);
        }
    }
}

Matrix *
init_mat(size_t row, size_t columns)
{
    Matrix *mat = calloc(1, sizeof(Matrix));
    mat->row = row;
    mat->column = columns;
    mat->data = calloc(row * row, sizeof(long double));
    return mat;
}

void
print_mat(Matrix *mat)
{
    printf("(%lu,%lu) = [", mat->row, mat->column);
    for (size_t i = 0; i < mat->row * mat->column; i++) {
        printf("%Lf,", mat->data[i]);
    }
    printf("]\n");
}

long double
compute_minor(Matrix *matrix, size_t i, size_t j)
{
    if (i >= matrix->row || j >= matrix->column) { return 0; }

    if (matrix->row == 1) { return matrix->data[0]; }

    if (matrix->row == 2) {
        if (i == 0) {
            if (j == 0) {
                return matrix->data[3];
            } else {
                return -matrix->data[2];
            }
        } else {
            if (j == 0) {
                return -matrix->data[1];
            } else {
                return matrix->data[0];
            }
        }
    }

    Matrix *minor = init_mat(matrix->row - 1, matrix->column - 1);
    for (size_t k = 0; k < matrix->row; k++) {
        if (i == k) { continue; }
        for (size_t l = 0; l < matrix->column; l++) {
            if (j == l) { continue; }
            minor->data[(k < i ? k : k - 1) * minor->column +
                        (l < j ? l : l - 1)] =
              matrix->data[k * matrix->column + l];
        }
    }

    long double k = compute_determinant(minor);
    free(minor->data);
    free(minor);
    return k;
}

long double
compute_determinant(Matrix *matrix)
{
    if (matrix->row != matrix->column) { return 0; }

    if (matrix->row == 1) { return matrix->data[0]; }

    if (matrix->row == 2) {
        return (long double) (matrix->data[0] * matrix->data[3] -
                              matrix->data[1] * matrix->data[2]);
    }

    long double det = 0;
    for (size_t i = 0; i < matrix->column; i++) {
        det +=
          matrix->data[i] * (i % 2 == 0 ? 1 : -1) * compute_minor(matrix, 0, i);
    }

    return det;
}

Matrix *
inverse_mat(Matrix *matrix)
{
    long double det = compute_determinant(matrix);

    if (det == 0) { return NULL; }

    Matrix *inverse = init_mat(matrix->row, matrix->row);
    long double s;

    for (size_t i = 0; i < matrix->row; i++) {
        for (size_t j = 0; j < matrix->column; j++) {
            s = (long double) (compute_minor(matrix, i, j)) *
              ((i + j) % 2 == 0 ? 1 : -1);
            s /= det;
            inverse->data[j * (matrix->row) + i] = s;
        }
    }

    return inverse;
}

Matrix *
mat_product(Matrix *a, Matrix *b)
{
    Matrix *r = init_mat(a->row, b->column);
    long double s = 0;
    for (size_t i = 0; i < a->row; i++) {
        for (size_t j = 0; j < b->column; j++) {
            s = 0;
            for (size_t k = 0; k < a->column; k++) {
                s +=
                  (a->data[i * a->column + k]) * (b->data[k * b->column + j]);
            }
            r->data[i * r->column + j] = s;
        }
    }
    return r;
}

Matrix *
create_transformation_matrix(Point *in_top_left,
                             Point *in_top_right,
                             Point *in_bot_left,
                             // Point *in_bot_right,
                             Point *out_top_left,
                             Point *out_top_right,
                             Point *out_bot_left
                             // Point *out_bot_right,
                             // size_t w,
                             // size_t h)
)
{
    Matrix *transform = init_mat(3, 3);

    transform->data[0] = out_top_left->x;
    transform->data[1] = out_top_left->y;
    transform->data[2] = 1;
    transform->data[3] = out_top_right->x;
    transform->data[4] = out_top_right->y;
    transform->data[5] = 1;
    transform->data[6] = out_bot_left->x;
    transform->data[7] = out_bot_left->y;
    transform->data[8] = 1;

    Matrix *inverse = inverse_mat(transform);

    free(transform->data);
    free(transform);

    Matrix *output = init_mat(3, 1);

    output->data[0] = in_top_left->x;
    output->data[1] = in_top_right->x;
    output->data[2] = in_bot_left->x;

    Matrix *output_x = mat_product(inverse, output);

    output->data[0] = in_top_left->y;
    output->data[1] = in_top_right->y;
    output->data[2] = in_bot_left->y;

    Matrix *output_y = mat_product(inverse, output);

    Matrix *perspective_transform = init_mat(3, 3);

    perspective_transform->data[0] = output_x->data[0];
    perspective_transform->data[1] = output_x->data[1];
    perspective_transform->data[2] = output_x->data[2];
    perspective_transform->data[3] = output_y->data[0];
    perspective_transform->data[4] = output_y->data[1];
    perspective_transform->data[5] = output_y->data[2];
    perspective_transform->data[6] = 0;
    perspective_transform->data[7] = 0;
    perspective_transform->data[8] = 1;

    free(output_x->data);
    free(output_y->data);
    free(output->data);
    free(inverse->data);

    free(output_x);
    free(output_y);
    free(output);
    free(inverse);

    return perspective_transform;
}

size_t
max_Point(size_t x1, size_t x2, size_t x3, size_t x4)
{
    x1 = MAX(x1, x2);
    x3 = MAX(x3, x4);
    x1 = MAX(x1, x3);
    return x1;
}

size_t
min_Point(size_t x1, size_t x2, size_t x3, size_t x4)
{
    x1 = MIN(x1, x2);
    x3 = MIN(x3, x4);
    x1 = MIN(x1, x3);
    return x1;
}

SDL_Surface *
flatten_image(SDL_Surface *image,
              Point *top_left,
              Point *top_right,
              Point *bot_left,
              Point *bot_right)
{
    size_t min_x =
      min_Point(top_left->x, top_right->x, bot_left->x, bot_right->x);
    size_t max_x =
      max_Point(top_left->x, top_right->x, bot_left->x, bot_right->x);
    size_t min_y =
      min_Point(top_left->y, top_right->y, bot_left->y, bot_right->y);
    size_t max_y =
      max_Point(top_left->y, top_right->y, bot_left->y, bot_right->y);

    size_t w = max_x - min_x + 1;
    size_t h = max_y - min_y + 1;

    Point i = { 0, 0 };
    Point j = { w, 0 };
    Point k = { 0, h };
    // Point l = { w, h };

    SDL_Surface *out = SDL_CreateRGBSurface(
      0, w, h, 32, image->format->Rmask, image->format->Gmask,
      image->format->Bmask, image->format->Amask);

    Matrix *transform =
      create_transformation_matrix(top_left, top_right, bot_left, &i, &j, &k);

    if (!transform) { errx(1, "mat transform not inv"); }

    int *pixels = image->pixels;
    int *new_pixels = out->pixels;

    long double new_x, new_y;
    for (size_t i = 0; i < w; i++) {
        for (size_t j = 0; j < h; j++) {
            new_x =
              round((long double) i * transform->data[0] +
                    (long double) j * transform->data[1] + transform->data[2]);
            new_y =
              round((long double) i * transform->data[3] +
                    (long double) j * transform->data[4] + transform->data[5]);
            if ((long int) new_x >= 0 && (long int) new_x < image->w &&
                (long int) new_y >= 0 && (long int) new_y < image->h) {
                new_pixels[(long int) j * w + (long int) i] =
                  pixels[(long int) new_y * image->w + (long int) new_x];
            }
        }
    }

    free(transform->data);
    free(transform);

    return out;
}

void
clean_corners_array(Point_arr_o *corners)
{
    Point *bg = NULL;
    Point *bd = NULL;
    Point *hg = NULL;
    Point *hd = NULL;
    size_t ibg, ibd, ihg, ihd;
    size_t t1 = 0, r1 = 0;
    size_t t2 = 0, r2 = 0;
    size_t t3 = 0, r3 = 0;
    size_t t4 = 0, r4 = 0;
    size_t t12 = 0, r12 = 0;
    size_t t22 = 0, r22 = 0;
    size_t t32 = 0, r32 = 0;
    size_t t42 = 0, r42 = 0;
    int flag = 0, new_flag = 0, save_flag = 0;
    if (corners->bgs == 1) {
        bg = corners->bg;
        r1 = corners->bge[0];
        t1 = corners->bge[1];
        r12 = corners->bge[2];
        t12 = corners->bge[3];
        ibg = 0;
    }
    if (corners->bds == 1) {
        bd = corners->bd;
        r2 = corners->bde[0];
        t2 = corners->bde[1];
        r22 = corners->bde[2];
        t22 = corners->bde[3];
        ibd = 0;
    }
    if (corners->hgs == 1) {
        hg = corners->hg;
        r3 = corners->hge[0];
        t3 = corners->hge[1];
        r32 = corners->hge[2];
        t32 = corners->hge[3];
        ihg = 0;
    }
    if (corners->hds == 1) {
        hd = corners->hd;
        r4 = corners->hde[0];
        t4 = corners->hde[1];
        r42 = corners->hde[2];
        t42 = corners->hde[3];
        ihd = 0;
    }
    if (!bg && !bd && !hg && !hd) {
        bg = corners->bg;
        r1 = corners->bge[0];
        t1 = corners->bge[1];
        r12 = corners->bge[2];
        t12 = corners->bge[3];
        ibg = 0;
    }
    while (!bg || !bd || !hg || !hd) {
        flag = (bg ? 1 : 0) << 1 | (bd ? 1 : 0) << 2 | (hg ? 1 : 0) << 3 |
          (hd ? 1 : 0) << 4;
        printf("fl:  %i, nf: %i, sf: %u\n", flag, new_flag, save_flag);
        if (bg && bd && hg) {
            for (size_t i = 0; i < corners->hds; i++) {
                if (r3 == corners->hde[i * 4] &&
                    t3 == corners->hde[i * 4 + 1] &&
                    r1 == corners->hde[i * 4 + 2] &&
                    t1 == corners->hde[i * 4 + 3] &&
                    (r2 != corners->hde[i * 4] ||
                     t2 != corners->hde[i * 4 + 1]) &&
                    (r22 != corners->hde[i * 4] ||
                     t22 != corners->hde[i * 4 + 1])) {
                    r4 = corners->hde[i * 4];
                    t4 = corners->hde[i * 4 + 1];
                    r42 = corners->hde[i * 4 + 2];
                    t42 = corners->hde[i * 4 + 3];
                    hd = &corners->hd[i];
                    ihd = i;
                    break;
                }
                if (r3 == corners->hde[i * 4 + 2] &&
                    t3 == corners->hde[i * 4 + 3] &&
                    r1 == corners->hde[i * 4] &&
                    t1 == corners->hde[i * 4 + 1] &&
                    (r2 != corners->hde[i * 4] ||
                     t2 != corners->hde[i * 4 + 1]) &&
                    (r22 != corners->hde[i * 4] ||
                     t22 != corners->hde[i * 4 + 1])) {
                    r4 = corners->hde[i * 4];
                    t4 = corners->hde[i * 4 + 1];
                    r42 = corners->hde[i * 4 + 2];
                    t42 = corners->hde[i * 4 + 3];
                    hd = &corners->hd[i];
                    ihd = i;
                    break;
                }
                if (r32 == corners->hde[i * 4] &&
                    t32 == corners->hde[i * 4 + 1] &&
                    r1 == corners->hde[i * 4 + 2] &&
                    t1 == corners->hde[i * 4 + 3] &&
                    (r2 != corners->hde[i * 4] ||
                     t2 != corners->hde[i * 4 + 1]) &&
                    (r22 != corners->hde[i * 4] ||
                     t22 != corners->hde[i * 4 + 1])) {
                    r4 = corners->hde[i * 4];
                    t4 = corners->hde[i * 4 + 1];
                    r42 = corners->hde[i * 4 + 2];
                    t42 = corners->hde[i * 4 + 3];
                    hd = &corners->hd[i];
                    ihd = i;
                    break;
                }
                if (r32 == corners->hde[i * 4 + 2] &&
                    t32 == corners->hde[i * 4 + 3] &&
                    r1 == corners->hde[i * 4] &&
                    t1 == corners->hde[i * 4 + 1] &&
                    (r2 != corners->hde[i * 4] ||
                     t2 != corners->hde[i * 4 + 1]) &&
                    (r22 != corners->hde[i * 4] ||
                     t22 != corners->hde[i * 4 + 1])) {
                    r4 = corners->hde[i * 4];
                    t4 = corners->hde[i * 4 + 1];
                    r42 = corners->hde[i * 4 + 2];
                    t42 = corners->hde[i * 4 + 3];
                    hd = &corners->hd[i];
                    ihd = i;
                    break;
                }
                if (r3 == corners->hde[i * 4] &&
                    t3 == corners->hde[i * 4 + 1] &&
                    r12 == corners->hde[i * 4 + 2] &&
                    t12 == corners->hde[i * 4 + 3] &&
                    (r2 != corners->hde[i * 4] ||
                     t2 != corners->hde[i * 4 + 1]) &&
                    (r22 != corners->hde[i * 4] ||
                     t22 != corners->hde[i * 4 + 1])) {
                    r4 = corners->hde[i * 4];
                    t4 = corners->hde[i * 4 + 1];
                    r42 = corners->hde[i * 4 + 2];
                    t42 = corners->hde[i * 4 + 3];
                    hd = &corners->hd[i];
                    ihd = i;
                    break;
                }
                if (r3 == corners->hde[i * 4 + 2] &&
                    t3 == corners->hde[i * 4 + 3] &&
                    r12 == corners->hde[i * 4] &&
                    t12 == corners->hde[i * 4 + 1] &&
                    (r2 != corners->hde[i * 4] ||
                     t2 != corners->hde[i * 4 + 1]) &&
                    (r22 != corners->hde[i * 4] ||
                     t22 != corners->hde[i * 4 + 1])) {
                    r4 = corners->hde[i * 4];
                    t4 = corners->hde[i * 4 + 1];
                    r42 = corners->hde[i * 4 + 2];
                    t42 = corners->hde[i * 4 + 3];
                    hd = &corners->hd[i];
                    ihd = i;
                    break;
                }
                if (r32 == corners->hde[i * 4] &&
                    t32 == corners->hde[i * 4 + 1] &&
                    r12 == corners->hde[i * 4 + 2] &&
                    t12 == corners->hde[i * 4 + 3] &&
                    (r2 != corners->hde[i * 4] ||
                     t2 != corners->hde[i * 4 + 1]) &&
                    (r22 != corners->hde[i * 4] ||
                     t22 != corners->hde[i * 4 + 1])) {
                    r4 = corners->hde[i * 4];
                    t4 = corners->hde[i * 4 + 1];
                    r42 = corners->hde[i * 4 + 2];
                    t42 = corners->hde[i * 4 + 3];
                    hd = &corners->hd[i];
                    ihd = i;
                    break;
                }
                if (r32 == corners->hde[i * 4 + 2] &&
                    t32 == corners->hde[i * 4 + 3] &&
                    r12 == corners->hde[i * 4] &&
                    t12 == corners->hde[i * 4 + 1] &&
                    (r2 != corners->hde[i * 4] ||
                     t2 != corners->hde[i * 4 + 1]) &&
                    (r22 != corners->hde[i * 4] ||
                     t22 != corners->hde[i * 4 + 1])) {
                    r4 = corners->hde[i * 4];
                    t4 = corners->hde[i * 4 + 1];
                    r42 = corners->hde[i * 4 + 2];
                    t42 = corners->hde[i * 4 + 3];
                    hd = &corners->hd[i];
                    ihd = i;
                    break;
                }
            }

            for (size_t i = 0; i < corners->bds; i++) {
                if (r3 == corners->bde[i * 4] &&
                    t3 == corners->bde[i * 4 + 1] &&
                    r1 == corners->bde[i * 4 + 2] &&
                    t1 == corners->bde[i * 4 + 3] &&
                    (r2 != corners->bde[i * 4] ||
                     t2 != corners->bde[i * 4 + 1]) &&
                    (r22 != corners->bde[i * 4] ||
                     t22 != corners->bde[i * 4 + 1])) {
                    r4 = corners->bde[i * 4];
                    t4 = corners->bde[i * 4 + 1];
                    r42 = corners->bde[i * 4 + 2];
                    t42 = corners->bde[i * 4 + 3];
                    hd = &corners->bd[i];
                    ihd = i;
                    break;
                }
                if (r3 == corners->bde[i * 4 + 2] &&
                    t3 == corners->bde[i * 4 + 3] &&
                    r1 == corners->bde[i * 4] &&
                    t1 == corners->bde[i * 4 + 1] &&
                    (r2 != corners->bde[i * 4] ||
                     t2 != corners->bde[i * 4 + 1]) &&
                    (r22 != corners->bde[i * 4] ||
                     t22 != corners->bde[i * 4 + 1])) {
                    r4 = corners->bde[i * 4];
                    t4 = corners->bde[i * 4 + 1];
                    r42 = corners->bde[i * 4 + 2];
                    t42 = corners->bde[i * 4 + 3];
                    hd = &corners->bd[i];
                    ihd = i;
                    break;
                }
                if (r32 == corners->bde[i * 4] &&
                    t32 == corners->bde[i * 4 + 1] &&
                    r1 == corners->bde[i * 4 + 2] &&
                    t1 == corners->bde[i * 4 + 3] &&
                    (r2 != corners->bde[i * 4] ||
                     t2 != corners->bde[i * 4 + 1]) &&
                    (r22 != corners->bde[i * 4] ||
                     t22 != corners->bde[i * 4 + 1])) {
                    r4 = corners->bde[i * 4];
                    t4 = corners->bde[i * 4 + 1];
                    r42 = corners->bde[i * 4 + 2];
                    t42 = corners->bde[i * 4 + 3];
                    hd = &corners->bd[i];
                    ihd = i;
                    break;
                }
                if (r32 == corners->bde[i * 4 + 2] &&
                    t32 == corners->bde[i * 4 + 3] &&
                    r1 == corners->bde[i * 4] &&
                    t1 == corners->bde[i * 4 + 1] &&
                    (r2 != corners->bde[i * 4] ||
                     t2 != corners->bde[i * 4 + 1]) &&
                    (r22 != corners->bde[i * 4] ||
                     t22 != corners->bde[i * 4 + 1])) {
                    r4 = corners->bde[i * 4];
                    t4 = corners->bde[i * 4 + 1];
                    r42 = corners->bde[i * 4 + 2];
                    t42 = corners->bde[i * 4 + 3];
                    hd = &corners->bd[i];
                    ihd = i;
                    break;
                }
                if (r3 == corners->bde[i * 4] &&
                    t3 == corners->bde[i * 4 + 1] &&
                    r12 == corners->bde[i * 4 + 2] &&
                    t12 == corners->bde[i * 4 + 3] &&
                    (r2 != corners->bde[i * 4] ||
                     t2 != corners->bde[i * 4 + 1]) &&
                    (r22 != corners->bde[i * 4] ||
                     t22 != corners->bde[i * 4 + 1])) {
                    r4 = corners->bde[i * 4];
                    t4 = corners->bde[i * 4 + 1];
                    r42 = corners->bde[i * 4 + 2];
                    t42 = corners->bde[i * 4 + 3];
                    hd = &corners->bd[i];
                    ihd = i;
                    break;
                }
                if (r3 == corners->bde[i * 4 + 2] &&
                    t3 == corners->bde[i * 4 + 3] &&
                    r12 == corners->bde[i * 4] &&
                    t12 == corners->bde[i * 4 + 1] &&
                    (r2 != corners->bde[i * 4] ||
                     t2 != corners->bde[i * 4 + 1]) &&
                    (r22 != corners->bde[i * 4] ||
                     t22 != corners->bde[i * 4 + 1])) {
                    r4 = corners->bde[i * 4];
                    t4 = corners->bde[i * 4 + 1];
                    r42 = corners->bde[i * 4 + 2];
                    t42 = corners->bde[i * 4 + 3];
                    hd = &corners->bd[i];
                    ihd = i;
                    break;
                }
                if (r32 == corners->bde[i * 4] &&
                    t32 == corners->bde[i * 4 + 1] &&
                    r12 == corners->bde[i * 4 + 2] &&
                    t12 == corners->bde[i * 4 + 3] &&
                    (r2 != corners->bde[i * 4] ||
                     t2 != corners->bde[i * 4 + 1]) &&
                    (r22 != corners->bde[i * 4] ||
                     t22 != corners->bde[i * 4 + 1])) {
                    r4 = corners->bde[i * 4];
                    t4 = corners->bde[i * 4 + 1];
                    r42 = corners->bde[i * 4 + 2];
                    t42 = corners->bde[i * 4 + 3];
                    hd = &corners->bd[i];
                    ihd = i;
                    break;
                }
                if (r32 == corners->bde[i * 4 + 2] &&
                    t32 == corners->bde[i * 4 + 3] &&
                    r12 == corners->bde[i * 4] &&
                    t12 == corners->bde[i * 4 + 1] &&
                    (r2 != corners->bde[i * 4] ||
                     t2 != corners->bde[i * 4 + 1]) &&
                    (r22 != corners->bde[i * 4] ||
                     t22 != corners->bde[i * 4 + 1])) {
                    r4 = corners->bde[i * 4];
                    t4 = corners->bde[i * 4 + 1];
                    r42 = corners->bde[i * 4 + 2];
                    t42 = corners->bde[i * 4 + 3];
                    hd = &corners->bd[i];
                    ihd = i;
                    break;
                }
            }

            for (size_t i = 0; i < corners->bgs; i++) {
                if (r3 == corners->bge[i * 4] &&
                    t3 == corners->bge[i * 4 + 1] &&
                    r1 == corners->bge[i * 4 + 2] &&
                    t1 == corners->bge[i * 4 + 3] &&
                    (r2 != corners->bge[i * 4] ||
                     t2 != corners->bge[i * 4 + 1]) &&
                    (r22 != corners->bge[i * 4] ||
                     t22 != corners->bge[i * 4 + 1])) {
                    r4 = corners->bge[i * 4];
                    t4 = corners->bge[i * 4 + 1];
                    r42 = corners->bge[i * 4 + 2];
                    t42 = corners->bge[i * 4 + 3];
                    hd = &corners->bg[i];
                    ihd = i;
                    break;
                }
                if (r3 == corners->bge[i * 4 + 2] &&
                    t3 == corners->bge[i * 4 + 3] &&
                    r1 == corners->bge[i * 4] &&
                    t1 == corners->bge[i * 4 + 1] &&
                    (r2 != corners->bge[i * 4] ||
                     t2 != corners->bge[i * 4 + 1]) &&
                    (r22 != corners->bge[i * 4] ||
                     t22 != corners->bge[i * 4 + 1])) {
                    r4 = corners->bge[i * 4];
                    t4 = corners->bge[i * 4 + 1];
                    r42 = corners->bge[i * 4 + 2];
                    t42 = corners->bge[i * 4 + 3];
                    hd = &corners->bg[i];
                    ihd = i;
                    break;
                }
                if (r32 == corners->bge[i * 4] &&
                    t32 == corners->bge[i * 4 + 1] &&
                    r1 == corners->bge[i * 4 + 2] &&
                    t1 == corners->bge[i * 4 + 3] &&
                    (r2 != corners->bge[i * 4] ||
                     t2 != corners->bge[i * 4 + 1]) &&
                    (r22 != corners->bge[i * 4] ||
                     t22 != corners->bge[i * 4 + 1])) {
                    r4 = corners->bge[i * 4];
                    t4 = corners->bge[i * 4 + 1];
                    r42 = corners->bge[i * 4 + 2];
                    t42 = corners->bge[i * 4 + 3];
                    hd = &corners->bg[i];
                    ihd = i;
                    break;
                }
                if (r32 == corners->bge[i * 4 + 2] &&
                    t32 == corners->bge[i * 4 + 3] &&
                    r1 == corners->bge[i * 4] &&
                    t1 == corners->bge[i * 4 + 1] &&
                    (r2 != corners->bge[i * 4] ||
                     t2 != corners->bge[i * 4 + 1]) &&
                    (r22 != corners->bge[i * 4] ||
                     t22 != corners->bge[i * 4 + 1])) {
                    r4 = corners->bge[i * 4];
                    t4 = corners->bge[i * 4 + 1];
                    r42 = corners->bge[i * 4 + 2];
                    t42 = corners->bge[i * 4 + 3];
                    hd = &corners->bg[i];
                    ihd = i;
                    break;
                }
                if (r3 == corners->bge[i * 4] &&
                    t3 == corners->bge[i * 4 + 1] &&
                    r12 == corners->bge[i * 4 + 2] &&
                    t12 == corners->bge[i * 4 + 3] &&
                    (r2 != corners->bge[i * 4] ||
                     t2 != corners->bge[i * 4 + 1]) &&
                    (r22 != corners->bge[i * 4] ||
                     t22 != corners->bge[i * 4 + 1])) {
                    r4 = corners->bge[i * 4];
                    t4 = corners->bge[i * 4 + 1];
                    r42 = corners->bge[i * 4 + 2];
                    t42 = corners->bge[i * 4 + 3];
                    hd = &corners->bg[i];
                    ihd = i;
                    break;
                }
                if (r3 == corners->bge[i * 4 + 2] &&
                    t3 == corners->bge[i * 4 + 3] &&
                    r12 == corners->bge[i * 4] &&
                    t12 == corners->bge[i * 4 + 1] &&
                    (r2 != corners->bge[i * 4] ||
                     t2 != corners->bge[i * 4 + 1]) &&
                    (r22 != corners->bge[i * 4] ||
                     t22 != corners->bge[i * 4 + 1])) {
                    r4 = corners->bge[i * 4];
                    t4 = corners->bge[i * 4 + 1];
                    r42 = corners->bge[i * 4 + 2];
                    t42 = corners->bge[i * 4 + 3];
                    hd = &corners->bg[i];
                    ihd = i;
                    break;
                }
                if (r32 == corners->bge[i * 4] &&
                    t32 == corners->bge[i * 4 + 1] &&
                    r12 == corners->bge[i * 4 + 2] &&
                    t12 == corners->bge[i * 4 + 3] &&
                    (r2 != corners->bge[i * 4] ||
                     t2 != corners->bge[i * 4 + 1]) &&
                    (r22 != corners->bge[i * 4] ||
                     t22 != corners->bge[i * 4 + 1])) {
                    r4 = corners->bge[i * 4];
                    t4 = corners->bge[i * 4 + 1];
                    r42 = corners->bge[i * 4 + 2];
                    t42 = corners->bge[i * 4 + 3];
                    hd = &corners->bg[i];
                    ihd = i;
                    break;
                }
                if (r32 == corners->bge[i * 4 + 2] &&
                    t32 == corners->bge[i * 4 + 3] &&
                    r12 == corners->bge[i * 4] &&
                    t12 == corners->bge[i * 4 + 1] &&
                    (r2 != corners->bge[i * 4] ||
                     t2 != corners->bge[i * 4 + 1]) &&
                    (r22 != corners->bge[i * 4] ||
                     t22 != corners->bge[i * 4 + 1])) {
                    r4 = corners->bge[i * 4];
                    t4 = corners->bge[i * 4 + 1];
                    r42 = corners->bge[i * 4 + 2];
                    t42 = corners->bge[i * 4 + 3];
                    hd = &corners->bg[i];
                    ihd = i;
                    break;
                }
            }

            for (size_t i = 0; i < corners->hgs; i++) {
                if (r3 == corners->hge[i * 4] &&
                    t3 == corners->hge[i * 4 + 1] &&
                    r1 == corners->hge[i * 4 + 2] &&
                    t1 == corners->hge[i * 4 + 3] &&
                    (r2 != corners->hge[i * 4] ||
                     t2 != corners->hge[i * 4 + 1]) &&
                    (r22 != corners->hge[i * 4] ||
                     t22 != corners->hge[i * 4 + 1])) {
                    r4 = corners->hge[i * 4];
                    t4 = corners->hge[i * 4 + 1];
                    r42 = corners->hge[i * 4 + 2];
                    t42 = corners->hge[i * 4 + 3];
                    hd = &corners->hg[i];
                    ihd = i;
                    break;
                }
                if (r3 == corners->hge[i * 4 + 2] &&
                    t3 == corners->hge[i * 4 + 3] &&
                    r1 == corners->hge[i * 4] &&
                    t1 == corners->hge[i * 4 + 1] &&
                    (r2 != corners->hge[i * 4] ||
                     t2 != corners->hge[i * 4 + 1]) &&
                    (r22 != corners->hge[i * 4] ||
                     t22 != corners->hge[i * 4 + 1])) {
                    r4 = corners->hge[i * 4];
                    t4 = corners->hge[i * 4 + 1];
                    r42 = corners->hge[i * 4 + 2];
                    t42 = corners->hge[i * 4 + 3];
                    hd = &corners->hg[i];
                    ihd = i;
                    break;
                }
                if (r32 == corners->hge[i * 4] &&
                    t32 == corners->hge[i * 4 + 1] &&
                    r1 == corners->hge[i * 4 + 2] &&
                    t1 == corners->hge[i * 4 + 3] &&
                    (r2 != corners->hge[i * 4] ||
                     t2 != corners->hge[i * 4 + 1]) &&
                    (r22 != corners->hge[i * 4] ||
                     t22 != corners->hge[i * 4 + 1])) {
                    r4 = corners->hge[i * 4];
                    t4 = corners->hge[i * 4 + 1];
                    r42 = corners->hge[i * 4 + 2];
                    t42 = corners->hge[i * 4 + 3];
                    hd = &corners->hg[i];
                    ihd = i;
                    break;
                }
                if (r32 == corners->hge[i * 4 + 2] &&
                    t32 == corners->hge[i * 4 + 3] &&
                    r1 == corners->hge[i * 4] &&
                    t1 == corners->hge[i * 4 + 1] &&
                    (r2 != corners->hge[i * 4] ||
                     t2 != corners->hge[i * 4 + 1]) &&
                    (r22 != corners->hge[i * 4] ||
                     t22 != corners->hge[i * 4 + 1])) {
                    r4 = corners->hge[i * 4];
                    t4 = corners->hge[i * 4 + 1];
                    r42 = corners->hge[i * 4 + 2];
                    t42 = corners->hge[i * 4 + 3];
                    hd = &corners->hg[i];
                    ihd = i;
                    break;
                }
                if (r3 == corners->hge[i * 4] &&
                    t3 == corners->hge[i * 4 + 1] &&
                    r12 == corners->hge[i * 4 + 2] &&
                    t12 == corners->hge[i * 4 + 3] &&
                    (r2 != corners->hge[i * 4] ||
                     t2 != corners->hge[i * 4 + 1]) &&
                    (r22 != corners->hge[i * 4] ||
                     t22 != corners->hge[i * 4 + 1])) {
                    r4 = corners->hge[i * 4];
                    t4 = corners->hge[i * 4 + 1];
                    r42 = corners->hge[i * 4 + 2];
                    t42 = corners->hge[i * 4 + 3];
                    hd = &corners->hg[i];
                    ihd = i;
                    break;
                }
                if (r3 == corners->hge[i * 4 + 2] &&
                    t3 == corners->hge[i * 4 + 3] &&
                    r12 == corners->hge[i * 4] &&
                    t12 == corners->hge[i * 4 + 1] &&
                    (r2 != corners->hge[i * 4] ||
                     t2 != corners->hge[i * 4 + 1]) &&
                    (r22 != corners->hge[i * 4] ||
                     t22 != corners->hge[i * 4 + 1])) {
                    r4 = corners->hge[i * 4];
                    t4 = corners->hge[i * 4 + 1];
                    r42 = corners->hge[i * 4 + 2];
                    t42 = corners->hge[i * 4 + 3];
                    hd = &corners->hg[i];
                    ihd = i;
                    break;
                }
                if (r32 == corners->hge[i * 4] &&
                    t32 == corners->hge[i * 4 + 1] &&
                    r12 == corners->hge[i * 4 + 2] &&
                    t12 == corners->hge[i * 4 + 3] &&
                    (r2 != corners->hge[i * 4] ||
                     t2 != corners->hge[i * 4 + 1]) &&
                    (r22 != corners->hge[i * 4] ||
                     t22 != corners->hge[i * 4 + 1])) {
                    r4 = corners->hge[i * 4];
                    t4 = corners->hge[i * 4 + 1];
                    r42 = corners->hge[i * 4 + 2];
                    t42 = corners->hge[i * 4 + 3];
                    hd = &corners->hg[i];
                    ihd = i;
                    break;
                }
                if (r32 == corners->hge[i * 4 + 2] &&
                    t32 == corners->hge[i * 4 + 3] &&
                    r12 == corners->hge[i * 4] &&
                    t12 == corners->hge[i * 4 + 1] &&
                    (r2 != corners->hge[i * 4] ||
                     t2 != corners->hge[i * 4 + 1]) &&
                    (r22 != corners->hge[i * 4] ||
                     t22 != corners->hge[i * 4 + 1])) {
                    r4 = corners->hge[i * 4];
                    t4 = corners->hge[i * 4 + 1];
                    r42 = corners->hge[i * 4 + 2];
                    t42 = corners->hge[i * 4 + 3];
                    hd = &corners->hg[i];
                    ihd = i;
                    break;
                }
            }
        }
        if (hd && bd && hg) {
            for (size_t i = 0; i < corners->hds; i++) {
                if (r3 == corners->hde[i * 4] &&
                    t3 == corners->hde[i * 4 + 1] &&
                    r2 == corners->hde[i * 4 + 2] &&
                    t2 == corners->hde[i * 4 + 3] &&
                    (r4 != corners->hde[i * 4] ||
                     t4 != corners->hde[i * 4 + 1]) &&
                    (r42 != corners->hde[i * 4] ||
                     t42 != corners->hde[i * 4 + 1])) {
                    r1 = corners->hde[i * 4];
                    t1 = corners->hde[i * 4 + 1];
                    r12 = corners->hde[i * 4 + 2];
                    t12 = corners->hde[i * 4 + 3];
                    bg = &corners->hd[i];
                    ibg = i;
                    break;
                }
                if (r3 == corners->hde[i * 4 + 2] &&
                    t3 == corners->hde[i * 4 + 3] &&
                    r2 == corners->hde[i * 4] &&
                    t2 == corners->hde[i * 4 + 1] &&
                    (r4 != corners->hde[i * 4] ||
                     t4 != corners->hde[i * 4 + 1]) &&
                    (r42 != corners->hde[i * 4] ||
                     t42 != corners->hde[i * 4 + 1])) {
                    r1 = corners->hde[i * 4];
                    t1 = corners->hde[i * 4 + 1];
                    r12 = corners->hde[i * 4 + 2];
                    t12 = corners->hde[i * 4 + 3];
                    bg = &corners->hd[i];
                    ibg = i;
                    break;
                }
                if (r32 == corners->hde[i * 4] &&
                    t32 == corners->hde[i * 4 + 1] &&
                    r2 == corners->hde[i * 4 + 2] &&
                    t2 == corners->hde[i * 4 + 3] &&
                    (r4 != corners->hde[i * 4] ||
                     t4 != corners->hde[i * 4 + 1]) &&
                    (r42 != corners->hde[i * 4] ||
                     t42 != corners->hde[i * 4 + 1])) {
                    r1 = corners->hde[i * 4];
                    t1 = corners->hde[i * 4 + 1];
                    r12 = corners->hde[i * 4 + 2];
                    t12 = corners->hde[i * 4 + 3];
                    bg = &corners->hd[i];
                    ibg = i;
                    break;
                }
                if (r32 == corners->hde[i * 4 + 2] &&
                    t32 == corners->hde[i * 4 + 3] &&
                    r2 == corners->hde[i * 4] &&
                    t2 == corners->hde[i * 4 + 1] &&
                    (r4 != corners->hde[i * 4] ||
                     t4 != corners->hde[i * 4 + 1]) &&
                    (r42 != corners->hde[i * 4] ||
                     t42 != corners->hde[i * 4 + 1])) {
                    r1 = corners->hde[i * 4];
                    t1 = corners->hde[i * 4 + 1];
                    r12 = corners->hde[i * 4 + 2];
                    t12 = corners->hde[i * 4 + 3];
                    bg = &corners->hd[i];
                    ibg = i;
                    break;
                }
                if (r3 == corners->hde[i * 4] &&
                    t3 == corners->hde[i * 4 + 1] &&
                    r22 == corners->hde[i * 4 + 2] &&
                    t22 == corners->hde[i * 4 + 3] &&
                    (r4 != corners->hde[i * 4] ||
                     t4 != corners->hde[i * 4 + 1]) &&
                    (r42 != corners->hde[i * 4] ||
                     t42 != corners->hde[i * 4 + 1])) {
                    r1 = corners->hde[i * 4];
                    t1 = corners->hde[i * 4 + 1];
                    r12 = corners->hde[i * 4 + 2];
                    t12 = corners->hde[i * 4 + 3];
                    bg = &corners->hd[i];
                    ibg = i;
                    break;
                }
                if (r3 == corners->hde[i * 4 + 2] &&
                    t3 == corners->hde[i * 4 + 3] &&
                    r22 == corners->hde[i * 4] &&
                    t22 == corners->hde[i * 4 + 1] &&
                    (r4 != corners->hde[i * 4] ||
                     t4 != corners->hde[i * 4 + 1]) &&
                    (r42 != corners->hde[i * 4] ||
                     t42 != corners->hde[i * 4 + 1])) {
                    r1 = corners->hde[i * 4];
                    t1 = corners->hde[i * 4 + 1];
                    r12 = corners->hde[i * 4 + 2];
                    t12 = corners->hde[i * 4 + 3];
                    bg = &corners->hd[i];
                    ibg = i;
                    break;
                }
                if (r32 == corners->hde[i * 4] &&
                    t32 == corners->hde[i * 4 + 1] &&
                    r22 == corners->hde[i * 4 + 2] &&
                    t22 == corners->hde[i * 4 + 3] &&
                    (r4 != corners->hde[i * 4] ||
                     t4 != corners->hde[i * 4 + 1]) &&
                    (r42 != corners->hde[i * 4] ||
                     t42 != corners->hde[i * 4 + 1])) {
                    r1 = corners->hde[i * 4];
                    t1 = corners->hde[i * 4 + 1];
                    r12 = corners->hde[i * 4 + 2];
                    t12 = corners->hde[i * 4 + 3];
                    bg = &corners->hd[i];
                    ibg = i;
                    break;
                }
                if (r32 == corners->hde[i * 4 + 2] &&
                    t32 == corners->hde[i * 4 + 3] &&
                    r22 == corners->hde[i * 4] &&
                    t22 == corners->hde[i * 4 + 1] &&
                    (r4 != corners->hde[i * 4] ||
                     t4 != corners->hde[i * 4 + 1]) &&
                    (r42 != corners->hde[i * 4] ||
                     t42 != corners->hde[i * 4 + 1])) {
                    r1 = corners->hde[i * 4];
                    t1 = corners->hde[i * 4 + 1];
                    r12 = corners->hde[i * 4 + 2];
                    t12 = corners->hde[i * 4 + 3];
                    bg = &corners->hd[i];
                    ibg = i;
                    break;
                }
            }

            for (size_t i = 0; i < corners->bds; i++) {
                if (r3 == corners->bde[i * 4] &&
                    t3 == corners->bde[i * 4 + 1] &&
                    r2 == corners->bde[i * 4 + 2] &&
                    t2 == corners->bde[i * 4 + 3] &&
                    (r4 != corners->bde[i * 4] ||
                     t4 != corners->bde[i * 4 + 1]) &&
                    (r42 != corners->bde[i * 4] ||
                     t42 != corners->bde[i * 4 + 1])) {
                    r1 = corners->bde[i * 4];
                    t1 = corners->bde[i * 4 + 1];
                    r12 = corners->bde[i * 4 + 2];
                    t12 = corners->bde[i * 4 + 3];
                    bg = &corners->bd[i];
                    ibg = i;
                    break;
                }
                if (r3 == corners->bde[i * 4 + 2] &&
                    t3 == corners->bde[i * 4 + 3] &&
                    r2 == corners->bde[i * 4] &&
                    t2 == corners->bde[i * 4 + 1] &&
                    (r4 != corners->bde[i * 4] ||
                     t4 != corners->bde[i * 4 + 1]) &&
                    (r42 != corners->bde[i * 4] ||
                     t42 != corners->bde[i * 4 + 1])) {
                    r1 = corners->bde[i * 4];
                    t1 = corners->bde[i * 4 + 1];
                    r12 = corners->bde[i * 4 + 2];
                    t12 = corners->bde[i * 4 + 3];
                    bg = &corners->bd[i];
                    ibg = i;
                    break;
                }
                if (r32 == corners->bde[i * 4] &&
                    t32 == corners->bde[i * 4 + 1] &&
                    r2 == corners->bde[i * 4 + 2] &&
                    t2 == corners->bde[i * 4 + 3] &&
                    (r4 != corners->bde[i * 4] ||
                     t4 != corners->bde[i * 4 + 1]) &&
                    (r42 != corners->bde[i * 4] ||
                     t42 != corners->bde[i * 4 + 1])) {
                    r1 = corners->bde[i * 4];
                    t1 = corners->bde[i * 4 + 1];
                    r12 = corners->bde[i * 4 + 2];
                    t12 = corners->bde[i * 4 + 3];
                    bg = &corners->bd[i];
                    ibg = i;
                    break;
                }
                if (r32 == corners->bde[i * 4 + 2] &&
                    t32 == corners->bde[i * 4 + 3] &&
                    r2 == corners->bde[i * 4] &&
                    t2 == corners->bde[i * 4 + 1] &&
                    (r4 != corners->bde[i * 4] ||
                     t4 != corners->bde[i * 4 + 1]) &&
                    (r42 != corners->bde[i * 4] ||
                     t42 != corners->bde[i * 4 + 1])) {
                    r1 = corners->bde[i * 4];
                    t1 = corners->bde[i * 4 + 1];
                    r12 = corners->bde[i * 4 + 2];
                    t12 = corners->bde[i * 4 + 3];
                    bg = &corners->bd[i];
                    ibg = i;
                    break;
                }
                if (r3 == corners->bde[i * 4] &&
                    t3 == corners->bde[i * 4 + 1] &&
                    r22 == corners->bde[i * 4 + 2] &&
                    t22 == corners->bde[i * 4 + 3] &&
                    (r4 != corners->bde[i * 4] ||
                     t4 != corners->bde[i * 4 + 1]) &&
                    (r42 != corners->bde[i * 4] ||
                     t42 != corners->bde[i * 4 + 1])) {
                    r1 = corners->bde[i * 4];
                    t1 = corners->bde[i * 4 + 1];
                    r12 = corners->bde[i * 4 + 2];
                    t12 = corners->bde[i * 4 + 3];
                    bg = &corners->bd[i];
                    ibg = i;
                    break;
                }
                if (r3 == corners->bde[i * 4 + 2] &&
                    t3 == corners->bde[i * 4 + 3] &&
                    r22 == corners->bde[i * 4] &&
                    t22 == corners->bde[i * 4 + 1] &&
                    (r4 != corners->bde[i * 4] ||
                     t4 != corners->bde[i * 4 + 1]) &&
                    (r42 != corners->bde[i * 4] ||
                     t42 != corners->bde[i * 4 + 1])) {
                    r1 = corners->bde[i * 4];
                    t1 = corners->bde[i * 4 + 1];
                    r12 = corners->bde[i * 4 + 2];
                    t12 = corners->bde[i * 4 + 3];
                    bg = &corners->bd[i];
                    ibg = i;
                    break;
                }
                if (r32 == corners->bde[i * 4] &&
                    t32 == corners->bde[i * 4 + 1] &&
                    r22 == corners->bde[i * 4 + 2] &&
                    t22 == corners->bde[i * 4 + 3] &&
                    (r4 != corners->bde[i * 4] ||
                     t4 != corners->bde[i * 4 + 1]) &&
                    (r42 != corners->bde[i * 4] ||
                     t42 != corners->bde[i * 4 + 1])) {
                    r1 = corners->bde[i * 4];
                    t1 = corners->bde[i * 4 + 1];
                    r12 = corners->bde[i * 4 + 2];
                    t12 = corners->bde[i * 4 + 3];
                    bg = &corners->bd[i];
                    ibg = i;
                    break;
                }
                if (r32 == corners->bde[i * 4 + 2] &&
                    t32 == corners->bde[i * 4 + 3] &&
                    r22 == corners->bde[i * 4] &&
                    t22 == corners->bde[i * 4 + 1] &&
                    (r4 != corners->bde[i * 4] ||
                     t4 != corners->bde[i * 4 + 1]) &&
                    (r42 != corners->bde[i * 4] ||
                     t42 != corners->bde[i * 4 + 1])) {
                    r1 = corners->bde[i * 4];
                    t1 = corners->bde[i * 4 + 1];
                    r12 = corners->bde[i * 4 + 2];
                    t12 = corners->bde[i * 4 + 3];
                    bg = &corners->bd[i];
                    ibg = i;
                    break;
                }
            }

            for (size_t i = 0; i < corners->bgs; i++) {
                if (r3 == corners->bge[i * 4] &&
                    t3 == corners->bge[i * 4 + 1] &&
                    r2 == corners->bge[i * 4 + 2] &&
                    t2 == corners->bge[i * 4 + 3] &&
                    (r4 != corners->bge[i * 4] ||
                     t4 != corners->bge[i * 4 + 1]) &&
                    (r42 != corners->bge[i * 4] ||
                     t42 != corners->bge[i * 4 + 1])) {
                    r1 = corners->bge[i * 4];
                    t1 = corners->bge[i * 4 + 1];
                    r12 = corners->bge[i * 4 + 2];
                    t12 = corners->bge[i * 4 + 3];
                    bg = &corners->bg[i];
                    ibg = i;
                    break;
                }
                if (r3 == corners->bge[i * 4 + 2] &&
                    t3 == corners->bge[i * 4 + 3] &&
                    r2 == corners->bge[i * 4] &&
                    t2 == corners->bge[i * 4 + 1] &&
                    (r4 != corners->bge[i * 4] ||
                     t4 != corners->bge[i * 4 + 1]) &&
                    (r42 != corners->bge[i * 4] ||
                     t42 != corners->bge[i * 4 + 1])) {
                    r1 = corners->bge[i * 4];
                    t1 = corners->bge[i * 4 + 1];
                    r12 = corners->bge[i * 4 + 2];
                    t12 = corners->bge[i * 4 + 3];
                    bg = &corners->bg[i];
                    ibg = i;
                    break;
                }
                if (r32 == corners->bge[i * 4] &&
                    t32 == corners->bge[i * 4 + 1] &&
                    r2 == corners->bge[i * 4 + 2] &&
                    t2 == corners->bge[i * 4 + 3] &&
                    (r4 != corners->bge[i * 4] ||
                     t4 != corners->bge[i * 4 + 1]) &&
                    (r42 != corners->bge[i * 4] ||
                     t42 != corners->bge[i * 4 + 1])) {
                    r1 = corners->bge[i * 4];
                    t1 = corners->bge[i * 4 + 1];
                    r12 = corners->bge[i * 4 + 2];
                    t12 = corners->bge[i * 4 + 3];
                    bg = &corners->bg[i];
                    ibg = i;
                    break;
                }
                if (r32 == corners->bge[i * 4 + 2] &&
                    t32 == corners->bge[i * 4 + 3] &&
                    r2 == corners->bge[i * 4] &&
                    t2 == corners->bge[i * 4 + 1] &&
                    (r4 != corners->bge[i * 4] ||
                     t4 != corners->bge[i * 4 + 1]) &&
                    (r42 != corners->bge[i * 4] ||
                     t42 != corners->bge[i * 4 + 1])) {
                    r1 = corners->bge[i * 4];
                    t1 = corners->bge[i * 4 + 1];
                    r12 = corners->bge[i * 4 + 2];
                    t12 = corners->bge[i * 4 + 3];
                    bg = &corners->bg[i];
                    ibg = i;
                    break;
                }
                if (r3 == corners->bge[i * 4] &&
                    t3 == corners->bge[i * 4 + 1] &&
                    r22 == corners->bge[i * 4 + 2] &&
                    t22 == corners->bge[i * 4 + 3] &&
                    (r4 != corners->bge[i * 4] ||
                     t4 != corners->bge[i * 4 + 1]) &&
                    (r42 != corners->bge[i * 4] ||
                     t42 != corners->bge[i * 4 + 1])) {
                    r1 = corners->bge[i * 4];
                    t1 = corners->bge[i * 4 + 1];
                    r12 = corners->bge[i * 4 + 2];
                    t12 = corners->bge[i * 4 + 3];
                    bg = &corners->bg[i];
                    ibg = i;
                    break;
                }
                if (r3 == corners->bge[i * 4 + 2] &&
                    t3 == corners->bge[i * 4 + 3] &&
                    r22 == corners->bge[i * 4] &&
                    t22 == corners->bge[i * 4 + 1] &&
                    (r4 != corners->bge[i * 4] ||
                     t4 != corners->bge[i * 4 + 1]) &&
                    (r42 != corners->bge[i * 4] ||
                     t42 != corners->bge[i * 4 + 1])) {
                    r1 = corners->bge[i * 4];
                    t1 = corners->bge[i * 4 + 1];
                    r12 = corners->bge[i * 4 + 2];
                    t12 = corners->bge[i * 4 + 3];
                    bg = &corners->bg[i];
                    ibg = i;
                    break;
                }
                if (r32 == corners->bge[i * 4] &&
                    t32 == corners->bge[i * 4 + 1] &&
                    r22 == corners->bge[i * 4 + 2] &&
                    t22 == corners->bge[i * 4 + 3] &&
                    (r4 != corners->bge[i * 4] ||
                     t4 != corners->bge[i * 4 + 1]) &&
                    (r42 != corners->bge[i * 4] ||
                     t42 != corners->bge[i * 4 + 1])) {
                    r1 = corners->bge[i * 4];
                    t1 = corners->bge[i * 4 + 1];
                    r12 = corners->bge[i * 4 + 2];
                    t12 = corners->bge[i * 4 + 3];
                    bg = &corners->bg[i];
                    ibg = i;
                    break;
                }
                if (r32 == corners->bge[i * 4 + 2] &&
                    t32 == corners->bge[i * 4 + 3] &&
                    r22 == corners->bge[i * 4] &&
                    t22 == corners->bge[i * 4 + 1] &&
                    (r4 != corners->bge[i * 4] ||
                     t4 != corners->bge[i * 4 + 1]) &&
                    (r42 != corners->bge[i * 4] ||
                     t42 != corners->bge[i * 4 + 1])) {
                    r1 = corners->bge[i * 4];
                    t1 = corners->bge[i * 4 + 1];
                    r12 = corners->bge[i * 4 + 2];
                    t12 = corners->bge[i * 4 + 3];
                    bg = &corners->bg[i];
                    ibg = i;
                    break;
                }
            }

            for (size_t i = 0; i < corners->hgs; i++) {
                if (r3 == corners->hge[i * 4] &&
                    t3 == corners->hge[i * 4 + 1] &&
                    r2 == corners->hge[i * 4 + 2] &&
                    t2 == corners->hge[i * 4 + 3] &&
                    (r4 != corners->hge[i * 4] ||
                     t4 != corners->hge[i * 4 + 1]) &&
                    (r42 != corners->hge[i * 4] ||
                     t42 != corners->hge[i * 4 + 1])) {
                    r1 = corners->hge[i * 4];
                    t1 = corners->hge[i * 4 + 1];
                    r12 = corners->hge[i * 4 + 2];
                    t12 = corners->hge[i * 4 + 3];
                    bg = &corners->hg[i];
                    ibg = i;
                    break;
                }
                if (r3 == corners->hge[i * 4 + 2] &&
                    t3 == corners->hge[i * 4 + 3] &&
                    r2 == corners->hge[i * 4] &&
                    t2 == corners->hge[i * 4 + 1] &&
                    (r4 != corners->hge[i * 4] ||
                     t4 != corners->hge[i * 4 + 1]) &&
                    (r42 != corners->hge[i * 4] ||
                     t42 != corners->hge[i * 4 + 1])) {
                    r1 = corners->hge[i * 4];
                    t1 = corners->hge[i * 4 + 1];
                    r12 = corners->hge[i * 4 + 2];
                    t12 = corners->hge[i * 4 + 3];
                    bg = &corners->hg[i];
                    ibg = i;
                    break;
                }
                if (r32 == corners->hge[i * 4] &&
                    t32 == corners->hge[i * 4 + 1] &&
                    r2 == corners->hge[i * 4 + 2] &&
                    t2 == corners->hge[i * 4 + 3] &&
                    (r4 != corners->hge[i * 4] ||
                     t4 != corners->hge[i * 4 + 1]) &&
                    (r42 != corners->hge[i * 4] ||
                     t42 != corners->hge[i * 4 + 1])) {
                    r1 = corners->hge[i * 4];
                    t1 = corners->hge[i * 4 + 1];
                    r12 = corners->hge[i * 4 + 2];
                    t12 = corners->hge[i * 4 + 3];
                    bg = &corners->hg[i];
                    ibg = i;
                    break;
                }
                if (r32 == corners->hge[i * 4 + 2] &&
                    t32 == corners->hge[i * 4 + 3] &&
                    r2 == corners->hge[i * 4] &&
                    t2 == corners->hge[i * 4 + 1] &&
                    (r4 != corners->hge[i * 4] ||
                     t4 != corners->hge[i * 4 + 1]) &&
                    (r42 != corners->hge[i * 4] ||
                     t42 != corners->hge[i * 4 + 1])) {
                    r1 = corners->hge[i * 4];
                    t1 = corners->hge[i * 4 + 1];
                    r12 = corners->hge[i * 4 + 2];
                    t12 = corners->hge[i * 4 + 3];
                    bg = &corners->hg[i];
                    ibg = i;
                    break;
                }
                if (r3 == corners->hge[i * 4] &&
                    t3 == corners->hge[i * 4 + 1] &&
                    r22 == corners->hge[i * 4 + 2] &&
                    t22 == corners->hge[i * 4 + 3] &&
                    (r4 != corners->hge[i * 4] ||
                     t4 != corners->hge[i * 4 + 1]) &&
                    (r42 != corners->hge[i * 4] ||
                     t42 != corners->hge[i * 4 + 1])) {
                    r1 = corners->hge[i * 4];
                    t1 = corners->hge[i * 4 + 1];
                    r12 = corners->hge[i * 4 + 2];
                    t12 = corners->hge[i * 4 + 3];
                    bg = &corners->hg[i];
                    ibg = i;
                    break;
                }
                if (r3 == corners->hge[i * 4 + 2] &&
                    t3 == corners->hge[i * 4 + 3] &&
                    r22 == corners->hge[i * 4] &&
                    t22 == corners->hge[i * 4 + 1] &&
                    (r4 != corners->hge[i * 4] ||
                     t4 != corners->hge[i * 4 + 1]) &&
                    (r42 != corners->hge[i * 4] ||
                     t42 != corners->hge[i * 4 + 1])) {
                    r1 = corners->hge[i * 4];
                    t1 = corners->hge[i * 4 + 1];
                    r12 = corners->hge[i * 4 + 2];
                    t12 = corners->hge[i * 4 + 3];
                    bg = &corners->hg[i];
                    ibg = i;
                    break;
                }
                if (r32 == corners->hge[i * 4] &&
                    t32 == corners->hge[i * 4 + 1] &&
                    r22 == corners->hge[i * 4 + 2] &&
                    t22 == corners->hge[i * 4 + 3] &&
                    (r4 != corners->hge[i * 4] ||
                     t4 != corners->hge[i * 4 + 1]) &&
                    (r42 != corners->hge[i * 4] ||
                     t42 != corners->hge[i * 4 + 1])) {
                    r1 = corners->hge[i * 4];
                    t1 = corners->hge[i * 4 + 1];
                    r12 = corners->hge[i * 4 + 2];
                    t12 = corners->hge[i * 4 + 3];
                    bg = &corners->hg[i];
                    ibg = i;
                    break;
                }
                if (r32 == corners->hge[i * 4 + 2] &&
                    t32 == corners->hge[i * 4 + 3] &&
                    r22 == corners->hge[i * 4] &&
                    t22 == corners->hge[i * 4 + 1] &&
                    (r4 != corners->hge[i * 4] ||
                     t4 != corners->hge[i * 4 + 1]) &&
                    (r42 != corners->hge[i * 4] ||
                     t42 != corners->hge[i * 4 + 1])) {
                    r1 = corners->hge[i * 4];
                    t1 = corners->hge[i * 4 + 1];
                    r12 = corners->hge[i * 4 + 2];
                    t12 = corners->hge[i * 4 + 3];
                    bg = &corners->hg[i];
                    ibg = i;
                    break;
                }
            }
        }
        if (bg && bd && hd) {
            for (size_t i = 0; i < corners->hds; i++) {
                // printf("%lu, %lu, %lu, %lu, %lu, %lu, %lu, %lu, %lu, %lu,
                // %lu, %lu, %lu\n", r4, t4, r12, t12, r2, t2, r22, t22,
                // corners->hde[ i * 4], corners->hde[i * 4 + 1], corners->hde[i
                // * 4 +2], corners->hde[i * 4 + 3], corners->hd[i].x);
                if (r4 == corners->hde[i * 4] &&
                    t4 == corners->hde[i * 4 + 1] &&
                    r1 == corners->hde[i * 4 + 2] &&
                    t1 == corners->hde[i * 4 + 3] &&
                    (r2 != corners->hde[i * 4] ||
                     t2 != corners->hde[i * 4 + 1]) &&
                    (r22 != corners->hde[i * 4] ||
                     t22 != corners->hde[i * 4 + 1])) {
                    r3 = corners->hde[i * 4];
                    t3 = corners->hde[i * 4 + 1];
                    r32 = corners->hde[i * 4 + 2];
                    t32 = corners->hde[i * 4 + 3];
                    hg = &corners->hd[i];
                    ihg = i;
                    break;
                }
                if (r4 == corners->hde[i * 4 + 2] &&
                    t4 == corners->hde[i * 4 + 3] &&
                    r1 == corners->hde[i * 4] &&
                    t1 == corners->hde[i * 4 + 1] &&
                    (r2 != corners->hde[i * 4] ||
                     t2 != corners->hde[i * 4 + 1]) &&
                    (r22 != corners->hde[i * 4] ||
                     t22 != corners->hde[i * 4 + 1])) {
                    r3 = corners->hde[i * 4];
                    t3 = corners->hde[i * 4 + 1];
                    r32 = corners->hde[i * 4 + 2];
                    t32 = corners->hde[i * 4 + 3];
                    hg = &corners->hd[i];
                    ihg = i;
                    break;
                }
                if (r42 == corners->hde[i * 4] &&
                    t42 == corners->hde[i * 4 + 1] &&
                    r1 == corners->hde[i * 4 + 2] &&
                    t1 == corners->hde[i * 4 + 3] &&
                    (r2 != corners->hde[i * 4] ||
                     t2 != corners->hde[i * 4 + 1]) &&
                    (r22 != corners->hde[i * 4] ||
                     t22 != corners->hde[i * 4 + 1])) {
                    r3 = corners->hde[i * 4];
                    t3 = corners->hde[i * 4 + 1];
                    r32 = corners->hde[i * 4 + 2];
                    t32 = corners->hde[i * 4 + 3];
                    hg = &corners->hd[i];
                    ihg = i;
                    break;
                }
                if (r42 == corners->hde[i * 4 + 2] &&
                    t42 == corners->hde[i * 4 + 3] &&
                    r1 == corners->hde[i * 4] &&
                    t1 == corners->hde[i * 4 + 1] &&
                    (r2 != corners->hde[i * 4] ||
                     t2 != corners->hde[i * 4 + 1]) &&
                    (r22 != corners->hde[i * 4] ||
                     t22 != corners->hde[i * 4 + 1])) {
                    r3 = corners->hde[i * 4];
                    t3 = corners->hde[i * 4 + 1];
                    r32 = corners->hde[i * 4 + 2];
                    t32 = corners->hde[i * 4 + 3];
                    hg = &corners->hd[i];
                    ihg = i;
                    break;
                }
                if (r4 == corners->hde[i * 4 + 2] &&
                    t4 == corners->hde[i * 4 + 3] &&
                    r12 == corners->hde[i * 4] &&
                    t12 == corners->hde[i * 4 + 1] &&
                    (r2 != corners->hde[i * 4] ||
                     t2 != corners->hde[i * 4 + 1]) &&
                    (r22 != corners->hde[i * 4] ||
                     t22 != corners->hde[i * 4 + 1])) {
                    r3 = corners->hde[i * 4];
                    t3 = corners->hde[i * 4 + 1];
                    r32 = corners->hde[i * 4 + 2];
                    t32 = corners->hde[i * 4 + 3];
                    hg = &corners->hd[i];
                    ihg = i;
                    break;
                }
                if (r4 == corners->hde[i * 4] &&
                    t4 == corners->hde[i * 4 + 1] &&
                    r12 == corners->hde[i * 4 + 2] &&
                    t12 == corners->hde[i * 4 + 3] &&
                    (r2 != corners->hde[i * 4] ||
                     t2 != corners->hde[i * 4 + 1]) &&
                    (r22 != corners->hde[i * 4] ||
                     t22 != corners->hde[i * 4 + 1])) {
                    r3 = corners->hde[i * 4];
                    t3 = corners->hde[i * 4 + 1];
                    r32 = corners->hde[i * 4 + 2];
                    t32 = corners->hde[i * 4 + 3];
                    hg = &corners->hd[i];
                    ihg = i;
                    break;
                }
                if (r42 == corners->hde[i * 4 + 2] &&
                    t42 == corners->hde[i * 4 + 3] &&
                    r12 == corners->hde[i * 4] &&
                    t12 == corners->hde[i * 4 + 1] &&
                    (r2 != corners->hde[i * 4] ||
                     t2 != corners->hde[i * 4 + 1]) &&
                    (r22 != corners->hde[i * 4] ||
                     t22 != corners->hde[i * 4 + 1])) {
                    r3 = corners->hde[i * 4];
                    t3 = corners->hde[i * 4 + 1];
                    r32 = corners->hde[i * 4 + 2];
                    t32 = corners->hde[i * 4 + 3];
                    hg = &corners->hd[i];
                    ihg = i;
                    break;
                }
                if (r42 == corners->hde[i * 4] &&
                    t42 == corners->hde[i * 4 + 1] &&
                    r12 == corners->hde[i * 4 + 2] &&
                    t12 == corners->hde[i * 4 + 3] &&
                    (r2 != corners->hde[i * 4] ||
                     t2 != corners->hde[i * 4 + 1]) &&
                    (r22 != corners->hde[i * 4] ||
                     t22 != corners->hde[i * 4 + 1])) {
                    r3 = corners->hde[i * 4];
                    t3 = corners->hde[i * 4 + 1];
                    r32 = corners->hde[i * 4 + 2];
                    t32 = corners->hde[i * 4 + 3];
                    hg = &corners->hd[i];
                    ihg = i;
                    break;
                }
            }

            for (size_t i = 0; i < corners->bds; i++) {
                if (r4 == corners->bde[i * 4] &&
                    t4 == corners->bde[i * 4 + 1] &&
                    r1 == corners->bde[i * 4 + 2] &&
                    t1 == corners->bde[i * 4 + 3] &&
                    (r2 != corners->bde[i * 4] ||
                     t2 != corners->bde[i * 4 + 1]) &&
                    (r22 != corners->bde[i * 4] ||
                     t22 != corners->bde[i * 4 + 1])) {
                    r3 = corners->bde[i * 4];
                    t3 = corners->bde[i * 4 + 1];
                    r32 = corners->bde[i * 4 + 2];
                    t32 = corners->bde[i * 4 + 3];
                    hg = &corners->bd[i];
                    ihg = i;
                    break;
                }
                if (r4 == corners->bde[i * 4 + 2] &&
                    t4 == corners->bde[i * 4 + 3] &&
                    r1 == corners->bde[i * 4] &&
                    t1 == corners->bde[i * 4 + 1] &&
                    (r2 != corners->bde[i * 4] ||
                     t2 != corners->bde[i * 4 + 1]) &&
                    (r22 != corners->bde[i * 4] ||
                     t22 != corners->bde[i * 4 + 1])) {
                    r3 = corners->bde[i * 4];
                    t3 = corners->bde[i * 4 + 1];
                    r32 = corners->bde[i * 4 + 2];
                    t32 = corners->bde[i * 4 + 3];
                    hg = &corners->bd[i];
                    ihg = i;
                    break;
                }
                if (r42 == corners->bde[i * 4] &&
                    t42 == corners->bde[i * 4 + 1] &&
                    r1 == corners->bde[i * 4 + 2] &&
                    t1 == corners->bde[i * 4 + 3] &&
                    (r2 != corners->bde[i * 4] ||
                     t2 != corners->bde[i * 4 + 1]) &&
                    (r22 != corners->bde[i * 4] ||
                     t22 != corners->bde[i * 4 + 1])) {
                    r3 = corners->bde[i * 4];
                    t3 = corners->bde[i * 4 + 1];
                    r32 = corners->bde[i * 4 + 2];
                    t32 = corners->bde[i * 4 + 3];
                    hg = &corners->bd[i];
                    ihg = i;
                    break;
                }
                if (r42 == corners->bde[i * 4 + 2] &&
                    t42 == corners->bde[i * 4 + 3] &&
                    r1 == corners->bde[i * 4] &&
                    t1 == corners->bde[i * 4 + 1] &&
                    (r2 != corners->bde[i * 4] ||
                     t2 != corners->bde[i * 4 + 1]) &&
                    (r22 != corners->bde[i * 4] ||
                     t22 != corners->bde[i * 4 + 1])) {
                    r3 = corners->bde[i * 4];
                    t3 = corners->bde[i * 4 + 1];
                    r32 = corners->bde[i * 4 + 2];
                    t32 = corners->bde[i * 4 + 3];
                    hg = &corners->bd[i];
                    ihg = i;
                    break;
                }
                if (r4 == corners->bde[i * 4] &&
                    t4 == corners->bde[i * 4 + 1] &&
                    r12 == corners->bde[i * 4 + 2] &&
                    t12 == corners->bde[i * 4 + 3] &&
                    (r2 != corners->bde[i * 4] ||
                     t2 != corners->bde[i * 4 + 1]) &&
                    (r22 != corners->bde[i * 4] ||
                     t22 != corners->bde[i * 4 + 1])) {
                    r3 = corners->bde[i * 4];
                    t3 = corners->bde[i * 4 + 1];
                    r32 = corners->bde[i * 4 + 2];
                    t32 = corners->bde[i * 4 + 3];
                    hg = &corners->bd[i];
                    ihg = i;
                    break;
                }
                if (r4 == corners->bde[i * 4 + 2] &&
                    t4 == corners->bde[i * 4 + 3] &&
                    r12 == corners->bde[i * 4] &&
                    t12 == corners->bde[i * 4 + 1] &&
                    (r2 != corners->bde[i * 4] ||
                     t2 != corners->bde[i * 4 + 1]) &&
                    (r22 != corners->bde[i * 4] ||
                     t22 != corners->bde[i * 4 + 1])) {
                    r3 = corners->bde[i * 4];
                    t3 = corners->bde[i * 4 + 1];
                    r32 = corners->bde[i * 4 + 2];
                    t32 = corners->bde[i * 4 + 3];
                    hg = &corners->bd[i];
                    ihg = i;
                    break;
                }
                if (r42 == corners->bde[i * 4] &&
                    t42 == corners->bde[i * 4 + 1] &&
                    r12 == corners->bde[i * 4 + 2] &&
                    t12 == corners->bde[i * 4 + 3] &&
                    (r2 != corners->bde[i * 4] ||
                     t2 != corners->bde[i * 4 + 1]) &&
                    (r22 != corners->bde[i * 4] ||
                     t22 != corners->bde[i * 4 + 1])) {
                    r3 = corners->bde[i * 4];
                    t3 = corners->bde[i * 4 + 1];
                    r32 = corners->bde[i * 4 + 2];
                    t32 = corners->bde[i * 4 + 3];
                    hg = &corners->bd[i];
                    ihg = i;
                    break;
                }
                if (r42 == corners->bde[i * 4 + 2] &&
                    t42 == corners->bde[i * 4 + 3] &&
                    r12 == corners->bde[i * 4] &&
                    t12 == corners->bde[i * 4 + 1] &&
                    (r2 != corners->bde[i * 4] ||
                     t2 != corners->bde[i * 4 + 1]) &&
                    (r22 != corners->bde[i * 4] ||
                     t22 != corners->bde[i * 4 + 1])) {
                    r3 = corners->bde[i * 4];
                    t3 = corners->bde[i * 4 + 1];
                    r32 = corners->bde[i * 4 + 2];
                    t32 = corners->bde[i * 4 + 3];
                    hg = &corners->bd[i];
                    ihg = i;
                    break;
                }
            }

            for (size_t i = 0; i < corners->bgs; i++) {
                if (r4 == corners->bge[i * 4] &&
                    t4 == corners->bge[i * 4 + 1] &&
                    r1 == corners->bge[i * 4 + 2] &&
                    t1 == corners->bge[i * 4 + 3] &&
                    (r2 != corners->bge[i * 4] ||
                     t2 != corners->bge[i * 4 + 1]) &&
                    (r22 != corners->bge[i * 4] ||
                     t22 != corners->bge[i * 4 + 1])) {
                    r3 = corners->bge[i * 4];
                    t3 = corners->bge[i * 4 + 1];
                    r32 = corners->bge[i * 4 + 2];
                    t32 = corners->bge[i * 4 + 3];
                    hg = &corners->bg[i];
                    ihg = i;
                    break;
                }
                if (r4 == corners->bge[i * 4 + 2] &&
                    t4 == corners->bge[i * 4 + 3] &&
                    r1 == corners->bge[i * 4] &&
                    t1 == corners->bge[i * 4 + 1] &&
                    (r2 != corners->bge[i * 4] ||
                     t2 != corners->bge[i * 4 + 1]) &&
                    (r22 != corners->bge[i * 4] ||
                     t22 != corners->bge[i * 4 + 1])) {
                    r3 = corners->bge[i * 4];
                    t3 = corners->bge[i * 4 + 1];
                    r32 = corners->bge[i * 4 + 2];
                    t32 = corners->bge[i * 4 + 3];
                    hg = &corners->bg[i];
                    ihg = i;
                    break;
                }
                if (r42 == corners->bge[i * 4] &&
                    t42 == corners->bge[i * 4 + 1] &&
                    r1 == corners->bge[i * 4 + 2] &&
                    t1 == corners->bge[i * 4 + 3] &&
                    (r2 != corners->bge[i * 4] ||
                     t2 != corners->bge[i * 4 + 1]) &&
                    (r22 != corners->bge[i * 4] ||
                     t22 != corners->bge[i * 4 + 1])) {
                    r3 = corners->bge[i * 4];
                    t3 = corners->bge[i * 4 + 1];
                    r32 = corners->bge[i * 4 + 2];
                    t32 = corners->bge[i * 4 + 3];
                    hg = &corners->bg[i];
                    ihg = i;
                    break;
                }
                if (r42 == corners->bge[i * 4 + 2] &&
                    t42 == corners->bge[i * 4 + 3] &&
                    r1 == corners->bge[i * 4] &&
                    t1 == corners->bge[i * 4 + 1] &&
                    (r2 != corners->bge[i * 4] ||
                     t2 != corners->bge[i * 4 + 1]) &&
                    (r22 != corners->bge[i * 4] ||
                     t22 != corners->bge[i * 4 + 1])) {
                    r3 = corners->bge[i * 4];
                    t3 = corners->bge[i * 4 + 1];
                    r32 = corners->bge[i * 4 + 2];
                    t32 = corners->bge[i * 4 + 3];
                    hg = &corners->bg[i];
                    ihg = i;
                    break;
                }
                if (r4 == corners->bge[i * 4] &&
                    t4 == corners->bge[i * 4 + 1] &&
                    r12 == corners->bge[i * 4 + 2] &&
                    t12 == corners->bge[i * 4 + 3] &&
                    (r2 != corners->bge[i * 4] ||
                     t2 != corners->bge[i * 4 + 1]) &&
                    (r22 != corners->bge[i * 4] ||
                     t22 != corners->bge[i * 4 + 1])) {
                    r3 = corners->bge[i * 4];
                    t3 = corners->bge[i * 4 + 1];
                    r32 = corners->bge[i * 4 + 2];
                    t32 = corners->bge[i * 4 + 3];
                    hg = &corners->bg[i];
                    ihg = i;
                    break;
                }
                if (r4 == corners->bge[i * 4 + 2] &&
                    t4 == corners->bge[i * 4 + 3] &&
                    r12 == corners->bge[i * 4] &&
                    t12 == corners->bge[i * 4 + 1] &&
                    (r2 != corners->bge[i * 4] ||
                     t2 != corners->bge[i * 4 + 1]) &&
                    (r22 != corners->bge[i * 4] ||
                     t22 != corners->bge[i * 4 + 1])) {
                    r3 = corners->bge[i * 4];
                    t3 = corners->bge[i * 4 + 1];
                    r32 = corners->bge[i * 4 + 2];
                    t32 = corners->bge[i * 4 + 3];
                    hg = &corners->bg[i];
                    ihg = i;
                    break;
                }
                if (r42 == corners->bge[i * 4] &&
                    t42 == corners->bge[i * 4 + 1] &&
                    r12 == corners->bge[i * 4 + 2] &&
                    t12 == corners->bge[i * 4 + 3] &&
                    (r2 != corners->bge[i * 4] ||
                     t2 != corners->bge[i * 4 + 1]) &&
                    (r22 != corners->bge[i * 4] ||
                     t22 != corners->bge[i * 4 + 1])) {
                    r3 = corners->bge[i * 4];
                    t3 = corners->bge[i * 4 + 1];
                    r32 = corners->bge[i * 4 + 2];
                    t32 = corners->bge[i * 4 + 3];
                    hg = &corners->bg[i];
                    ihg = i;
                    break;
                }
                if (r42 == corners->bge[i * 4 + 2] &&
                    t42 == corners->bge[i * 4 + 3] &&
                    r12 == corners->bge[i * 4] &&
                    t12 == corners->bge[i * 4 + 1] &&
                    (r2 != corners->bge[i * 4] ||
                     t2 != corners->bge[i * 4 + 1]) &&
                    (r22 != corners->bge[i * 4] ||
                     t22 != corners->bge[i * 4 + 1])) {
                    r3 = corners->bge[i * 4];
                    t3 = corners->bge[i * 4 + 1];
                    r32 = corners->bge[i * 4 + 2];
                    t32 = corners->bge[i * 4 + 3];
                    hg = &corners->bg[i];
                    ihg = i;
                    break;
                }
            }

            for (size_t i = 0; i < corners->hgs; i++) {
                if (r4 == corners->hge[i * 4] &&
                    t4 == corners->hge[i * 4 + 1] &&
                    r1 == corners->hge[i * 4 + 2] &&
                    t1 == corners->hge[i * 4 + 3] &&
                    (r2 != corners->hge[i * 4] ||
                     t2 != corners->hge[i * 4 + 1]) &&
                    (r22 != corners->hge[i * 4] ||
                     t22 != corners->hge[i * 4 + 1])) {
                    r3 = corners->hge[i * 4];
                    t3 = corners->hge[i * 4 + 1];
                    r32 = corners->hge[i * 4 + 2];
                    t32 = corners->hge[i * 4 + 3];
                    hg = &corners->hg[i];
                    ihg = i;
                    break;
                }
                if (r4 == corners->hge[i * 4 + 2] &&
                    t4 == corners->hge[i * 4 + 3] &&
                    r1 == corners->hge[i * 4] &&
                    t1 == corners->hge[i * 4 + 1] &&
                    (r2 != corners->hge[i * 4] ||
                     t2 != corners->hge[i * 4 + 1]) &&
                    (r22 != corners->hge[i * 4] ||
                     t22 != corners->hge[i * 4 + 1])) {
                    r3 = corners->hge[i * 4];
                    t3 = corners->hge[i * 4 + 1];
                    r32 = corners->hge[i * 4 + 2];
                    t32 = corners->hge[i * 4 + 3];
                    hg = &corners->hg[i];
                    ihg = i;
                    break;
                }
                if (r42 == corners->hge[i * 4] &&
                    t42 == corners->hge[i * 4 + 1] &&
                    r1 == corners->hge[i * 4 + 2] &&
                    t1 == corners->hge[i * 4 + 3] &&
                    (r2 != corners->hge[i * 4] ||
                     t2 != corners->hge[i * 4 + 1]) &&
                    (r22 != corners->hge[i * 4] ||
                     t22 != corners->hge[i * 4 + 1])) {
                    r3 = corners->hge[i * 4];
                    t3 = corners->hge[i * 4 + 1];
                    r32 = corners->hge[i * 4 + 2];
                    t32 = corners->hge[i * 4 + 3];
                    hg = &corners->hg[i];
                    ihg = i;
                    break;
                }
                if (r42 == corners->hge[i * 4 + 2] &&
                    t42 == corners->hge[i * 4 + 3] &&
                    r1 == corners->hge[i * 4] &&
                    t1 == corners->hge[i * 4 + 1] &&
                    (r2 != corners->hge[i * 4] ||
                     t2 != corners->hge[i * 4 + 1]) &&
                    (r22 != corners->hge[i * 4] ||
                     t22 != corners->hge[i * 4 + 1])) {
                    r3 = corners->hge[i * 4];
                    t3 = corners->hge[i * 4 + 1];
                    r32 = corners->hge[i * 4 + 2];
                    t32 = corners->hge[i * 4 + 3];
                    hg = &corners->hg[i];
                    ihg = i;
                    break;
                }
                if (r4 == corners->hge[i * 4] &&
                    t4 == corners->hge[i * 4 + 1] &&
                    r12 == corners->hge[i * 4 + 2] &&
                    t12 == corners->hge[i * 4 + 3] &&
                    (r2 != corners->hge[i * 4] ||
                     t2 != corners->hge[i * 4 + 1]) &&
                    (r22 != corners->hge[i * 4] ||
                     t22 != corners->hge[i * 4 + 1])) {
                    r3 = corners->hge[i * 4];
                    t3 = corners->hge[i * 4 + 1];
                    r32 = corners->hge[i * 4 + 2];
                    t32 = corners->hge[i * 4 + 3];
                    hg = &corners->hg[i];
                    ihg = i;
                    break;
                }
                if (r4 == corners->hge[i * 4 + 2] &&
                    t4 == corners->hge[i * 4 + 3] &&
                    r12 == corners->hge[i * 4] &&
                    t12 == corners->hge[i * 4 + 1] &&
                    (r2 != corners->hge[i * 4] ||
                     t2 != corners->hge[i * 4 + 1]) &&
                    (r22 != corners->hge[i * 4] ||
                     t22 != corners->hge[i * 4 + 1])) {
                    r3 = corners->hge[i * 4];
                    t3 = corners->hge[i * 4 + 1];
                    r32 = corners->hge[i * 4 + 2];
                    t32 = corners->hge[i * 4 + 3];
                    hg = &corners->hg[i];
                    ihg = i;
                    break;
                }
                if (r42 == corners->hge[i * 4] &&
                    t42 == corners->hge[i * 4 + 1] &&
                    r12 == corners->hge[i * 4 + 2] &&
                    t12 == corners->hge[i * 4 + 3] &&
                    (r2 != corners->hge[i * 4] ||
                     t2 != corners->hge[i * 4 + 1]) &&
                    (r22 != corners->hge[i * 4] ||
                     t22 != corners->hge[i * 4 + 1])) {
                    r3 = corners->hge[i * 4];
                    t3 = corners->hge[i * 4 + 1];
                    r32 = corners->hge[i * 4 + 2];
                    t32 = corners->hge[i * 4 + 3];
                    hg = &corners->hg[i];
                    ihg = i;
                    break;
                }
                if (r42 == corners->hge[i * 4 + 2] &&
                    t42 == corners->hge[i * 4 + 3] &&
                    r12 == corners->hge[i * 4] &&
                    t12 == corners->hge[i * 4 + 1] &&
                    (r2 != corners->hge[i * 4] ||
                     t2 != corners->hge[i * 4 + 1]) &&
                    (r22 != corners->hge[i * 4] ||
                     t22 != corners->hge[i * 4 + 1])) {
                    r3 = corners->hge[i * 4];
                    t3 = corners->hge[i * 4 + 1];
                    r32 = corners->hge[i * 4 + 2];
                    t32 = corners->hge[i * 4 + 3];
                    hg = &corners->hg[i];
                    ihg = i;
                    break;
                }
            }
        }
        if (hd && bg && hg) {
            for (size_t i = 0; i < corners->hds; i++) {
                if (r4 == corners->hde[i * 4] &&
                    t4 == corners->hde[i * 4 + 1] &&
                    r1 == corners->hde[i * 4 + 2] &&
                    t1 == corners->hde[i * 4 + 3] &&
                    r3 != corners->hde[i * 4] && r32 != corners->hde[i * 4]) {
                    r2 = corners->hde[i * 4];
                    t2 = corners->hde[i * 4 + 1];
                    r22 = corners->hde[i * 4 + 2];
                    t22 = corners->hde[i * 4 + 3];
                    bd = &corners->hd[i];
                    ibd = i;
                    break;
                }
                if (r4 == corners->hde[i * 4 + 2] &&
                    t4 == corners->hde[i * 4 + 3] &&
                    r1 == corners->hde[i * 4] &&
                    t1 == corners->hde[i * 4 + 1] &&
                    r3 != corners->hde[i * 4] && r32 != corners->hde[i * 4]) {
                    r2 = corners->hde[i * 4];
                    t2 = corners->hde[i * 4 + 1];
                    r22 = corners->hde[i * 4 + 2];
                    t22 = corners->hde[i * 4 + 3];
                    bd = &corners->hd[i];
                    ibd = i;
                    break;
                }
                if (r42 == corners->hde[i * 4] &&
                    t42 == corners->hde[i * 4 + 1] &&
                    r1 == corners->hde[i * 4 + 2] &&
                    t1 == corners->hde[i * 4 + 3] &&
                    r3 != corners->hde[i * 4] && r32 != corners->hde[i * 4]) {
                    r2 = corners->hde[i * 4];
                    t2 = corners->hde[i * 4 + 1];
                    r22 = corners->hde[i * 4 + 2];
                    t22 = corners->hde[i * 4 + 3];
                    bd = &corners->hd[i];
                    ibd = i;
                    break;
                }
                if (r42 == corners->hde[i * 4 + 2] &&
                    t42 == corners->hde[i * 4 + 3] &&
                    r1 == corners->hde[i * 4] &&
                    t1 == corners->hde[i * 4 + 1] &&
                    r3 != corners->hde[i * 4] && r32 != corners->hde[i * 4]) {
                    r2 = corners->hde[i * 4];
                    t2 = corners->hde[i * 4 + 1];
                    r22 = corners->hde[i * 4 + 2];
                    t22 = corners->hde[i * 4 + 3];
                    bd = &corners->hd[i];
                    ibd = i;
                    break;
                }
                if (r4 == corners->hde[i * 4] &&
                    t4 == corners->hde[i * 4 + 1] &&
                    r12 == corners->hde[i * 4 + 2] &&
                    t12 == corners->hde[i * 4 + 3] &&
                    r3 != corners->hde[i * 4] && r32 != corners->hde[i * 4]) {
                    r2 = corners->hde[i * 4];
                    t2 = corners->hde[i * 4 + 1];
                    r22 = corners->hde[i * 4 + 2];
                    t22 = corners->hde[i * 4 + 3];
                    bd = &corners->hd[i];
                    ibd = i;
                    break;
                }
                if (r4 == corners->hde[i * 4 + 2] &&
                    t4 == corners->hde[i * 4 + 3] &&
                    r12 == corners->hde[i * 4] &&
                    t12 == corners->hde[i * 4 + 1] &&
                    r3 != corners->hde[i * 4] && r32 != corners->hde[i * 4]) {
                    r2 = corners->hde[i * 4];
                    t2 = corners->hde[i * 4 + 1];
                    r22 = corners->hde[i * 4 + 2];
                    t22 = corners->hde[i * 4 + 3];
                    bd = &corners->hd[i];
                    ibd = i;
                    break;
                }
                if (r42 == corners->hde[i * 4] &&
                    t42 == corners->hde[i * 4 + 1] &&
                    r12 == corners->hde[i * 4 + 2] &&
                    t12 == corners->hde[i * 4 + 3] &&
                    r3 != corners->hde[i * 4] && r32 != corners->hde[i * 4]) {
                    r2 = corners->hde[i * 4];
                    t2 = corners->hde[i * 4 + 1];
                    r22 = corners->hde[i * 4 + 2];
                    t22 = corners->hde[i * 4 + 3];
                    bd = &corners->hd[i];
                    ibd = i;
                    break;
                }
                if (r42 == corners->hde[i * 4 + 2] &&
                    t42 == corners->hde[i * 4 + 3] &&
                    r12 == corners->hde[i * 4] &&
                    t12 == corners->hde[i * 4 + 1] &&
                    r3 != corners->hde[i * 4] && r32 != corners->hde[i * 4]) {
                    r2 = corners->hde[i * 4];
                    t2 = corners->hde[i * 4 + 1];
                    r22 = corners->hde[i * 4 + 2];
                    t22 = corners->hde[i * 4 + 3];
                    bd = &corners->hd[i];
                    ibd = i;
                    break;
                }
            }

            for (size_t i = 0; i < corners->bds; i++) {
                if (r4 == corners->bde[i * 4] &&
                    t4 == corners->bde[i * 4 + 1] &&
                    r1 == corners->bde[i * 4 + 2] &&
                    t1 == corners->bde[i * 4 + 3] &&
                    r3 != corners->bde[i * 4] && r32 != corners->bde[i * 4]) {
                    r2 = corners->bde[i * 4];
                    t2 = corners->bde[i * 4 + 1];
                    r22 = corners->bde[i * 4 + 2];
                    t22 = corners->bde[i * 4 + 3];
                    bd = &corners->bd[i];
                    ibd = i;
                    break;
                }
                if (r4 == corners->bde[i * 4 + 2] &&
                    t4 == corners->bde[i * 4 + 3] &&
                    r1 == corners->bde[i * 4] &&
                    t1 == corners->bde[i * 4 + 1] &&
                    r3 != corners->bde[i * 4] && r32 != corners->bde[i * 4]) {
                    r2 = corners->bde[i * 4];
                    t2 = corners->bde[i * 4 + 1];
                    r22 = corners->bde[i * 4 + 2];
                    t22 = corners->bde[i * 4 + 3];
                    bd = &corners->bd[i];
                    ibd = i;
                    break;
                }
                if (r42 == corners->bde[i * 4] &&
                    t42 == corners->bde[i * 4 + 1] &&
                    r1 == corners->bde[i * 4 + 2] &&
                    t1 == corners->bde[i * 4 + 3] &&
                    r3 != corners->bde[i * 4] && r32 != corners->bde[i * 4]) {
                    r2 = corners->bde[i * 4];
                    t2 = corners->bde[i * 4 + 1];
                    r22 = corners->bde[i * 4 + 2];
                    t22 = corners->bde[i * 4 + 3];
                    bd = &corners->bd[i];
                    ibd = i;
                    break;
                }
                if (r42 == corners->bde[i * 4 + 2] &&
                    t42 == corners->bde[i * 4 + 3] &&
                    r1 == corners->bde[i * 4] &&
                    t1 == corners->bde[i * 4 + 1] &&
                    r3 != corners->bde[i * 4] && r32 != corners->bde[i * 4]) {
                    r2 = corners->bde[i * 4];
                    t2 = corners->bde[i * 4 + 1];
                    r22 = corners->bde[i * 4 + 2];
                    t22 = corners->bde[i * 4 + 3];
                    bd = &corners->bd[i];
                    ibd = i;
                    break;
                }
                if (r4 == corners->bde[i * 4] &&
                    t4 == corners->bde[i * 4 + 1] &&
                    r12 == corners->bde[i * 4 + 2] &&
                    t12 == corners->bde[i * 4 + 3] &&
                    r3 != corners->bde[i * 4] && r32 != corners->bde[i * 4]) {
                    r2 = corners->bde[i * 4];
                    t2 = corners->bde[i * 4 + 1];
                    r22 = corners->bde[i * 4 + 2];
                    t22 = corners->bde[i * 4 + 3];
                    bd = &corners->bd[i];
                    ibd = i;
                    break;
                }
                if (r4 == corners->bde[i * 4 + 2] &&
                    t4 == corners->bde[i * 4 + 3] &&
                    r12 == corners->bde[i * 4] &&
                    t12 == corners->bde[i * 4 + 1] &&
                    r3 != corners->bde[i * 4] && r32 != corners->bde[i * 4]) {
                    r2 = corners->bde[i * 4];
                    t2 = corners->bde[i * 4 + 1];
                    r22 = corners->bde[i * 4 + 2];
                    t22 = corners->bde[i * 4 + 3];
                    bd = &corners->bd[i];
                    ibd = i;
                    break;
                }
                if (r42 == corners->bde[i * 4] &&
                    t42 == corners->bde[i * 4 + 1] &&
                    r12 == corners->bde[i * 4 + 2] &&
                    t12 == corners->bde[i * 4 + 3] &&
                    r3 != corners->bde[i * 4] && r32 != corners->bde[i * 4]) {
                    r2 = corners->bde[i * 4];
                    t2 = corners->bde[i * 4 + 1];
                    r22 = corners->bde[i * 4 + 2];
                    t22 = corners->bde[i * 4 + 3];
                    bd = &corners->bd[i];
                    ibd = i;
                    break;
                }
                if (r42 == corners->bde[i * 4 + 2] &&
                    t42 == corners->bde[i * 4 + 3] &&
                    r12 == corners->bde[i * 4] &&
                    t12 == corners->bde[i * 4 + 1] &&
                    r3 != corners->bde[i * 4] && r32 != corners->bde[i * 4]) {
                    r2 = corners->bde[i * 4];
                    t2 = corners->bde[i * 4 + 1];
                    r22 = corners->bde[i * 4 + 2];
                    t22 = corners->bde[i * 4 + 3];
                    bd = &corners->bd[i];
                    ibd = i;
                    break;
                }
            }

            for (size_t i = 0; i < corners->bgs; i++) {
                if (r4 == corners->bge[i * 4] &&
                    t4 == corners->bge[i * 4 + 1] &&
                    r1 == corners->bge[i * 4 + 2] &&
                    t1 == corners->bge[i * 4 + 3] &&
                    r3 != corners->bge[i * 4] && r32 != corners->bge[i * 4]) {
                    r2 = corners->bge[i * 4];
                    t2 = corners->bge[i * 4 + 1];
                    r22 = corners->bge[i * 4 + 2];
                    t22 = corners->bge[i * 4 + 3];
                    bd = &corners->bg[i];
                    ibd = i;
                    break;
                }
                if (r4 == corners->bge[i * 4 + 2] &&
                    t4 == corners->bge[i * 4 + 3] &&
                    r1 == corners->bge[i * 4] &&
                    t1 == corners->bge[i * 4 + 1] &&
                    r3 != corners->bge[i * 4] && r32 != corners->bge[i * 4]) {
                    r2 = corners->bge[i * 4];
                    t2 = corners->bge[i * 4 + 1];
                    r22 = corners->bge[i * 4 + 2];
                    t22 = corners->bge[i * 4 + 3];
                    bd = &corners->bg[i];
                    ibd = i;
                    break;
                }
                if (r42 == corners->bge[i * 4] &&
                    t42 == corners->bge[i * 4 + 1] &&
                    r1 == corners->bge[i * 4 + 2] &&
                    t1 == corners->bge[i * 4 + 3] &&
                    r3 != corners->bge[i * 4] && r32 != corners->bge[i * 4]) {
                    r2 = corners->bge[i * 4];
                    t2 = corners->bge[i * 4 + 1];
                    r22 = corners->bge[i * 4 + 2];
                    t22 = corners->bge[i * 4 + 3];
                    bd = &corners->bg[i];
                    ibd = i;
                    break;
                }
                if (r42 == corners->bge[i * 4 + 2] &&
                    t42 == corners->bge[i * 4 + 3] &&
                    r1 == corners->bge[i * 4] &&
                    t1 == corners->bge[i * 4 + 1] &&
                    r3 != corners->bge[i * 4] && r32 != corners->bge[i * 4]) {
                    r2 = corners->bge[i * 4];
                    t2 = corners->bge[i * 4 + 1];
                    r22 = corners->bge[i * 4 + 2];
                    t22 = corners->bge[i * 4 + 3];
                    bd = &corners->bg[i];
                    ibd = i;
                    break;
                }
                if (r4 == corners->bge[i * 4] &&
                    t4 == corners->bge[i * 4 + 1] &&
                    r12 == corners->bge[i * 4 + 2] &&
                    t12 == corners->bge[i * 4 + 3] &&
                    r3 != corners->bge[i * 4] && r32 != corners->bge[i * 4]) {
                    r2 = corners->bge[i * 4];
                    t2 = corners->bge[i * 4 + 1];
                    r22 = corners->bge[i * 4 + 2];
                    t22 = corners->bge[i * 4 + 3];
                    bd = &corners->bg[i];
                    ibd = i;
                    break;
                }
                if (r4 == corners->bge[i * 4 + 2] &&
                    t4 == corners->bge[i * 4 + 3] &&
                    r12 == corners->bge[i * 4] &&
                    t12 == corners->bge[i * 4 + 1] &&
                    r3 != corners->bge[i * 4] && r32 != corners->bge[i * 4]) {
                    r2 = corners->bge[i * 4];
                    t2 = corners->bge[i * 4 + 1];
                    r22 = corners->bge[i * 4 + 2];
                    t22 = corners->bge[i * 4 + 3];
                    bd = &corners->bg[i];
                    ibd = i;
                    break;
                }
                if (r42 == corners->bge[i * 4] &&
                    t42 == corners->bge[i * 4 + 1] &&
                    r12 == corners->bge[i * 4 + 2] &&
                    t12 == corners->bge[i * 4 + 3] &&
                    r3 != corners->bge[i * 4] && r32 != corners->bge[i * 4]) {
                    r2 = corners->bge[i * 4];
                    t2 = corners->bge[i * 4 + 1];
                    r22 = corners->bge[i * 4 + 2];
                    t22 = corners->bge[i * 4 + 3];
                    bd = &corners->bg[i];
                    ibd = i;
                    break;
                }
                if (r42 == corners->bge[i * 4 + 2] &&
                    t42 == corners->bge[i * 4 + 3] &&
                    r12 == corners->bge[i * 4] &&
                    t12 == corners->bge[i * 4 + 1] &&
                    r3 != corners->bge[i * 4] && r32 != corners->bge[i * 4]) {
                    r2 = corners->bge[i * 4];
                    t2 = corners->bge[i * 4 + 1];
                    r22 = corners->bge[i * 4 + 2];
                    t22 = corners->bge[i * 4 + 3];
                    bd = &corners->bg[i];
                    ibd = i;
                    break;
                }
            }

            for (size_t i = 0; i < corners->hgs; i++) {
                if (r4 == corners->hge[i * 4] &&
                    t4 == corners->hge[i * 4 + 1] &&
                    r1 == corners->hge[i * 4 + 2] &&
                    t1 == corners->hge[i * 4 + 3] &&
                    r3 != corners->hge[i * 4] && r32 != corners->hge[i * 4]) {
                    r2 = corners->hge[i * 4];
                    t2 = corners->hge[i * 4 + 1];
                    r22 = corners->hge[i * 4 + 2];
                    t22 = corners->hge[i * 4 + 3];
                    bd = &corners->hg[i];
                    ibd = i;
                    break;
                }
                if (r4 == corners->hge[i * 4 + 2] &&
                    t4 == corners->hge[i * 4 + 3] &&
                    r1 == corners->hge[i * 4] &&
                    t1 == corners->hge[i * 4 + 1] &&
                    r3 != corners->hge[i * 4] && r32 != corners->hge[i * 4]) {
                    r2 = corners->hge[i * 4];
                    t2 = corners->hge[i * 4 + 1];
                    r22 = corners->hge[i * 4 + 2];
                    t22 = corners->hge[i * 4 + 3];
                    bd = &corners->hg[i];
                    ibd = i;
                    break;
                }
                if (r42 == corners->hge[i * 4] &&
                    t42 == corners->hge[i * 4 + 1] &&
                    r1 == corners->hge[i * 4 + 2] &&
                    t1 == corners->hge[i * 4 + 3] &&
                    r3 != corners->hge[i * 4] && r32 != corners->hge[i * 4]) {
                    r2 = corners->hge[i * 4];
                    t2 = corners->hge[i * 4 + 1];
                    r22 = corners->hge[i * 4 + 2];
                    t22 = corners->hge[i * 4 + 3];
                    bd = &corners->hg[i];
                    ibd = i;
                    break;
                }
                if (r42 == corners->hge[i * 4 + 2] &&
                    t42 == corners->hge[i * 4 + 3] &&
                    r1 == corners->hge[i * 4] &&
                    t1 == corners->hge[i * 4 + 1] &&
                    r3 != corners->hge[i * 4] && r32 != corners->hge[i * 4]) {
                    r2 = corners->hge[i * 4];
                    t2 = corners->hge[i * 4 + 1];
                    r22 = corners->hge[i * 4 + 2];
                    t22 = corners->hge[i * 4 + 3];
                    bd = &corners->hg[i];
                    ibd = i;
                    break;
                }
                if (r4 == corners->hge[i * 4] &&
                    t4 == corners->hge[i * 4 + 1] &&
                    r12 == corners->hge[i * 4 + 2] &&
                    t12 == corners->hge[i * 4 + 3] &&
                    r3 != corners->hge[i * 4] && r32 != corners->hge[i * 4]) {
                    r2 = corners->hge[i * 4];
                    t2 = corners->hge[i * 4 + 1];
                    r22 = corners->hge[i * 4 + 2];
                    t22 = corners->hge[i * 4 + 3];
                    bd = &corners->hg[i];
                    ibd = i;
                    break;
                }
                if (r4 == corners->hge[i * 4 + 2] &&
                    t4 == corners->hge[i * 4 + 3] &&
                    r12 == corners->hge[i * 4] &&
                    t12 == corners->hge[i * 4 + 1] &&
                    r3 != corners->hge[i * 4] && r32 != corners->hge[i * 4]) {
                    r2 = corners->hge[i * 4];
                    t2 = corners->hge[i * 4 + 1];
                    r22 = corners->hge[i * 4 + 2];
                    t22 = corners->hge[i * 4 + 3];
                    bd = &corners->hg[i];
                    ibd = i;
                    break;
                }
                if (r42 == corners->hge[i * 4] &&
                    t42 == corners->hge[i * 4 + 1] &&
                    r12 == corners->hge[i * 4 + 2] &&
                    t12 == corners->hge[i * 4 + 3] &&
                    r3 != corners->hge[i * 4] && r32 != corners->hge[i * 4]) {
                    r2 = corners->hge[i * 4];
                    t2 = corners->hge[i * 4 + 1];
                    r22 = corners->hge[i * 4 + 2];
                    t22 = corners->hge[i * 4 + 3];
                    bd = &corners->hg[i];
                    ibd = i;
                    break;
                }
                if (r42 == corners->hge[i * 4 + 2] &&
                    t42 == corners->hge[i * 4 + 3] &&
                    r12 == corners->hge[i * 4] &&
                    t12 == corners->hge[i * 4 + 1] &&
                    r3 != corners->hge[i * 4] && r32 != corners->hge[i * 4]) {
                    r2 = corners->hge[i * 4];
                    t2 = corners->hge[i * 4 + 1];
                    r22 = corners->hge[i * 4 + 2];
                    t22 = corners->hge[i * 4 + 3];
                    bd = &corners->hg[i];
                    ibd = i;
                    break;
                }
            }
        }

        else if (hg && hd) {
            if (r3 == r4 && t3 == t4) {
                for (size_t i = 0; i < corners->bgs; i++) {
                    if (r32 == corners->bge[i * 4] &&
                        t32 == corners->bge[i * 4 + 1]) {
                        r1 = corners->bge[i * 4];
                        t1 = corners->bge[i * 4 + 1];
                        r12 = corners->bge[i * 4 + 2];
                        t12 = corners->bge[i * 4 + 3];
                        bg = &corners->bg[i];
                        ibg = i;
                        break;
                    }
                    if (r32 == corners->bge[i * 4 + 2] &&
                        t32 == corners->bge[i * 4 + 3]) {
                        r1 = corners->bge[i * 4];
                        t1 = corners->bge[i * 4 + 1];
                        r12 = corners->bge[i * 4 + 2];
                        t12 = corners->bge[i * 4 + 3];
                        bg = &corners->bg[i];
                        ibg = i;
                        break;
                    }
                }
                for (size_t i = 0; i < corners->bds; i++) {
                    if (r42 == corners->bde[i * 4] &&
                        t42 == corners->bde[i * 4 + 1]) {
                        r2 = corners->bde[i * 4];
                        t2 = corners->bde[i * 4 + 1];
                        r22 = corners->bde[i * 4 + 2];
                        t22 = corners->bde[i * 4 + 3];
                        bd = &corners->bd[i];
                        ibd = i;
                        break;
                    }
                    if (r42 == corners->bde[i * 4 + 2] &&
                        t42 == corners->bde[i * 4 + 3]) {
                        r2 = corners->bde[i * 4];
                        t2 = corners->bde[i * 4 + 1];
                        r22 = corners->bde[i * 4 + 2];
                        t22 = corners->bde[i * 4 + 3];
                        bd = &corners->bd[i];
                        ibd = i;
                        break;
                    }
                }
            } else {
                for (size_t i = 0; i < corners->bgs; i++) {
                    if (r3 == corners->bge[i * 4] &&
                        t3 == corners->bge[i * 4 + 1]) {
                        r1 = corners->bge[i * 4];
                        t1 = corners->bge[i * 4 + 1];
                        r12 = corners->bge[i * 4 + 2];
                        t12 = corners->bge[i * 4 + 3];
                        bg = &corners->bg[i];
                        ibg = i;
                        break;
                    }
                    if (r3 == corners->bge[i * 4 + 2] &&
                        t3 == corners->bge[i * 4 + 3]) {
                        r1 = corners->bge[i * 4];
                        t1 = corners->bge[i * 4 + 1];
                        r12 = corners->bge[i * 4 + 2];
                        t12 = corners->bge[i * 4 + 3];
                        bg = &corners->bg[i];
                        ibg = i;
                        break;
                    }
                }
                for (size_t i = 0; i < corners->bds; i++) {
                    if (r4 == corners->bde[i * 4] &&
                        t4 == corners->bde[i * 4 + 1]) {
                        r2 = corners->bde[i * 4];
                        t2 = corners->bde[i * 4 + 1];
                        r22 = corners->bde[i * 4 + 2];
                        t22 = corners->bde[i * 4 + 3];
                        bd = &corners->bd[i];
                        ibd = i;
                        break;
                    }
                    if (r4 == corners->bde[i * 4 + 2] &&
                        t4 == corners->bde[i * 4 + 3]) {
                        r2 = corners->bde[i * 4];
                        t2 = corners->bde[i * 4 + 1];
                        r22 = corners->bde[i * 4 + 2];
                        t22 = corners->bde[i * 4 + 3];
                        bd = &corners->bd[i];
                        ibd = i;
                        break;
                    }
                }
            }
        } else if (hg && bg) {
            if (r3 == r1 && t3 == t1) {
                for (size_t i = 0; i < corners->hds; i++) {
                    if (r32 == corners->hde[i * 4] &&
                        t32 == corners->hde[i * 4 + 1]) {
                        r4 = corners->hde[i * 4];
                        t4 = corners->hde[i * 4 + 1];
                        r42 = corners->hde[i * 4 + 2];
                        t42 = corners->hde[i * 4 + 3];
                        hd = &corners->hd[i];
                        ihd = i;
                        break;
                    }
                    if (r32 == corners->hde[i * 4 + 2] &&
                        t32 == corners->hde[i * 4 + 3]) {
                        r4 = corners->hde[i * 4];
                        t4 = corners->hde[i * 4 + 1];
                        r42 = corners->hde[i * 4 + 2];
                        t42 = corners->hde[i * 4 + 3];
                        hd = &corners->hd[i];
                        ibd = i;
                        break;
                    }
                }
                for (size_t i = 0; i < corners->bds; i++) {
                    if (r12 == corners->bde[i * 4] &&
                        t12 == corners->bde[i * 4 + 1]) {
                        r2 = corners->bde[i * 4];
                        t2 = corners->bde[i * 4 + 1];
                        r22 = corners->bde[i * 4 + 2];
                        t22 = corners->bde[i * 4 + 3];
                        bd = &corners->bd[i];
                        ibd = i;
                        break;
                    }
                    if (r12 == corners->bde[i * 4 + 2] &&
                        t12 == corners->bde[i * 4 + 3]) {
                        r2 = corners->bde[i * 4];
                        t2 = corners->bde[i * 4 + 1];
                        r22 = corners->bde[i * 4 + 2];
                        t22 = corners->bde[i * 4 + 3];
                        bd = &corners->bd[i];
                        ibd = i;
                        break;
                    }
                }
            } else {
                for (size_t i = 0; i < corners->hds; i++) {
                    if (r3 == corners->hde[i * 4] &&
                        t3 == corners->hde[i * 4 + 1]) {
                        r4 = corners->hde[i * 4];
                        t4 = corners->hde[i * 4 + 1];
                        r42 = corners->hde[i * 4 + 2];
                        t42 = corners->hde[i * 4 + 3];
                        hd = &corners->hd[i];
                        ihd = i;
                        break;
                    }
                    if (r3 == corners->hde[i * 4 + 2] &&
                        t3 == corners->hde[i * 4 + 3]) {
                        r4 = corners->hde[i * 4];
                        t4 = corners->hde[i * 4 + 1];
                        r42 = corners->hde[i * 4 + 2];
                        t42 = corners->hde[i * 4 + 3];
                        hd = &corners->hd[i];
                        ihd = i;
                        break;
                    }
                }
                for (size_t i = 0; i < corners->bds; i++) {
                    if (r1 == corners->bde[i * 4] &&
                        t1 == corners->bde[i * 4 + 1]) {
                        r2 = corners->bde[i * 4];
                        t2 = corners->bde[i * 4 + 1];
                        r22 = corners->bde[i * 4 + 2];
                        t22 = corners->bde[i * 4 + 3];
                        bd = &corners->bd[i];
                        ibd = i;
                        break;
                    }
                    if (r1 == corners->bde[i * 4 + 2] &&
                        t1 == corners->bde[i * 4 + 3]) {
                        r2 = corners->bde[i * 4];
                        t2 = corners->bde[i * 4 + 1];
                        r22 = corners->bde[i * 4 + 2];
                        t22 = corners->bde[i * 4 + 3];
                        bd = &corners->bd[i];
                        ibd = i;
                        break;
                    }
                }
            }
        }

        else if (bd && bg) {
            if (r2 == r1 && t2 == t1) {
                for (size_t i = 0; i < corners->hds; i++) {
                    if (r22 == corners->hde[i * 4] &&
                        t22 == corners->hde[i * 4 + 1]) {
                        r4 = corners->hde[i * 4];
                        t4 = corners->hde[i * 4 + 1];
                        r42 = corners->hde[i * 4 + 2];
                        t42 = corners->hde[i * 4 + 3];
                        hd = &corners->hd[i];
                        ihd = i;
                        break;
                    }
                    if (r22 == corners->hde[i * 4 + 2] &&
                        t22 == corners->hde[i * 4 + 3]) {
                        r4 = corners->hde[i * 4];
                        t4 = corners->hde[i * 4 + 1];
                        r42 = corners->hde[i * 4 + 2];
                        t42 = corners->hde[i * 4 + 3];
                        hd = &corners->hd[i];
                        ihd = i;
                        break;
                    }
                }
                for (size_t i = 0; i < corners->hgs; i++) {
                    if (r12 == corners->hge[i * 4] &&
                        t12 == corners->hge[i * 4 + 1]) {
                        r3 = corners->hge[i * 4];
                        t3 = corners->hge[i * 4 + 1];
                        r32 = corners->hge[i * 4 + 2];
                        t32 = corners->hge[i * 4 + 3];
                        hg = &corners->hg[i];
                        ihg = i;
                        break;
                    }
                    if (r12 == corners->hge[i * 4 + 2] &&
                        t12 == corners->hge[i * 4 + 3]) {
                        r3 = corners->hge[i * 4];
                        t3 = corners->hge[i * 4 + 1];
                        r32 = corners->hge[i * 4 + 2];
                        t32 = corners->hge[i * 4 + 3];
                        hg = &corners->hg[i];
                        ihg = i;
                        break;
                    }
                }
            } else {
                for (size_t i = 0; i < corners->hds; i++) {
                    if (r2 == corners->hde[i * 4] &&
                        t2 == corners->hde[i * 4 + 1]) {
                        r4 = corners->hde[i * 4];
                        t4 = corners->hde[i * 4 + 1];
                        r42 = corners->hde[i * 4 + 2];
                        t42 = corners->hde[i * 4 + 3];
                        hd = &corners->hd[i];
                        ihd = i;
                        break;
                    }
                    if (r2 == corners->hde[i * 4 + 2] &&
                        t2 == corners->hde[i * 4 + 3]) {
                        r4 = corners->hde[i * 4];
                        t4 = corners->hde[i * 4 + 1];
                        r42 = corners->hde[i * 4 + 2];
                        t42 = corners->hde[i * 4 + 3];
                        hd = &corners->hd[i];
                        ihd = i;
                        break;
                    }
                }
                for (size_t i = 0; i < corners->hgs; i++) {
                    if (r1 == corners->hge[i * 4] &&
                        t1 == corners->hge[i * 4 + 1]) {
                        r3 = corners->hge[i * 4];
                        t3 = corners->hge[i * 4 + 1];
                        r32 = corners->hge[i * 4 + 2];
                        t32 = corners->hge[i * 4 + 3];
                        hg = &corners->hg[i];
                        ihg = i;
                        break;
                    }
                    if (r1 == corners->hge[i * 4 + 2] &&
                        t1 == corners->hge[i * 4 + 3]) {
                        r3 = corners->hge[i * 4];
                        t3 = corners->hge[i * 4 + 1];
                        r32 = corners->hge[i * 4 + 2];
                        t32 = corners->hge[i * 4 + 3];
                        hg = &corners->hg[i];
                        ihg = i;
                        break;
                    }
                }
            }
        }

        else if (bd && hd) {
            if (r2 == r4 && t2 == t4) {
                for (size_t i = 0; i < corners->bgs; i++) {
                    if (r22 == corners->bge[i * 4] &&
                        t22 == corners->bge[i * 4 + 1]) {
                        r1 = corners->bge[i * 4];
                        t1 = corners->bge[i * 4 + 1];
                        r12 = corners->bge[i * 4 + 2];
                        t12 = corners->bge[i * 4 + 3];
                        bg = &corners->bg[i];
                        ibg = i;
                        break;
                    }
                    if (r22 == corners->bge[i * 4 + 2] &&
                        t22 == corners->bge[i * 4 + 3]) {
                        r1 = corners->bge[i * 4];
                        t1 = corners->bge[i * 4 + 1];
                        r12 = corners->bge[i * 4 + 2];
                        t12 = corners->bge[i * 4 + 3];
                        bg = &corners->bg[i];
                        ibg = i;
                        break;
                    }
                }
                for (size_t i = 0; i < corners->hgs; i++) {
                    if (r42 == corners->hge[i * 4] &&
                        t42 == corners->hge[i * 4 + 1]) {
                        r3 = corners->hge[i * 4];
                        t3 = corners->hge[i * 4 + 1];
                        r32 = corners->hge[i * 4 + 2];
                        t32 = corners->hge[i * 4 + 3];
                        hg = &corners->hg[i];
                        ihg = i;
                        break;
                    }
                    if (r42 == corners->hge[i * 4 + 2] &&
                        t42 == corners->hge[i * 4 + 3]) {
                        r3 = corners->hge[i * 4];
                        t3 = corners->hge[i * 4 + 1];
                        r32 = corners->hge[i * 4 + 2];
                        t32 = corners->hge[i * 4 + 3];
                        hg = &corners->hg[i];
                        ihg = i;
                        break;
                    }
                }
            } else {
                for (size_t i = 0; i < corners->bgs; i++) {
                    if (r2 == corners->bge[i * 4] &&
                        t2 == corners->bge[i * 4 + 1]) {
                        r1 = corners->bge[i * 4];
                        t1 = corners->bge[i * 4 + 1];
                        r12 = corners->bge[i * 4 + 2];
                        t12 = corners->bge[i * 4 + 3];
                        bg = &corners->bg[i];
                        ibg = i;
                        break;
                    }
                    if (r2 == corners->bge[i * 4 + 2] &&
                        t2 == corners->bge[i * 4 + 3]) {
                        r1 = corners->bge[i * 4];
                        t1 = corners->bge[i * 4 + 1];
                        r12 = corners->bge[i * 4 + 2];
                        t12 = corners->bge[i * 4 + 3];
                        bg = &corners->bg[i];
                        ibg = i;
                        break;
                    }
                }
                for (size_t i = 0; i < corners->hgs; i++) {
                    if (r4 == corners->hge[i * 4] &&
                        t4 == corners->hge[i * 4 + 1]) {
                        r3 = corners->hge[i * 4];
                        t3 = corners->hge[i * 4 + 1];
                        r32 = corners->hge[i * 4 + 2];
                        t32 = corners->hge[i * 4 + 3];
                        hg = &corners->hg[i];
                        ihg = i;
                        break;
                    }
                    if (r4 == corners->hge[i * 4 + 2] &&
                        t4 == corners->hge[i * 4 + 3]) {
                        r3 = corners->hge[i * 4];
                        t3 = corners->hge[i * 4 + 1];
                        r32 = corners->hge[i * 4 + 2];
                        t32 = corners->hge[i * 4 + 3];
                        hg = &corners->hg[i];
                        ihg = i;
                        break;
                    }
                }
            }
        } else if (bg) {
            for (size_t i = 0; i < corners->hgs; i++) {
                if (r1 == corners->hge[i * 4] &&
                    t1 == corners->hge[i * 4 + 1]) {
                    r3 = corners->hge[i * 4];
                    t3 = corners->hge[i * 4 + 1];
                    r32 = corners->hge[i * 4 + 2];
                    t32 = corners->hge[i * 4 + 3];
                    hg = &corners->hg[i];
                    ihg = i;
                    break;
                }
                if (r1 == corners->hge[i * 4 + 2] &&
                    t1 == corners->hge[i * 4 + 3]) {
                    r3 = corners->hge[i * 4];
                    t3 = corners->hge[i * 4 + 1];
                    r32 = corners->hge[i * 4 + 2];
                    t32 = corners->hge[i * 4 + 3];
                    hg = &corners->hg[i];
                    ihg = i;
                    break;
                }
                if (r12 == corners->hge[i * 4] &&
                    t12 == corners->hge[i * 4 + 1]) {
                    r3 = corners->hge[i * 4];
                    t3 = corners->hge[i * 4 + 1];
                    r32 = corners->hge[i * 4 + 2];
                    t32 = corners->hge[i * 4 + 3];
                    hg = &corners->hg[i];
                    ihg = i;
                    break;
                }
                if (r12 == corners->hge[i * 4 + 2] &&
                    t12 == corners->hge[i * 4 + 3]) {
                    r3 = corners->hge[i * 4];
                    t3 = corners->hge[i * 4 + 1];
                    r32 = corners->hge[i * 4 + 2];
                    t32 = corners->hge[i * 4 + 3];
                    hg = &corners->hg[i];
                    ihg = i;
                    break;
                }
            }
        } else if (bd) {
            for (size_t i = 0; i < corners->hds; i++) {
                if (r2 == corners->hde[i * 4] &&
                    t2 == corners->hde[i * 4 + 1]) {
                    r4 = corners->hde[i * 4];
                    t4 = corners->hde[i * 4 + 1];
                    r42 = corners->hde[i * 4 + 2];
                    t42 = corners->hde[i * 4 + 3];
                    hd = &corners->hd[i];
                    ihd = i;
                    break;
                }
                if (r2 == corners->hde[i * 4 + 2] &&
                    t2 == corners->hde[i * 4 + 3]) {
                    r4 = corners->hde[i * 4];
                    t4 = corners->hde[i * 4 + 1];
                    r42 = corners->hde[i * 4 + 2];
                    t42 = corners->hde[i * 4 + 3];
                    hd = &corners->hd[i];
                    ihg = i;
                    break;
                }
                if (r22 == corners->hde[i * 4] &&
                    t22 == corners->hde[i * 4 + 1]) {
                    r4 = corners->hde[i * 4];
                    t4 = corners->hde[i * 4 + 1];
                    r42 = corners->hde[i * 4 + 2];
                    t42 = corners->hde[i * 4 + 3];
                    hd = &corners->hd[i];
                    ihd = i;
                    break;
                }
                if (r22 == corners->hde[i * 4 + 2] &&
                    t22 == corners->hde[i * 4 + 3]) {
                    r4 = corners->hde[i * 4];
                    t4 = corners->hde[i * 4 + 1];
                    r42 = corners->hde[i * 4 + 2];
                    t42 = corners->hde[i * 4 + 3];
                    hd = &corners->hd[i];
                    ihd = i;
                    break;
                }
            }
        } else if (hd) {
            for (size_t i = 0; i < corners->bds; i++) {
                if (r4 == corners->bde[i * 4] &&
                    t4 == corners->bde[i * 4 + 1]) {
                    r2 = corners->bde[i * 4];
                    t2 = corners->bde[i * 4 + 1];
                    r22 = corners->bde[i * 4 + 2];
                    t22 = corners->bde[i * 4 + 3];
                    bd = &corners->bd[i];
                    ibd = i;
                    break;
                }
                if (r4 == corners->bde[i * 4 + 2] &&
                    t4 == corners->bde[i * 4 + 3]) {
                    r2 = corners->bde[i * 4];
                    t2 = corners->bde[i * 4 + 1];
                    r22 = corners->bde[i * 4 + 2];
                    t22 = corners->bde[i * 4 + 3];
                    bd = &corners->bd[i];
                    ibd = i;
                    break;
                }
                if (r42 == corners->bde[i * 4] &&
                    t42 == corners->bde[i * 4 + 1]) {
                    r2 = corners->bde[i * 4];
                    t2 = corners->bde[i * 4 + 1];
                    r22 = corners->bde[i * 4 + 2];
                    t22 = corners->bde[i * 4 + 3];
                    bd = &corners->bd[i];
                    ibd = i;
                    break;
                }
                if (r42 == corners->bde[i * 4 + 2] &&
                    t42 == corners->bde[i * 4 + 3]) {
                    r2 = corners->bde[i * 4];
                    t2 = corners->bde[i * 4 + 1];
                    r22 = corners->bde[i * 4 + 2];
                    t22 = corners->bde[i * 4 + 3];
                    bd = &corners->bd[i];
                    ibd = i;
                    break;
                }
            }
        } else if (hg) {
            for (size_t i = 0; i < corners->bgs; i++) {
                if (r3 == corners->bge[i * 4] &&
                    t3 == corners->bge[i * 4 + 1]) {
                    r1 = corners->bge[i * 4];
                    t1 = corners->bge[i * 4 + 1];
                    r12 = corners->bge[i * 4 + 2];
                    t12 = corners->bge[i * 4 + 3];
                    bg = &corners->bg[i];
                    ibg = i;
                    break;
                }
                if (r3 == corners->bge[i * 4 + 2] &&
                    t3 == corners->bge[i * 4 + 3]) {
                    r1 = corners->bge[i * 4];
                    t1 = corners->bge[i * 4 + 1];
                    r12 = corners->bge[i * 4 + 2];
                    t12 = corners->bge[i * 4 + 3];
                    bg = &corners->bg[i];
                    ibg = i;
                    break;
                }
                if (r32 == corners->bge[i * 4] &&
                    t32 == corners->bge[i * 4 + 1]) {
                    r1 = corners->bge[i * 4];
                    t1 = corners->bge[i * 4 + 1];
                    r12 = corners->bge[i * 4 + 2];
                    t12 = corners->bge[i * 4 + 3];
                    bg = &corners->bg[i];
                    ibg = i;
                    break;
                }
                if (r32 == corners->bge[i * 4 + 2] &&
                    t32 == corners->bge[i * 4 + 3]) {
                    r1 = corners->bge[i * 4];
                    t1 = corners->bge[i * 4 + 1];
                    r12 = corners->bge[i * 4 + 2];
                    t12 = corners->bge[i * 4 + 3];
                    bg = &corners->bg[i];
                    ibg = i;
                    break;
                }
            }
        }
        if (flag == save_flag) {
            switch (save_flag ^ new_flag) {
                case 0:
                    if (bg) {
                        bg = &corners->bg[ibg + 1];
                        r1 = corners->bge[(ibg + 1) * 4];
                        t1 = corners->bge[(ibg + 1) * 4 + 1];
                        r12 = corners->bge[(ibg + 1) * 4 + 2];
                        t12 = corners->bge[(ibg + 1) * 4 + 3];
                        ibg++;
                    } else if (bd) {
                        bd = &corners->bd[ibd + 1];
                        r2 = corners->bde[(ibd + 1) * 4];
                        t2 = corners->bde[(ibd + 1) * 4 + 1];
                        r22 = corners->bde[(ibd + 1) * 4 + 2];
                        t22 = corners->bde[(ibd + 1) * 4 + 3];
                        ibd++;
                    } else if (hg) {
                        hg = &corners->hg[ihg + 1];
                        r3 = corners->hge[(ihg + 1) * 4];
                        t3 = corners->hge[(ihg + 1) * 4 + 1];
                        r32 = corners->hge[(ihg + 1) * 4 + 2];
                        t32 = corners->hge[(ihg + 1) * 4 + 3];
                        ihg++;
                    } else {
                        hd = &corners->hd[ihd + 1];
                        r4 = corners->hde[(ihd + 1) * 4];
                        t4 = corners->hde[(ihd + 1) * 4 + 1];
                        r42 = corners->hde[(ihd + 1) * 4 + 2];
                        t42 = corners->hde[(ihd + 1) * 4 + 3];
                        ihd++;
                    }
                    break;
                case 2:
                    bg = NULL;
                    switch (flag ^ new_flag) {
                        case 4:
                            bd = NULL;
                            break;
                        case 8:
                            hg = NULL;
                            break;
                        case 16:
                            hd = NULL;
                            break;
                    }
                    corners->bge[ibg * 4] = 0;
                    corners->bge[ibg * 4 + 1] = 0;
                    corners->bge[ibg * 4 + 2] = 0;
                    corners->bge[ibg * 4 + 3] = 0;
                    break;
                case 4:
                    bd = NULL;
                    switch (flag ^ new_flag) {
                        case 2:
                            bg = NULL;
                            break;
                        case 8:
                            hg = NULL;
                            break;
                        case 16:
                            hd = NULL;
                            break;
                    }
                    corners->bde[ibd * 4] = 0;
                    corners->bde[ibd * 4 + 1] = 0;
                    corners->bde[ibd * 4 + 2] = 0;
                    corners->bde[ibd * 4 + 3] = 0;
                    break;
                case 8:
                    hg = NULL;
                    switch (flag ^ new_flag) {
                        case 4:
                            bd = NULL;
                            break;
                        case 2:
                            bg = NULL;
                            break;
                        case 16:
                            hd = NULL;
                            break;
                    }
                    corners->hge[ihg * 4] = 0;
                    corners->hge[ihg * 4 + 1] = 0;
                    corners->hge[ihg * 4 + 2] = 0;
                    corners->hge[ihg * 4 + 3] = 0;
                    break;
                case 16:
                    hd = NULL;
                    switch (flag ^ new_flag) {
                        case 4:
                            bd = NULL;
                            break;
                        case 8:
                            hg = NULL;
                            break;
                        case 2:
                            bg = NULL;
                            break;
                    }
                    corners->hde[ihd * 4] = 0;
                    corners->hde[ihd * 4 + 1] = 0;
                    corners->hde[ihd * 4 + 2] = 0;
                    corners->hde[ihd * 4 + 3] = 0;
                    break;
            }
        } else {
            save_flag = flag;
        }
        new_flag = (bg ? 1 : 0) << 1 | (bd ? 1 : 0) << 2 | (hg ? 1 : 0) << 3 |
          (hd ? 1 : 0) << 4;
        printf("$$$$$$$$$$$$$$$$$$\n");
        if (bg) printf("bg: x: %lu, y: %lu\n", bg->x, bg->y);
        if (bd) printf("bd: x: %lu, y: %lu\n", bd->x, bd->y);
        if (hg) printf("hg: x: %lu, y: %lu\n", hg->x, hg->y);
        if (hd) printf("hd: x: %lu, y: %lu\n", hd->x, hd->y);
    }
    printf("##################\n");
    printf("bg: x: %lu, y: %lu\n", bg->x, bg->y);
    printf("bd: x: %lu, y: %lu\n", bd->x, bd->y);
    printf("hg: x: %lu, y: %lu\n", hg->x, hg->y);
    printf("hd: x: %lu, y: %lu\n", hd->x, hd->y);

    corners->bg[0] = *bg;
    corners->bd[0] = *bd;
    corners->hg[0] = *hg;
    corners->hd[0] = *hd;
    corners->bgs = 1;
    corners->bds = 1;
    corners->hgs = 1;
    corners->hds = 1;
}

int
main(int argc, char **argv)
{

    Points_Array *arr;

    if (argc != 3) {
        printf(
          "Usage: %s <path_to_binarized_image> <path_to_grayscale_image>\n",
          argv[0]);
        return 1;
    }

    SDL_Surface *imagee = IMG_Load(argv[1]);
    SDL_Surface *imageee = IMG_Load(argv[2]);

    if (imagee == NULL) {
        printf("Image %s is not valid\n", argv[1]);
        return 1;
    }

    if (imageee == NULL) {
        printf("Image %s is not valid\n", argv[3]);
        return 1;
    }

    SDL_Surface *image_temp =
      SDL_ConvertSurfaceFormat(imagee, SDL_PIXELFORMAT_RGB888, 0);

    SDL_Surface *image_tempe =
      SDL_ConvertSurfaceFormat(imageee, SDL_PIXELFORMAT_RGB888, 0);

    arr = detect_lines(image_temp);

    SDL_LockSurface(image_temp);

    /*    for (size_t i = 0; i < sorted_arr->count_h; i += 2) {
  printf("r: %lu,theta: %lu\n", sorted_arr->horizontal[i],
  sorted_arr->horizontal[i + 1]);
  m = sin(sorted_arr->horizontal[i + 1] * PI / 180);
  n = cos(sorted_arr->horizontal[i + 1] * PI / 180);
  x0 = m * sorted_arr->horizontal[i];
  y0 = n * sorted_arr->horizontal[i];
  x1 = x0 + 2 * image_temp->w * (-n);
  y1 = y0 + 2 * image_temp->h * (m);
  x2 = x0 - 2 * image_temp->w * (-n);
          y2 = y0 - 2 * image_temp->h * (m);
          // draw_line(pixels, image_temp->w, image_temp->h, x1, y1, x2, y2,
          // SDL_MapRGB(image_temp->format, 0, 255, 0));
          draw_line(pixels, image_temp->w, image_temp->h, x1, y1, x2, y2,
                    SDL_MapRGB(image_temp->format, 0, 255, 0));
      }

      for (size_t i = 0; i < sorted_arr->count_v; i += 2) {
          printf("r: %lu,theta: %lu\n", sorted_arr->vertical[i],
                 sorted_arr->vertical[i + 1]);
          m = sin(sorted_arr->vertical[i + 1] * PI / 180);
          n = cos(sorted_arr->vertical[i + 1] * PI / 180);
          x0 = m * sorted_arr->vertical[i];
          y0 = n * sorted_arr->vertical[i];
          x1 = x0 + 2 * image_temp->w * (-n);
          y1 = y0 + 2 * image_temp->h * (m);
          x2 = x0 - 2 * image_temp->w * (-n);
          y2 = y0 - 2 * image_temp->h * (m);
          // draw_line(pixels, image_temp->w, image_temp->h, x1, y1, x2, y2,
          // SDL_MapRGB(image_temp->format, 255, 0, 0));
      }
  */
    // sorted_arr_2 = yeet_the_fools(sorted_arr);

    // for (size_t i = 0; i < arr->len; i += 2) {
    // for (int j = -2; j <= 2; j++) {
    // for (int k = -2; k <= 2; k++) {
    // printf("%ld, %ld, %ld, %ld\n", intersect_arr->array[i] + j,
    // intersect_arr->array[i+ 1] + k, image_temp->w,
    // image_temp->h);
    // change_pixel(image_temp,
    // intersect_arr->array[i] - ABS(j) >= 0 &&
    // intersect_arr->array[i] + ABS(j) < image_temp->h ?
    // intersect_arr->array[i] + j : intersect_arr->array[i],
    // intersect_arr->array[i+1] - ABS(k) >= 0 &&
    // intersect_arr->array[i+1] + ABS(k) < image_temp->w ?
    // intersect_arr->array[i+ 1] + k : intersect_arr->array[i+ 1],
    // SDL_MapRGB(image_temp->format, 0, 255, 0));
    // change_pixel(image_temp, arr->array[i],
    // arr->array[i + 1],
    // SDL_MapRGB(image_temp->format, 255, 0, 0));
    // }
    // }
    // }

    IMG_SavePNG(image_temp, "./test9.png");

    Point_arr_o *corners_arr =
      get_intersection_points(arr, image_temp->w, image_temp->h, image_temp);
    // for (size_t i = 0; i < corners_arr->hgs; i++) {
    // printf("hg x: %lu, y: %lu\n", (corners_arr->hg[i]).x,
    // (corners_arr->hg[i]).y);
    // }
    // for (size_t i = 0; i < corners_arr->hds; i++) {
    // printf("hd x: %lu, y: %lu\n", (corners_arr->hd[i]).x,
    // (corners_arr->hd[i]).y);
    // }
    // for (size_t i = 0; i < corners_arr->bds; i++) {
    // printf("bd x: %lu, y: %lu\n", (corners_arr->bd[i]).x,
    // (corners_arr->bd[i]).y);
    // }
    // for (size_t i = 0; i < corners_arr->bgs; i++) {
    // printf("bg x: %lu, y: %lu\n", (corners_arr->bg[i]).x,
    // (corners_arr->bg[i]).y);
    // }
    clean_corners_array(corners_arr);

    SDL_Surface *out =
      flatten_image(image_temp, corners_arr->hg, corners_arr->hd,
                    corners_arr->bg, corners_arr->bd);

    SDL_Surface *oute =
      flatten_image(image_tempe, corners_arr->hg, corners_arr->hd,
                    corners_arr->bg, corners_arr->bd);

    IMG_SavePNG(out, "test-done.png");

    IMG_SavePNG(image_temp, "./test6.png");
    SDL_UnlockSurface(image_temp);

    split_image(out, oute);

    free(arr->array);
    free(arr);

    free(corners_arr->bd);
    free(corners_arr->bg);
    free(corners_arr->hd);
    free(corners_arr->hg);
    free(corners_arr->hde);
    free(corners_arr->hge);
    free(corners_arr->bde);
    free(corners_arr->bge);
    free(corners_arr);
    SDL_FreeSurface(out);
    SDL_FreeSurface(oute);
    SDL_FreeSurface(image_temp);
    SDL_FreeSurface(image_tempe);
    SDL_FreeSurface(imagee);
    SDL_FreeSurface(imageee);

    return 0;
}
