#include "linesdetection.h"

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

    double moy = 0;

    for (size_t i = 0; i < (size_t) arr->len; i += 2) {
	moy += DIFF(arr->array[i + 1], 90);
    }

    moy /= (double) counter / 2;

    printf("%f\n", moy);

    surface = SDL_CreateRGBSurfaceFrom(
      (void *) pixels, w, h, 32, format->BytesPerPixel * w, format->Rmask,
      format->Gmask, format->Bmask, format->Amask);
    surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ARGB8888, 0);
    if (moy != 0) { surface = rotate_image(surface, -moy); }

    //    long int last_w = w;
    //    long int last_h = h;
    //    diag = DIAG(w, h);
    //    pixels = surface->pixels;
    //    w = surface->w;
    //    h = surface->h;
    //
    //    double ratio = (double)DIAG(w, h) / (double)DIAG(last_w, last_h);
    //    double trigo_angle, temp2;
    //
    //
    //    long int y = (double)last_w /2 + cos (moy * PI / 180) * (-
    //    (double)last_w / 2) + sin(moy * PI /180) * (- (double)last_h / 2) +
    //    (double)(w - last_w) /2; long int x = (double)last_h /2 - sin (moy *
    //    PI / 180) * (- (double)last_w / 2) + cos(moy * PI /180) * (-
    //    (double)last_h / 2) + (double)(h - last_h) /2;
    //
    //    // printf("bbbbbbbbbb %li, %li, %f\n", x, y, ratio);
    //
    //    // printf("%i, %i\n", (int) moy % 90, (int) (moy +1) %90);
    //
    //    y -= (((int)moy % 90 < 1 && (int)moy % 90 > -1)) ? 0 : 15;
    //    x += (((int)moy % 90 < 1 && (int)moy % 90 > -1)) ? 0 : 15;
    //
    //    // printf("bbbbbbbbbb %li, %li, %f, %i\n", x, y, ratio, (int)moy);
    //
    //    for (size_t i = 0; i < (size_t) arr->len; i += 2) {
    //        // printf("%li, %f, %li\n", arr->array[i+1],
    //        cos(sin(arr->array[i+1] * PI / 180)), arr->array[i]); trigo_angle
    //        = TRIGO(arr->array[i+1] * PI / 180, arr->array[i+1]); trigo_angle
    //        = ABS(trigo_angle);
    //        // printf("%i\n", (int)arr->array[i+1] % 180);
    //        if (ORIENT((int)arr->array[i+1], moy)) {
    //            // printf("aaaaaaa: %li, %li, %f, %f\n", arr->array[i+1], w,
    //            (double)last_w/ trigo_angle, trigo_angle); temp2 = x; //
    //            ((double)w - (double) last_w / temp); // + 100;
    //        }
    //        else {
    //            temp2 = y; //((double)h - (double) last_h /temp); // - 90;
    //        }
    //
    //
    //        // printf("angle: %li, temp2: %f\n", arr->array[i+1], temp2);
    //
    //
    //
    //        // printf("%li, decal: %f\n", arr->array[i], temp2 );
    //        arr->array[i] = (double)arr->array[i] + temp2; // / temp/2 ;
    //    	arr->array[i + 1] -= (long int)moy;
    //    }
    //
    //    for (size_t i = 0; i < arr->len; i+=2) {
    // 	m = sin(arr->array[i+1] * PI / 180);
    // 	n = cos(arr->array[i+1] * PI / 180);
    // 	x0 = m * arr->array[i];
    // 	y0 = n * arr->array[i];
    // 	x1 = x0 + w * (-n);
    // 	y1 = y0 + h * (m);
    // 	x2 = x0 - w * (-n);
    // 	y2 = y0 - h * (m);
    //        draw_line(pixels, w, h, x1, y1, x2, y2,
    // 	          SDL_MapRGB(format, 0, 255, 0));
    // }

    IMG_SavePNG(surface, "./test3.png");

    free(mat);

    // printf("%lu\n", arr->len);

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

    arr->array = realloc(arr->array, sizeof(long int) * counter);
    if (!arr->array) { return NULL; }
    arr->len = counter;

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
    char flag = 0;
    Sorted_Points_Array *sorted_arr = calloc(1, sizeof(Sorted_Points_Array));
    sorted_arr->horizontal = calloc(arr->len, sizeof(long int));
    sorted_arr->vertical = calloc(arr->len, sizeof(long int));
    for (size_t i = 0; i < arr->len; i += 2) {
	if (ISVERT(arr->array[i + 1])) {
        for (size_t j = 0; j < sorted_arr->count_v; j+=2) {
            if (MAXDIFF(arr->array[i], sorted_arr->vertical[j], 8)){
                flag = 1;
            }
        }
        if (!flag){
	        sorted_arr->vertical[sorted_arr->count_v++] = arr->array[i];
    	    sorted_arr->vertical[sorted_arr->count_v++] = arr->array[i + 1];
        }
	} else if (ISHOR(arr->array[i + 1])) {
        for (size_t j = 0; j < sorted_arr->count_h; j+=2) {
            if (MAXDIFF(arr->array[i], sorted_arr->horizontal[j], 8)){
                flag = 1;
            }
        }
        if (!flag){
	        sorted_arr->horizontal[sorted_arr->count_h++] = arr->array[i];
    	    sorted_arr->horizontal[sorted_arr->count_h++] = arr->array[i + 1];
        }
	}
    }

    sorted_arr->vertical =
      realloc(sorted_arr->vertical, sizeof(long int) * sorted_arr->count_v);
    if (!sorted_arr->vertical) {
	return NULL;

	sorted_arr->horizontal = realloc(
	  sorted_arr->horizontal, sizeof(long int) * sorted_arr->count_h);
	if (!sorted_arr->horizontal) {}
	return NULL;
    }

    /*
    for (size_t i = 0; i < sorted_arr->count_h; i += 2) {
        printf("%li, %li\n", sorted_arr->horizontal[i],
               sorted_arr->horizontal[i + 1]);
    }

    printf("pute\n");

    for (size_t i = 0; i < sorted_arr->count_v; i += 2) {
        printf("%li, %li\n", sorted_arr->vertical[i],
               sorted_arr->vertical[i + 1]);
    }

    */
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
    		    intersect_arr->array[counter++] = x;
	    	    intersect_arr->array[counter++] = y;
            }
	    }
	}
    }

    intersect_arr->len = counter;
    intersect_arr->array =
      realloc(intersect_arr->array, sizeof(long int) * counter);
    if (!intersect_arr->array) { return NULL; }

    return intersect_arr;
}

