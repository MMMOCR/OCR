#include <math.h>
#include <stdio.h>
#include <stdlib.h>
// #include "imageutils.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_surface.h>
#include <err.h>
#include <stdint.h>
#include <stdio.h>

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

/*void
boxes_gauss(int boxes[], size_t sigma,size_t n)
{
    float wi = sqrt((12*sigma*sigma/n) + 1);
    int wl = floor(wi);
    if (wl%2==0) wl--;
    int wu = wl + 2;

    float mi = (12*sigma*sigma - n*wl*wl - 4*n*wl - 3*n)/(-4*wl - 4);
    int m = round(mi);

    for (size_t i = 0; i < n; i++) {
        boxes[i] = ((i < m ? wl : wu) - 1) / 2;
    }
}

void
box_blur(float *in, float *out, int w, int h, int r)
{

}

void
gauss_blur(float *&in, float *&out, int w, int h, int sigma)
{
    int boxes[3];
    boxes_gauss(boxes, sigma, 3);
}*/

// void
// filter(double ker[][5])
// {
//     double sigma = 1.0;
//     double r = 0.0;
//     double s = 2.0 * sigma *sigma;
//
//     double sum = 0.0;
//
//     for (size_t i = -2; i < 3; i++) {
//         for (size_t j = -2; j < 3; j++) {
//             r = sqrt(i*i + j*j);
//             ker[i+2][j+2] = (exp(-(r*r)/s)) / (M_PI * s);
//             sum += ker[i+2][j+2];
//         }
//     }
//
//     for (size_t i = 0; i < 5; i++) {
//         for (size_t j = 0; j < 5; j++) {
//             ker[i][j] /= sum;
//         }
//     }
// }

// int
// main (int argc, char *argv[])
// {
//     double ker[5][5];
//     filter(ker);
//
//     for (size_t i = 0; i < 5; i++) {
//         for (size_t j = 0; j < 5; j++) {
//             printf("%f \t", ker[i][j]);
//         }
//         printf("\n");
//     }
//     return 0;
// }
//
#define smooth_kernel_size 5
#define sigma 1 // 0.84089642
#define K 1

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
gaussian_kernel(double gauss[smooth_kernel_size][smooth_kernel_size])
{
    double sum = 0;
    int i, j;

    for (i = 0; i < smooth_kernel_size; i++) {
        for (j = 0; j < smooth_kernel_size; j++) {
            double x = i - (smooth_kernel_size - 1) / 2.0;
            double y = j - (smooth_kernel_size - 1) / 2.0;
            gauss[i][j] =
              K * exp(((pow(x, 2) + pow(y, 2)) / ((2 * pow(sigma, 2)))) * (-1));
            sum += gauss[i][j];
        }
    }
    for (i = 0; i < smooth_kernel_size; i++) {
        for (j = 0; j < smooth_kernel_size; j++) {
            gauss[i][j] /= sum;
        }
    }
    for (i = 0; i < smooth_kernel_size; i++) {
        for (j = 0; j < smooth_kernel_size; j++) {
            printf("%f ", gauss[i][j]);
        }
        printf("\n");
    }
}

SDL_Surface*
load(const char* path)
{
    SDL_Surface* image = IMG_Load(path);
    if (image == NULL) errx(EXIT_FAILURE, "%s", SDL_GetError());
    SDL_Surface* format =
      SDL_ConvertSurfaceFormat(image, SDL_PIXELFORMAT_RGB888, 0);
    SDL_FreeSurface(image);
    return format;
}

double
yes(Uint32* pixels,
    int i,
    double gauss[smooth_kernel_size][smooth_kernel_size],
    int j)
{
    double sum = 0;
    for (size_t k = 0; k < 5; k++) {
        sum += (pixels[i + k] >> 16 & 0xff) * gauss[j][k];
    }
    return sum;
}

void
compute(SDL_Surface* surface,
        double gauss[smooth_kernel_size][smooth_kernel_size],
        int i,
        SDL_Surface* out)
{
    int r, g, b;
    Uint32* pixels = surface->pixels;
    SDL_PixelFormat* format = surface->format;
    int w = surface->w;
    int h = surface->h;
    double sum = 0;

    for (size_t j = 0; i < 2; i++) {
        sum += yes(pixels, i - w - j, gauss, j);
    }
    // sum += yes(pixels,i-w-2,gauss,0);
    // sum += yes(pixels,i-w-1,gauss,1);
    sum += yes(pixels, i - 1, gauss, 2);
    // sum += yes(pixels,i+w-1,gauss,3);
    // sum += yes(pixels,i+w-2,gauss,4);
    for (size_t j = 0; j < 2; j++) {
        sum += yes(pixels, i + w - j, gauss, 2 + j);
    }
    // sum += (pixels[i-w-1] >> 16 & 0xff)*gauss[0][0] + (pixels[i-w] >> 16 &
    // 0xff)*gauss[0][1] + (pixels[i-w+1] >> 16 & 0xff)*gauss[0][2]; sum +=
    // (pixels[i-1] >> 16 & 0xff)*gauss[1][0] + (pixels[i] >> 16 &
    // 0xff)*gauss[1][1] + (pixels[i+1] >> 16 & 0xff)*gauss[1][2]; sum +=
    // (pixels[i+w-1] >> 16 & 0xff)*gauss[2][0] + (pixels[i+w] >> 16 &
    // 0xff)*gauss[2][1] + (pixels[i+w+1] >> 16 & 0xff)*gauss[2][2];
    // printf("%f\n",sum);
    r = sum;
    g = sum;
    b = sum;

    Uint32* pixelsOut = out->pixels;
    pixelsOut[i] = SDL_MapRGB(format, r, g, b);
}

void
test(SDL_Surface* surface, double gauss[smooth_kernel_size][smooth_kernel_size])
{
    SDL_Window* window =
      SDL_CreateWindow("Dynamic Fractal Canopy", 0, 0, 640, 400,
                       SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    if (window == NULL) errx(EXIT_FAILURE, "%s", SDL_GetError());

    SDL_Renderer* renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (renderer == NULL) errx(EXIT_FAILURE, "%s", SDL_GetError());
    SDL_Texture* bwtexture = SDL_CreateTextureFromSurface(renderer, surface);
    Uint32* pixels = surface->pixels;
    SDL_Surface* out = surface;
    int len = surface->w * surface->h;
    SDL_PixelFormat* format = surface->format;
    SDL_LockSurface(surface);
    for (int i = surface->w + 1; i < len - surface->h - 1; i++) {
        // printf("%d\n", val);
        // printf("%d\n",i);
        compute(surface, gauss, i, out);
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, out);
    event_loop(renderer, bwtexture, texture);

    SDL_UnlockSurface(surface);
}

int
main()
{
    double gauss[smooth_kernel_size][smooth_kernel_size];
    gaussian_kernel(gauss);
    // SDL_Surface* colored_surface = load("/home/malossa/Documents/81px.png");
    SDL_Surface* colored_surface = load("/home/malossa/Downloads/sudoku2.jpeg");
    // surface_to_grayscale(colored_surface);
    test(colored_surface, gauss);
    return 0;
}
