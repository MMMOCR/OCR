#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
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
  SDL_Event event;

  SDL_Init(SDL_INIT_VIDEO);

  SDL_Window *window =
    SDL_CreateWindow("SDL2 Displaying Image", SDL_WINDOWPOS_UNDEFINED,
                     SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, image);
  SDL_RendererFlip flip = SDL_FLIP_NONE;
  while (!ended) {
    SDL_WaitEvent(&event);

    switch (event.type) {
      case SDL_QUIT:
	ended = 1;
	break;
    }
    SDL_RenderCopyEx(renderer, texture, NULL, NULL, angle, NULL, flip);
    SDL_RenderPresent(renderer);
  }
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
