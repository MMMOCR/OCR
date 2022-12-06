#include "linesdetection.h"

#include "SDL_rect.h"
#include "rotateutils.h"

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
restrict_array2(Points_Array *arr, size_t w, size_t h, size_t *mat)
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
            size_t diffangle2 =
              ABS((180 -
                   ABS((long int) (180 -
                                   ((180 + arr->array[k + 1]) % 180 -
                                    (180 + arr->array[i + 1]) % 180)))));
            printf("diff %lu diffangle %lu, diffangle2 %lu\n", diff, diffangle,
                   diffangle2);

            if (diff < 0.03 * h && diffangle <= 4) {
                if (mat[save_i * ANGLE + save_j] >
                    mat[save_k * ANGLE + save_l]) {
                    printf("replace %lu with %lu, replace %lu with %lu\n",
                           arr->array[k], arr->array[i], arr->array[k + 1],
                           arr->array[i + 1]);
                    arr->array[i] = arr->array[i];
                    arr->array[i + 1] = arr->array[i + 1];
                    arr->array[k] = 0;
                    arr->array[k + 1] = 0;
                } else {
                    printf("replace %lu with %lu, replace %lu with %lu\n",
                           arr->array[i], arr->array[k], arr->array[i + 1],
                           arr->array[k + 1]);
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
restrict_array(Points_Array *arr, size_t w, size_t h, size_t *mat)
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
    long int x0, y0, x1, y1, x2, y2;
    size_t counter = 0;
    float m, n;
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

    size_t max = 0;

    for (size_t i = 1; i < max_size; i++) {
        for (size_t j = 0; j < ANGLE; j++) {
            if (mat[i * ANGLE + j] > (unsigned long int) (w + h) / 7) {
                m = sin(j * PI / 180);
                n = cos(j * PI / 180);
                x0 = m * i;
                y0 = n * i;
                x1 = x0 + 2 * w * (-n);
                y1 = y0 + 2 * h * (m);
                x2 = x0 - 2 * w * (-n);
                y2 = y0 - 2 * h * (m);
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

    restrict_array(arr, w, h, mat);
    clean_array(arr);

    // float m, n;
    // int x0, y0, x1, y1, x2, y2;
    //
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
        printf("%i, %i\n", arr->array[i + 1], ANGLEF(arr->array[i + 1]));
        moy += ANGLEF(arr->array[i + 1]);
    }

    moy /= (double) arr->len / 2;

    printf("%f\n", moy);

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
    long int x0, y0, x1, y1, x2, y2;
    size_t counter = 0;
    float m, n;
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

    size_t max = 0;

    for (size_t i = 1; i < max_size; i++) {
        for (size_t j = 0; j < ANGLE; j++) {
            if (mat[i * ANGLE + j] > (unsigned long int) (w + h) / 7) {
                m = sin(j * PI / 180);
                n = cos(j * PI / 180);
                x0 = m * i;
                y0 = n * i;
                x1 = x0 + 2 * w * (-n);
                y1 = y0 + 2 * h * (m);
                x2 = x0 - 2 * w * (-n);
                y2 = y0 - 2 * h * (m);
                // draw_line(pixels, w, h, x1, y1, x2, y2,
                //     SDL_MapRGB(format, 0, 255, 0));
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

    for (size_t i = 0; i < counter; i += 2) {
        printf("r: %lu,theta: %lu\n", arr->array[i], arr->array[i + 1]);
    }

    printf("##########################################################\n");

    arr->array = realloc(arr->array, sizeof(long int) * counter);
    if (!arr->array) { return NULL; }
    arr->len = counter;

    restrict_array2(arr, w, h, mat);
    clean_array(arr);

    // float m, n;
    // int x0, y0, x1, y1, x2, y2;
    //
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

    end_surface = SDL_CreateRGBSurfaceFrom(
      (void *) pixels, w, h, 32, format->BytesPerPixel * w, format->Rmask,
      format->Gmask, format->Bmask, format->Amask);

    IMG_SavePNG(end_surface, "./test5.png");
    SDL_FreeSurface(end_surface);
    SDL_UnlockSurface(surface);
    // SDL_FreeSurface(surface);

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
                if (MAXDIFF(y1, y0, 20)) {
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
                if (MAXDIFF(x1, x0, 20)) {
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

Points_Array *
get_intersection_points(Sorted_Points_Array *array, long int w, long int h)
{
    Points_Array *intersect_arr = calloc(1, sizeof(Points_Array));
    intersect_arr->array = calloc(w * h, sizeof(long int));
    long int x, y;
    size_t counter = 0;

    for (size_t i = 0; i < array->count_h; i += 2) {
        for (size_t j = 0; j < array->count_v; j += 2) {
            if (parametricIntersect(
                  array->horizontal[i], array->horizontal[i + 1],
                  array->vertical[j], array->vertical[j + 1], &x, &y)) {
                if (x >= 0 && x < w && y >= 0 && y < h) {
                    printf("(%lu, %lu)\n", x, y);
                    intersect_arr->array[counter++] = x;
                    intersect_arr->array[counter++] = y;
                }
            }
        }
    }

    printf("%lu\n", counter / 2);

    intersect_arr->len = counter;
    intersect_arr->array =
      realloc(intersect_arr->array, sizeof(long int) * counter);
    if (!intersect_arr->array) { return NULL; }

    return intersect_arr;
}

void
split_image(SDL_Surface *image, Points_Array *intersect_arr)
{
    SDL_Surface *surf;
    long unsigned int x1, y1, x2, y3;
    int *pixels = image->pixels;
    int *pixels_2;
    char path[12] = { '.', '/', 'c', '_', 0, '_', 0, '.', 'p', 'n', 'g', 0 };
    for (size_t i = 0; i < 9; i++) {
        for (size_t j = 0; j < 18; j += 2) {
            x1 = intersect_arr->array[i * 20 + j + 1];
            y1 = intersect_arr->array[i * 20 + j];
            x2 = intersect_arr->array[i * 20 + j + 2 + 1];
            y3 = intersect_arr->array[(i + 1) * 20 + j];
            surf = SDL_CreateRGBSurfaceWithFormat(0, x2 - x1, y3 - y1, 32,
                                                  image->format->format);
            pixels_2 = surf->pixels;
            for (size_t k = y1; k < y3; k++) {
                for (size_t l = x1; l < x2; l++) {
                    pixels_2[(k - y1) * surf->w + (l - x1)] =
                      pixels[k * image->w + l];
                }
            }
            path[4] = i + 0x30;
            path[6] = j / 2 + 0x30;
            IMG_SavePNG(surf, path);
            SDL_UnlockSurface(surf);
            SDL_FreeSurface(surf);
        }
    }
}

// TODO: distance au premier point d'intersection
Sorted_Points_Array *
yeet_the_fools(Sorted_Points_Array *pa)
{
    long int *grossepute = calloc(pa->count_h - 1, sizeof(size_t));
    size_t *grosseputa = calloc(pa->count_h - 10, sizeof(size_t));
    size_t max = 0, moy = 0, min = -1, maxi, mini;
    for (size_t i = 0; i < pa->count_h - 1; i += 2) {
        printf("%lu\n", pa->horizontal[i]);
        grossepute[i] = pa->horizontal[i + 2] - pa->horizontal[i];
    }
    for (size_t i = 0; i < pa->count_h - 1; i++) {
        moy += grossepute[i];
    }
    moy /= pa->count_h - 1;
    for (size_t i = 0; i < pa->count_h - 1; i++) {
        grossepute[i] = ABS(moy - grossepute[i]);
    }
}

Matrix *
init_mat(size_t row)
{
    Matrix *mat = calloc(1, sizeof(Matrix));
    mat->row = row;
    mat->column = row;
    mat->data = calloc(row * row, sizeof(size_t));
}

float
compute_minor(Matrix *matrix, size_t i, size_t j)
{
    if (i < 0 || i >= matrix->row || j < 0 || j >= matrix->column) { return 0; }

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

    Matrix *minor = init_mat(matrix->row - 1);
    for (size_t k = 0; k < matrix->row; k++) {
        if (i == k) { continue; }
        for (size_t l = 0; l < matrix->column; l++) {
            if (j == l) { continue; }
            minor->data[(k < i ? k : k - 1) * minor->row + l < j ? l : l - 1] =
              matrix->data[k * matrix->row + l];
        }
    }

    float k = compute_determinant(minor);
    free(minor);
    return k;
}

float
compute_determinant(Matrix *matrix)
{
    if (matrix->row != matrix->column) { return 0; }

    if (matrix->row == 1) { return matrix->data[0]; }

    if (matrix->row == 2) {
        return matrix->data[0] * matrix->data[3] -
          matrix->data[1] * matrix->data[2];
    }

    float det = 0;
    for (size_t i = 0; i < matrix->column; i++) {
        det += matrix->data[i] * compute_minor(matrix, 0, i);
    }

    return det;
}

Matrix *
inverse_mat(Matrix *matrix)
{
    float det = compute_determinant(matrix);

    if (det == 0) { return NULL; }

    Matrix *inverse = init_mat(matrix->row);

    for (size_t i = 0; i < matrix->row; i++) {
        for (size_t j = 0; j < matrix->column; j++) {
            inverse->data[i * (matrix->row - 1) + j] =
              compute_minor(matrix, i, j) / det;
        }
    }

    return inverse;
}

Matrix *
create_transformation_matrix(Point *in_top_left,
                             Point *in_top_right,
                             Point *in_bot_left)
{
    Matrix *transform = init_mat(3);

    transform->data[0] = in_top_left->x;
    transform->data[1] = in_top_left->y;
    transform->data[2] = 1;
    transform->data[3] = in_top_right->x;
    transform->data[4] = in_top_right->y;
    transform->data[5] = 1;
    transform->data[6] = in_bot_left->x;
    transform->data[7] = in_bot_left->y;
    transform->data[8] = 1;

    Matrix *inverse_transform = inverse_mat(transform);

    return inverse_transform;
}

Matrix *
compute_perspective_transform(Point *in_top_left,
                              Point *in_top_right,
                              Point *in_bot_left)
{
    Matrix *transform =
      create_transformation_matrix(in_top_left, in_top_right, in_bot_left);

    Matrix *inverse = inverse_mat(transform);

    free(transform);

    return inverse;
}

SDL_Surface *
flatten_image(SDL_Surface *image,
              Point *top_left,
              Point *top_right,
              Point *bot_left,
              Point *bot_right)
{
    size_t w = top_right->x - top_left->x;
    size_t h = bot_left->y - top_left->y;

    SDL_Surface *out = SDL_CreateRGBSurface(
      0, w, h, image->format->BytesPerPixel * w, image->format->Rmask,
      image->format->Gmask, image->format->Bmask, image->format->Amask);

    Matrix *transform =
      compute_perspective_transform(top_left, top_right, bot_left);

    int *pixels = image->pixels;
    int *new_pixels = out->pixels;

    size_t new_x, new_y;
    for (size_t i = 0; i < image->w; i++) {
        for (size_t j = 0; j < image->h; j++) {
            new_x = i * transform->data[0] + j * transform->data[3] +
              transform->data[6];
            new_y = i * transform->data[1] + j * transform->data[4] +
              transform->data[7];
            if (new_x >= 0 && new_x < w && new_y >= 0 && new_y < h) {
                new_pixels[new_x * w + new_y] = pixels[i * w + j];
            }
        }
    }
}

int
main(int argc, char **argv)
{

    SDL_Surface *image_test = IMG_Load(argv[1]);
    Point top_left = { 892, 364 };
    Point top_right = { 1038, 887 };
    Point bot_left = { 233, 547 };
    Point bot_right = { 364, 1073 };

    SDL_Surface *out =
      flatten_image(image_test, &top_left, &top_right, &bot_left, &bot_right);

    IMG_SavePNG(image_test, "./flatten_test.png");

    Points_Array *arr;
    Points_Array *intersect_arr;
    Sorted_Points_Array *sorted_arr;
    Sorted_Points_Array *sorted_arr_2;

    if (argc != 2) {
        printf("Usage: %s <path_to_image>\n", argv[0]);
        return 1;
    }

    SDL_Surface *image_temp = IMG_Load(argv[1]);

    if (image_temp == NULL) {
        printf("Image %s is not valid\n", argv[1]);
        return 1;
    }

    image_temp =
      SDL_ConvertSurfaceFormat(image_temp, SDL_PIXELFORMAT_RGB888, 0);

    int *pixels = image_temp->pixels;

    image_temp = detect_lines_and_rotate(pixels, image_temp->w, image_temp->h,
                                         image_temp->format);

    arr = detect_lines(image_temp);

    sorted_arr = sort_array(arr);
    if (!sorted_arr) { return 1; }

    SDL_LockSurface(image_temp);

    float m, n;
    int x0, y0, x1, y1, x2, y2;
    pixels = image_temp->pixels;

    for (size_t i = 0; i < sorted_arr->count_h; i += 2) {
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

    sorted_arr_2 = yeet_the_fools(sorted_arr);

    IMG_SavePNG(image_temp, "./test9.png");

    intersect_arr =
      get_intersection_points(sorted_arr, image_temp->w, image_temp->h);
    if (!intersect_arr) { return 1; }

    for (size_t i = 0; i < intersect_arr->len; i += 2) {
        if (intersect_arr->array[i] == 0) { continue; }
        for (size_t j = i + 2; j < intersect_arr->len; j += 2) {
            if (intersect_arr->array[j] == 0) { continue; }
            if (MAXDIFF(intersect_arr->array[i], intersect_arr->array[j], 20)) {
                if (MAXDIFF(intersect_arr->array[i + 1],
                            intersect_arr->array[j + 1], 20)) {
                    intersect_arr->array[i] =
                      (intersect_arr->array[i] + intersect_arr->array[j]) / 2;
                    intersect_arr->array[i + 1] =
                      (intersect_arr->array[i + 1] +
                       intersect_arr->array[j + 1]) /
                      2;
                    intersect_arr->array[j] = 0;
                    intersect_arr->array[j + 1] = 0;
                }
            }
        }
    }

    size_t x, y;

    for (size_t i = 0; i < intersect_arr->len; i += 2) {
        for (int j = -2; j <= 2; j++) {
            for (int k = -2; k <= 2; k++) {
                // printf("%ld, %ld, %ld, %ld\n", intersect_arr->array[i] + j,
                // intersect_arr->array[i+ 1] + k, image_temp->w,
                // image_temp->h); change_pixel(image_temp,
                // intersect_arr->array[i] - ABS(j) >= 0 &&
                // intersect_arr->array[i] + ABS(j) < image_temp->h ?
                // intersect_arr->array[i] + j : intersect_arr->array[i],
                // intersect_arr->array[i+1] - ABS(k) >= 0 &&
                // intersect_arr->array[i+1] + ABS(k) < image_temp->w ?
                // intersect_arr->array[i+ 1] + k : intersect_arr->array[i+ 1],
                // SDL_MapRGB(image_temp->format, 0, 255, 0));
                change_pixel(image_temp, intersect_arr->array[i],
                             intersect_arr->array[i + 1],
                             SDL_MapRGB(image_temp->format, 255, 0, 0));
            }
        }
    }

    IMG_SavePNG(image_temp, "./test6.png");
    SDL_UnlockSurface(image_temp);

    clean_array(intersect_arr);
    if (!intersect_arr || !intersect_arr->array) {
        printf("Error in clean_array\n");
        return 1;
    }

    //    split_image(image_temp, intersect_arr);

    free(intersect_arr->array);
    free(intersect_arr);
    free(arr->array);
    free(arr);
    free(sorted_arr->vertical);
    free(sorted_arr->horizontal);
    free(sorted_arr);

    SDL_FreeSurface(image_temp);

    return 0;
}
