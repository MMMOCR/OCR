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
    long sum = 0;
    size_t ratiox = (width *0.1);
    size_t ratioy = (height*0.1);
    size_t i, j;
    for (i = ratiox; i < width - ratiox; i++) {
        for (j = ratioy; j < height - ratioy; j++) {
            long pos = width / 20;
            int pixel = pixels[i * (width - pos) + j] >> 16 & 0xff;
            sum += pixel;
        }
    }
    long test = width - (width * 0.2);
    long test2 = height - (height * 0.2);
    /*printf("%d\n", sum);
    printf("%li\n", test);
    printf("%li\n", test2);
    printf("%li\n", test * test2);
    printf("%d %d\n", height, width);*/
    sum /= test * test2;
    printf("%d\n", sum);
    long max_average = 255 * 0.90;
    if (sum < max_average) return 0;
    return 1;
}

int* PicToList(SDL_Surface *image)
{
    int height = image->h;
    int width = image->w;
    Uint32* pixels = image->pixels;
    int len = width*height;
    int* list = calloc(len,sizeof(int));
    size_t ratiox = (width *0.1);
    size_t ratioy = (height*0.1);
    for(size_t i = ratiox; i<width - ratiox; i++)
    {
        for (size_t j = ratioy; j< height - ratioy; j++)
        {
            long pos = (width*0.2);
            int pixel = pixels[i * (width - pos) + j] >> 16 & 0xff;
            list[i*(width-pos)+j]= pixel;
        }
    }
    return list;
}
int
main(int argc, char** argv)
{
    SDL_Surface* surface = load_image(argv[1]);
    printf("%d\n", is_empty(surface));
    int* ptr = PicToList(surface);
    int *const end = ptr + 784;
    while( ptr < end ) {
        printf("%d\n", *ptr++);
    }
    return 1;
}
