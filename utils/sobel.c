#include "sobel.h"

#include <SDL2/SDL_stdinc.h>

void
convolve(Uint32* pixels, int* Gx_compute, int* Gy_compute, int i, int j, int w)
{
    int Gx[5][5] = { { 2, 2, 4, 2, 2 },
                     { 1, 1, 2, 1, 1 },
                     { 0, 0, 0, 0, 0 },
                     { -1, -1, -2, -1, -1 },
                     { -2, -2, -4, -2, -2 } };
    int Gy[5][5] = { { 2, 1, 0, -1, -2 },
                     { 2, 1, 0, -1, -2 },
                     { 4, 2, 0, -2, -4 },
                     { 2, 1, 0, -1, -2 },
                     { 2, 1, 0, -1, -2 } };
    for (int x = -2; x < 3; x++) { // compute the kernel times the
        for (int y = -2; y < 3; y++) { // pixels
            int kx = x + 2;
            int ky = y + 2;
            int val = pixels[(i + y) * w + j + x] >> 16 & 0xff;
            *Gx_compute += val * Gx[kx][ky];
            *Gy_compute += val * Gy[kx][ky];
        }
    }
}

void
edges(SDL_Surface* surface)
{
    int h = surface->h;
    int w = surface->w;
    Uint32* pixels = surface->pixels;
    int Gx[5][5] = { { 2, 2, 4, 2, 2 },
                     { 1, 1, 2, 1, 1 },
                     { 0, 0, 0, 0, 0 },
                     { -1, -1, -2, -1, -1 },
                     { -2, -2, -4, -2, -2 } };
    int Gy[5][5] = { { 2, 1, 0, -1, -2 },
                     { 2, 1, 0, -1, -2 },
                     { 4, 2, 0, -2, -4 },
                     { 2, 1, 0, -1, -2 },
                     { 2, 1, 0, -1, -2 } };
    // int Gx[5][5] = { {-5, -4, 0, 4, 5}, {-8, -10, 0, 10, 8},
    // {-10, -20, 0, 20, 10}, {-8, -10, 0, 10, 8}, {-5, -4, 0, 4, 5}};
    // int Gy[5][5] = { {-5, -8, -10, -8, -5}, {-4, -10, -20, -10, -4},
    // {0, 0, 0, 0, 0}, {4, 10, 20, 10, 4}, {5, 8, 10, 8, 5}};
    // alternative kernel, first one produce better result tho
    SDL_PixelFormat* format = surface->format;
    Uint32* result_pixels = calloc(h * w + 1, sizeof(Uint32));

    int Gx_compute = 0;
    int Gy_compute = 0;
    int valx = 0;
    int valy = 0;
    int kx, ky;
    int max = -9999999, min = 9999999;

    for (size_t i = 2; i < h - 2; i++) { // begining at 2 because of the kernel
        for (size_t j = 2; j < w - 2; j++) { // padding
            Gx_compute = 0;
            Gy_compute = 0;
            // for (int x = -2; x < 3; x++) { // compute the kernel times the
            //     for (int y = -2; y < 3; y++) { // pixels
            //         int kx = x+2;
            //         int ky = y+2;
            //         int val = pixels[(i+y)*w + (j+x)]>>16&0xff;
            //         Gx_compute += val*Gx[kx][ky];
            //         Gy_compute += val*Gy[kx][ky];
            //     }
            // }
            convolve(pixels, &Gx_compute, &Gy_compute, i, j, w);
            int sobel = sqrt(Gx_compute * Gx_compute + Gy_compute * Gy_compute);
            result_pixels[i * w + j] = sobel;
            if (sobel < min) { min = sobel; }
            if (sobel > max) { max = sobel; }
        }
    }

    long int sum = 0;
    for (size_t i = 0; i < h; i++) {
        for (size_t j = 0; j < w; j++) {
            int val = result_pixels[i * w + j];
            int test =
              255 * (double) (val - min) / (max - min); // set between 0 and 255
            result_pixels[i * w + j] = test;
            sum += test; // sum everything to make median value
            pixels[i * w + j] = SDL_MapRGB(format, test, test, test);
        }
    }
    int med = sum / (h * w);
    for (size_t i = 0; i < h; i++) {
        for (size_t j = 0; j < w; j++) {
            int val = result_pixels[i * w + j];
            if (val > 3 * med) val = 255; // decide if pixel black, or white
            else
                val = 0;
            pixels[i * w + j] = SDL_MapRGB(format, val, val, val);
        }
    }
    free(result_pixels);
}

/* reduce black pixels
  for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++) {
            Gx_compute = Gy_compute = 0;
            for (int x = -1; x < 2; x++) {
                for (int y = -1; y < 2; y++) {
                    if (i+x < h && i+x > 0 && j+y < w && j+y >0)
                    {

                        kx = x+1;
                        ky = y+1;
                        // printf("%d\n", ky);
                        valx = (pixels[(i+x)*h + (j+y)] >> 16 &
  0xff)*Gx[kx][ky];
                        // printf("%d\n", pixels[(i+x)*h + (j+y)] >> 16 & 0xff);
                        Gx_compute += valx;
                        valy = (pixels[(i+x)*h + (j+y)] >> 16 &
  0xff)*Gy[kx][ky];
                        // printf("%d\n", valy);
                        Gy_compute += valy;
                    }
                }
            }
            // printf("%d\n", sobel(Gx_compute, Gy_compute));
            result_pixels[i*h + j] = sobel(Gx_compute, Gy_compute);
        }
    }*/
