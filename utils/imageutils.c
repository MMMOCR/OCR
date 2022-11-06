#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <err.h>
#include <stdint.h>
#include <stdio.h>

int
treshold(long int len, Uint32* pixels, int override_treshold)
{
    // TODO implement gaussian noise reduction
    int treshold = 0;
    int var_max = 0;
    unsigned histogram[256];
    int pbg = 0;
    int pfg = 0;
    float wbg = 0;
    float wfg = 0;
    int sumbg = 0;
    int sumfg = 0;
    float meanbg = 0;
    float meanfg = 0;
    float sumvarbg = 0;
    float sumvarfg = 0;
    float varbg = 0;
    float varfg = 0;
    float var = 0;
    float vars[256];
    Uint8 val = 0;

    for (size_t i = 0; i <= 255; i++)
        histogram[i] = 0;

    if (override_treshold != 0) {
        treshold = override_treshold;
    } else {
        for (int i = 0; i < len; i++) {
            int pixel = pixels[i];
            val = pixel >> 16 & 0xff;
            histogram[val]++;
        }
        for (size_t i = 0; i <= 255; i++) {
            for (size_t j = 0; j <= 255; j++) {
                if (j < i) {
                    pbg += histogram[j];
                    if (histogram[j] != 0) sumbg += j * histogram[j];
                } else {
                    pfg += histogram[j];
                    if (histogram[j] != 0) sumfg += j * histogram[j];
                }
            }
            if (pbg == 0 || pfg == 0) {
                vars[i] = 0;
                continue;
            }
            wbg = (float) pbg / (float) len;
            wfg = (float) pfg / (float) len;
            meanbg = (float) sumbg / (float) pbg;
            meanfg = (float) sumfg / (float) pfg;
            for (size_t j = 0; j <= 255; j++) {
                if (j < i) {
                    if (histogram[j] != 0)
                        sumvarbg += histogram[j] * (float) (j - meanbg) *
                          (float) (j - meanbg);
                } else {
                    if (histogram[j] != 0)
                        sumvarfg += histogram[j] * (float) (j - meanfg) *
                          (float) (j - meanfg);
                }
            }
            varbg = (float) sumvarbg / (float) pbg;
            varfg = (float) sumvarfg / (float) pfg;
            var = wbg * (float) varbg + wfg * (float) varfg;
            vars[i] = var;
            pbg = 0;
            pfg = 0;
            sumbg = 0;
            sumfg = 0;
            sumvarbg = 0;
            sumvarfg = 0;
        }
        var_max = vars[22];
        for (size_t i = 1; i <= 254; i++) {
            if (vars[i] < var_max && vars[i] != 0) {
                treshold = i;
                var_max = vars[i];
            }
        }
    }
    // printf("treshold: %d\n", treshold);
    return treshold;
}

void
draw(SDL_Renderer* renderer, SDL_Texture* texture)
{
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
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

    int threshold = treshold(colored_surface->w * colored_surface->h,
                             colored_surface->pixels, 0);

    back_to_black(colored_surface, threshold);

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

    if (SDL_Init(SDL_INIT_VIDEO) != 0) errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Window* window =
      SDL_CreateWindow("Dynamic Fractal Canopy", 0, 0, 640, 400,
                       SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window == NULL) errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Renderer* renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Surface* colored_surface = load_image(argv[1]);

    h = colored_surface->h;
    w = colored_surface->w;
    SDL_SetWindowSize(window, w, h);

    SDL_Texture* texture =
      SDL_CreateTextureFromSurface(renderer, colored_surface);

    surface_to_grayscale(colored_surface);

    int threshold = treshold(colored_surface->w * colored_surface->h,
                             colored_surface->pixels, 0);

    back_to_black(colored_surface, threshold);

    SDL_Texture* grayscale_texture =
      SDL_CreateTextureFromSurface(renderer, colored_surface);

    SDL_FreeSurface(colored_surface);

    event_loop(renderer, texture, grayscale_texture);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}
