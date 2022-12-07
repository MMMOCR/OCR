#include "imageutils.h"

#include "gaussian_blur.h"
#include "otsu.h"
#include "sobel.h"

#include <SDL2/SDL_image.h>

void
draw(SDL_Renderer* renderer, SDL_Texture* texture)
{
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void
save_texture(const char* file_name,
             SDL_Renderer* renderer,
             SDL_Texture* texture)
{
    SDL_Texture* target = SDL_GetRenderTarget(renderer);
    SDL_SetRenderTarget(renderer, texture);
    int width, height;
    SDL_QueryTexture(texture, NULL, NULL, &width, &height);
    SDL_Surface* surface =
      SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    SDL_RenderReadPixels(renderer, NULL, surface->format->format,
                         surface->pixels, surface->pitch);
    IMG_SavePNG(surface, file_name);
    SDL_FreeSurface(surface);
    SDL_SetRenderTarget(renderer, target);
}

void
event_loop(SDL_Renderer* renderer, SDL_Texture* colored, SDL_Texture* grayscale)
{
    SDL_Event event;
    SDL_Texture* t = colored;
    int colorVersion = 1;

    while (1) {
        SDL_WaitEvent(&event);

        switch (event.type) {
            case SDL_QUIT:
                return;

            case SDL_WINDOWEVENT:
                if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                    if (!colorVersion) {
                        draw(renderer, grayscale);
                    } else {
                        draw(renderer, t);
                    }
                }
                break;
            case SDL_KEYDOWN:
                if (!colorVersion) {
                    draw(renderer, t);
                    colorVersion = 1;
                } else {
                    draw(renderer, grayscale);
                    colorVersion = 0;
                }
                break;
        }
    }
}

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

Uint32
pixel_to_grayscale(Uint32 pixel_color, SDL_PixelFormat* format)
{
    Uint8 r, g, b;
    SDL_GetRGB(pixel_color, format, &r, &g, &b);
    Uint32 average = 0.2989 * r + 0.587 * g + 0.114 * b;
    r = average;
    g = average;
    b = average;

    Uint32 color = SDL_MapRGB(format, r, g, b);
    return color;
}

void
surface_to_grayscale(SDL_Surface* surface)
{
    Uint32* pixels = surface->pixels;
    int len = surface->w * surface->h;
    SDL_PixelFormat* format = surface->format;
    SDL_LockSurface(surface);
    for (int i = 0; i < len; i++) {
        Uint32 average = pixel_to_grayscale(pixels[i], format);
        pixels[i] = average;
    }
    SDL_UnlockSurface(surface);
}

void
back_to_black(SDL_Surface* surface, int threshold)
{
    Uint32* pixels = surface->pixels;
    int len = surface->w * surface->h;
    SDL_PixelFormat* format = surface->format;
    SDL_LockSurface(surface);
    for (int i = 0; i < len; i++) {
        int val = pixels[i] >> 16 & 0xff;
        int new_val = 0;
        if (val > threshold) new_val = 255;
        pixels[i] = SDL_MapRGB(format, new_val, new_val, new_val);
    }
    SDL_UnlockSurface(surface);
}

void
image_utils(char* filename)
{
    int h;
    int w;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Window* window =
      SDL_CreateWindow("Dynamic Fractal Canopy", 0, 0, 640, 400,
                       SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window == NULL) errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Renderer* renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Surface* colored_surface = load_image(filename);

    h = colored_surface->h;
    w = colored_surface->w;
    SDL_SetWindowSize(window, w, h);

    SDL_Texture* texture =
      SDL_CreateTextureFromSurface(renderer, colored_surface);

    surface_to_grayscale(colored_surface);

    int threshold = otsu_treshold(colored_surface->w * colored_surface->h,
                             colored_surface->pixels, 0);

    back_to_black(colored_surface, threshold);
    printf("%d\n", threshold);

    SDL_Texture* grayscale_texture =
      SDL_CreateTextureFromSurface(renderer, colored_surface);

    SDL_FreeSurface(colored_surface);

    event_loop(renderer, texture, grayscale_texture);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int
main(int argc, char** argv)
{
    int h;
    int w;
    if (argc != 2) errx(EXIT_FAILURE, "Usage: image-file");
    SDL_Surface* colored_surface;
    colored_surface = load_image(argv[1]);

    if (SDL_Init(SDL_INIT_VIDEO) != 0) errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Window* window =
      SDL_CreateWindow("Dynamic Fractal Canopy", 0, 0, 640, 400,
                       SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window == NULL) errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Renderer* renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) errx(EXIT_FAILURE, "%s", SDL_GetError());

    h = colored_surface->h;
    w = colored_surface->w;
    SDL_SetWindowSize(window, w, h);

    // int treshold = otsu_treshold(colored_surface->w * colored_surface->h,
    // colored_surface->pixels, 0);
    // back_to_black(colored_surface, treshold);
    SDL_Texture* texture =
      SDL_CreateTextureFromSurface(renderer, colored_surface);

    surface_to_grayscale(colored_surface);

    // int treshold = otsu_treshold(colored_surface->w * colored_surface->h,
    // colored_surface->pixels, 0);

    // printf("%d\n",colored_surface->w);
    // multiple(colored_surface->w, colored_surface->h, colored_surface);

    // back_to_black(colored_surface, treshold);
    // to_black(colored_surface, 185, 0, 0, 708, 666);

    // Gaussian Blur
    double gauss[5][5];
    gaussian_kernel(gauss);
    SDL_Surface* out = colored_surface;
    compute(colored_surface, gauss, 0, out);

    // Compute edges with sobel
    edges(out);

    // multiple(out->w, out->h, out);

    // int threshold = otsu_treshold(colored_surface->w * colored_surface->h,
    // colored_surface->pixels, 0); back_to_black(colored_surface, threshold);
    // multiple(colored_surface->w, colored_surface->h, colored_surface);

    SDL_Texture* grayscale_texture =
      SDL_CreateTextureFromSurface(renderer, out);

    SDL_FreeSurface(colored_surface);

    event_loop(renderer, texture, grayscale_texture);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
