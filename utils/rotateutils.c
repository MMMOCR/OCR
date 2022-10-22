#include <SDL.h>
#include <SDL_image.h>
#include <SDL_pixels.h>
#include <SDL_render.h>
#include <SDL_surface.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PI 3.14159265358979323846

SDL_Surface *
load_image(char file[])
{
    SDL_Surface *image = IMG_Load(file);
    return image;
}

unsigned int
get_pixel(SDL_Surface *surface, size_t x, size_t y)
{
    unsigned int *pixels = surface->pixels;
    return pixels[y * surface->w + x];
}

void
change_pixel(SDL_Surface *surface, size_t x, size_t y, Uint32 pixel)
{
    unsigned int *pixels = surface->pixels;
    pixels[y * surface->w + x] = pixel;
}

void
fill_surface(SDL_Surface *surface, Uint32 pixel)
{
    size_t surface_w = surface->w;
    size_t surface_h = surface->h;

    for (size_t i = 0; i < surface_w; i++) {
	for (size_t j = 0; j < surface_h; j++) {
	    change_pixel(surface, i, j, pixel);
	}
    }
}

SDL_Surface *
rotate_image(SDL_Surface *image, double angle)
{
    SDL_Surface *rotated_surface = NULL;
    Uint32 pixel;
    Uint8 r, g, b;
    char name[] = "/tmp/fileXXXXXX";
    int fd = mkstemp(name);
    close(fd);
    long int x = 0;
    long int y = 0;
    int image_w = image->w;
    int image_h = image->h;
    double half_image_w = (double) image_w / 2;
    double half_image_h = (double) image_h / 2;
    size_t max_edge;

    image = SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_ABGR8888, 0);
    rotated_surface =
      SDL_ConvertSurfaceFormat(rotated_surface, SDL_PIXELFORMAT_ABGR8888, 0);
    SDL_LockSurface(image);
    angle = angle * 2 * PI / 360;

    max_edge = (int) sqrt(image_w * image_w + image_h * image_h) + 1;

    rotated_surface =
      SDL_CreateRGBSurface(0, max_edge, max_edge, 32, 0, 0, 0, 0);

    fill_surface(rotated_surface,
                 SDL_MapRGB(rotated_surface->format, 255, 255, 255));

    for (size_t i = 0; i < max_edge + (max_edge - image_h) / 2; i++) {
	for (size_t j = 0; j < max_edge + (max_edge - image_w) / 2; j++) {
	    x = round((j - half_image_w - (double) (max_edge - image_w) / 2) *
	                cos(angle) -
	              (i - half_image_h - (double) (max_edge - image_h) / 2) *
	                sin(angle)) +
	      half_image_w;
	    y = round((j - half_image_w - (double) (max_edge - image_w) / 2) *
	                sin(angle) +
	              (i - half_image_h - (double) (max_edge - image_h) / 2) *
	                cos(angle)) +
	      half_image_h;
	    if (x < 0 || x >= image_w) { continue; }
	    if (y < 0 || y >= image_h) { continue; }
	    if (x == 0 && y == 0) {
		printf("COOOORRRRRDDDSSS : %li, %li\n", j, i);
	    }
	    pixel = get_pixel(image, x, y);
	    SDL_GetRGB(pixel, image->format, &r, &g, &b);
	    pixel = SDL_MapRGB(rotated_surface->format, r, g, b);
	    change_pixel(rotated_surface, j, i, pixel);
	}
    }

    SDL_UnlockSurface(image);

    return rotated_surface;
}
