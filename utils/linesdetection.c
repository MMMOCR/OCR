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

long int *
detect_lines(int *pixels, long int w, long int h, SDL_PixelFormat *format)
{
    long unsigned int diag;
    long unsigned int *mat;
    long int k;
    long unsigned int max_size;
    long int x0, y0, x1, y1, x2, y2;
    float m, n;
    Uint8 r, g, b;
    SDL_Surface *surface;

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
		    if (ABS(k) < (long int) max_size) {
			mat[k * ANGLE + theta]++;
		    }
		}
	    }
	}
    }

    int jj = 0;
    long int *kk = calloc(800, sizeof(long int));

    for (size_t i = 1; i < max_size; i++) {
	for (size_t j = 0; j < ANGLE; j++) {
	    if (mat[i * ANGLE + j] > 400) {
		m = sin(j * PI / 180);
		n = cos(j * PI / 180);
		x0 = m * i;
		y0 = n * i;
		x1 = x0 + w * (-n);
		y1 = y0 + h * (m);
		x2 = x0 - w * (-n);
		y2 = y0 - h * (m);
		draw_line(pixels, w, h, x1, y1, x2, y2,
		          SDL_MapRGB(format, 255, 0, 0));
		kk[jj++] = i;
		kk[jj++] = j;
	    }
	}
    }

    reallocf(kk, jj);

    double moy = 0;

    for (size_t i = 0; i < (size_t) jj; i += 2) {
	moy += kk[i + 1] % 90;
	printf("%li, %li\n", kk[i], kk[i + 1]);
    }

    moy /= jj / 2;

    printf("%f\n", moy);

    surface = SDL_CreateRGBSurfaceFrom(
      (void *) pixels, w, h, 32, format->BytesPerPixel * w, format->Rmask,
      format->Gmask, format->Bmask, format->Amask);
    surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ARGB8888, 0);
    surface = rotate_image(surface, -moy);
    IMG_SavePNG(surface, "./test3.png");
    SDL_FreeSurface(surface);

    free(mat);

    printf("%i\n", jj);

    return 0;
}

int
main(int argc, char **argv)
{
    if (argc != 2) {
	printf("Usage: %s path_to_image\n", argv[0]);
	return 1;
    }

    SDL_Surface *image_temp = IMG_Load(argv[1]);

    image_temp =
      SDL_ConvertSurfaceFormat(image_temp, SDL_PIXELFORMAT_RGB888, 0);

    detect_lines(image_temp->pixels, image_temp->w, image_temp->h,
                 image_temp->format);

    SDL_FreeSurface(image_temp);

    return 0;
}
