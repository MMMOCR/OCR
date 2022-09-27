#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
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

static inline Uint8 *
pixel_ref(SDL_Surface *surf, unsigned x, unsigned y)
{
  int bpp = surf->format->BytesPerPixel;
  return (Uint8 *) surf->pixels + y * surf->pitch + x * bpp;
}

Uint32
get_pixel(SDL_Surface *surface, unsigned x, unsigned y)
{
  Uint8 *p = pixel_ref(surface, x, y);

  switch (surface->format->BytesPerPixel) {
    case 1:
      return *p;

    case 2:
      return *(Uint16 *) p;

    case 3:
      if (SDL_BYTEORDER == SDL_BIG_ENDIAN) return p[0] << 16 | p[1] << 8 | p[2];
      else
	return p[0] | p[1] << 8 | p[2] << 16;

    case 4:
      return *(Uint32 *) p;
  }

  return 0;
}

void
put_pixel(SDL_Surface *surface, unsigned x, unsigned y, Uint32 pixel)
{
  Uint8 *p = pixel_ref(surface, x, y);

  switch (surface->format->BytesPerPixel) {
    case 1:
      *p = pixel;
      break;

    case 2:
      *(Uint16 *) p = pixel;
      break;

    case 3:
      if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
	p[0] = (pixel >> 16) & 0xff;
	p[1] = (pixel >> 8) & 0xff;
	p[2] = pixel & 0xff;
      } else {
	p[0] = pixel & 0xff;
	p[1] = (pixel >> 8) & 0xff;
	p[2] = (pixel >> 16) & 0xff;
      }
      break;

    case 4:
      *(Uint32 *) p = pixel;
      break;
  }
}

void
fill_surface(SDL_Surface *surface, Uint32 pixel)
{
  size_t surface_w = surface->w;
  size_t surface_h = surface->h;

  for (size_t i = 0; i < surface_w; i++) {
    for (size_t j = 0; j < surface_h; j++) {
      put_pixel(surface, i, j, pixel);
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
  char *SAVED_IMG_NAME_R = name;
  long int x = 0;
  long int y = 0;
  int image_w = image->w;
  int image_h = image->h;
  double half_image_w = image_w / 2;
  double half_image_h = image_h / 2;
  size_t max_edge;

  angle = angle * 2 * PI / 360;

  if (fmod(angle, (double) PI / 2) == 0) {
    if (image_w > image_h) max_edge = image_w + 1;
    else
      max_edge = image_h + 1;
  } else {
    max_edge = (int) sqrt(image_w * image_w + image_h * image_h) + 1;
  }

  rotated_surface = SDL_CreateRGBSurface(0, max_edge, max_edge, 32, 0, 0, 0, 0);

  fill_surface(rotated_surface, SDL_MapRGB(rotated_surface->format, 0, 0, 0));

  if (fmod(angle, (double) PI / 2) == 0) {
    for (size_t i = 0; i < max_edge; i++) {
      for (size_t j = 0; j < max_edge; j++) {
	x = round((j - half_image_w) * cos(angle) -
	          (i - half_image_h) * sin(angle)) +
	  half_image_w;
	y = round((j - half_image_w) * sin(angle) +
	          (i - half_image_h) * cos(angle)) +
	  half_image_h;
	if (x < 0 || x > image_w) { continue; }
	if (y < 0 || y > image_h) { continue; }
	pixel = get_pixel(image, x, y);
	SDL_GetRGB(pixel, image->format, &r, &g, &b);
	pixel = SDL_MapRGB(rotated_surface->format, r, g, b);
	put_pixel(rotated_surface, j, i, pixel);
      }
    }
  } else {
    for (size_t i = 0; i < max_edge + (max_edge - image_h) / 2; i++) {
      for (size_t j = 0; j < max_edge + (max_edge - image_w) / 2; j++) {
	x = round((j - half_image_w) * cos(angle) -
	          (i - half_image_h) * sin(angle)) +
	  half_image_w;
	y = round((j - half_image_w) * sin(angle) +
	          (i - half_image_h) * cos(angle)) +
	  half_image_h;
	if (x < 0 || x > image_w) { continue; }
	if (y < 0 || y > image_h) { continue; }
	pixel = get_pixel(image, x, y);
	SDL_GetRGB(pixel, image->format, &r, &g, &b);
	pixel = SDL_MapRGB(rotated_surface->format, r, g, b);
	put_pixel(rotated_surface, j + (max_edge - image_w) / 2,
	          i + (max_edge - image_h) / 2, pixel);
      }
    }
  }

  SDL_SaveBMP(rotated_surface, SAVED_IMG_NAME_R);
  return rotated_surface;
}
