/*
 *
- load image
- reduce
 h = colored_surface->h;`     w = colored_surface->w;
 for i = 10; i < h - 10; i++
        for j = 10; j< w; j++

 * */

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <err.h>
#include <stdint.h>
#include <stdio.h>

SDL_Surface*
load_image(const char* path)
{
    SDL_Surface* image = IMG_Load(path);
    if (image == NULL) errx(EXIT_FAILURE, "%s", SDL_GetError());
    SDL_Surface* format =
      SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_RGB888, 0);
    SDL_FreeSurface(image);
    return format;
}

int
is_empty(SDL_Surface* image)
{
    int height = image->h;
    int width = image->w;
    Uint32* pixels = image->pixels;
    printf("%d %d\n", height, width);
    long sum = 0;
    printf("%d\n", sum);
    size_t i, j;
    for (i = (width * 0.1); i < width - (width * 0.1); i++) {
        for (j = (height * 0.1); j < height - (height * 0.1); j++) {
            long pos = width / 20;
            int pixel = pixels[i * (width - pos) + j] >> 16 & 0xff0;
            sum += pixel;
        }
    }
    printf("%d\n", sum);
    long test = width - (width * 0.2);
    long test2 = height - (height * 0.2);
    printf("%li\n", test);
    printf("%li\n", test2);
    printf("%li\n", test * test2);
    sum /= test * test2;
    printf("%d\n", sum);
    long max_average = 255 * 0.85;
    if (sum < max_average) return 0;
    return 1;
}

int
main(int argc, char** argv)
{
    SDL_Surface* surface = load_image(argv[1]);
    printf("%d\n", is_empty(surface));
    return 1;
}