int
main(int argc, char **argv)
{
    Points_Array *arr;
    Points_Array *intersect_arr;
    Sorted_Points_Array *sorted_arr;

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

    for (size_t i = 0; i < arr->len; i += 2) {
	printf("%li, %li\n", arr->array[i], arr->array[i + 1]);
    }

    printf("puuuuuuuute\n");

    sorted_arr = sort_array(arr);
    if (!sorted_arr) { return 1; }

    SDL_LockSurface(image_temp);

    /*
        float m, n;
        int x0, y0, x1, y1, x2, y2;
        pixels = image_temp->pixels;

        for (size_t i = 0; i < sorted_arr->count_h; i += 2) {
        printf("r: %lu,theta: %lu\n", sorted_arr->horizontal[i],
       sorted_arr->horizontal[i+1]); m = sin(sorted_arr->horizontal[i + 1] * PI
       / 180); n = cos(sorted_arr->horizontal[i + 1] * PI / 180); x0 = m *
       sorted_arr->horizontal[i]; y0 = n * sorted_arr->horizontal[i]; x1 = x0 +
       2 * image_temp->w * (-n); y1 = y0 + 2 * image_temp->h * (m); x2 = x0 - 2
       * image_temp->w * (-n); y2 = y0 - 2 * image_temp->h * (m);
            draw_line(pixels, image_temp->w, image_temp->h, x1, y1, x2, y2,
                      SDL_MapRGB(image_temp->format, 0, 255, 0));
        }

        for (size_t i = 0; i < sorted_arr->count_v; i += 2) {
            m = sin(sorted_arr->vertical[i + 1] * PI / 180);
            n = cos(sorted_arr->vertical[i + 1] * PI / 180);
            x0 = m * sorted_arr->vertical[i];
            y0 = n * sorted_arr->vertical[i];
            x1 = x0 + 2 * image_temp->w * (-n);
            y1 = y0 + 2 * image_temp->h * (m);
            x2 = x0 - 2 * image_temp->w * (-n);
            y2 = y0 - 2 * image_temp->h * (m);
            draw_line(pixels, image_temp->w, image_temp->h, x1, y1, x2, y2,
                      SDL_MapRGB(image_temp->format, 255, 0, 0));
        }
    */

    intersect_arr =
      get_intersection_points(sorted_arr, image_temp->w, image_temp->h);
    if (!intersect_arr) { return 1; }

    for (size_t i = 0; i < intersect_arr->len; i += 2) {
	for (int j = -2; j <= 2; j++) {
	    for (int k = -2; k <= 2; k++) {
		change_pixel(image_temp, intersect_arr->array[i],
		             intersect_arr->array[i + 1],
		             SDL_MapRGB(image_temp->format, 255, 255, 0));
	    }
	}
    }

    IMG_SavePNG(image_temp, "./test6.png");
    SDL_UnlockSurface(image_temp);

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
