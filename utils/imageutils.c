#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <stdint.h>
#include <stdio.h>

SDL_Surface *
load_image(char file[])
{
  SDL_Surface *image = IMG_Load(file);
  return image;
}

void
rotate_image(SDL_Surface *image, double angle)
{
  int ended = 0;
  unsigned int *pixels = 0;
  unsigned int pixel = 0;
  unsigned int r = 0;
  unsigned int g = 0;
  unsigned int b = 0;
  unsigned int new = 0;
  SDL_Event event;

  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window *window =
    SDL_CreateWindow("SDL2 Displaying Image", SDL_WINDOWPOS_UNDEFINED,
                     SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
  SDL_Texture *texture =
    SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888,
                      SDL_TEXTUREACCESS_STATIC, image->w, image->h);
  SDL_RendererFlip flip = SDL_FLIP_NONE;
  image = SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_ABGR8888, 0);
  while (!ended) {
    SDL_UpdateTexture(texture, NULL, image->pixels,
                      image->w * sizeof(unsigned int));
    SDL_WaitEvent(&event);

    pixels = image->pixels;
    for (int i = 0; i < image->h; i++) {
      for (int j = 0; j < image->w; j++) {
	pixel = pixels[i * image->w + j];
	r = pixel >> 16 & 0xff;
	g = pixel >> 16 & 0xff;
	b = pixel >> 16 & 0xff;
	new = 0.212671f * r + 0.715160f * g + 0.072169f * b;
	pixel = (0xff << 24) | (new << 16) | (new << 8) | new;
	pixels[i * image->w + j] = pixel;
      }
    }
    switch (event.type) {
      case SDL_QUIT:
	ended = 1;
	break;
    }
    SDL_RenderCopyEx(renderer, texture, NULL, NULL, angle, NULL, flip);
    SDL_RenderPresent(renderer);
  }
  SDL_RenderCopyEx(renderer, texture, NULL, NULL, angle, NULL, flip);
  SDL_RenderPresent(renderer);
  SDL_DestroyTexture(texture);
  SDL_FreeSurface(image);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

int
main(int argc, char *argv[])
{
  if (argc != 2) {
    printf("Usage: %s path_to_image\n", argv[0]);
    return 1;
  }

  SDL_Surface *image = load_image(argv[1]);
  rotate_image(image, 45.0);
  return 0;
}
