#include "gaussian_blur.h"

// function that create the gaussian kernel
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
    // only for printing the kernel, not important
    // for (i = 0; i < smooth_kernel_size; i++) {
    //     for (j = 0; j < smooth_kernel_size; j++) {
    //         printf("%f ", gauss[i][j]);
    //     }
    //     printf("\n");
    // }
}

// function that compute the gaussian kernel to the grayscale
// image, output image is the parameter out
void
compute(SDL_Surface* surface,
        double gauss[smooth_kernel_size][smooth_kernel_size],
        int k,
        SDL_Surface* out)
{
    int w = surface->w;
    int h = surface->h;
    Uint32* pixels = surface->pixels;
    Uint32* pixelsOut = out->pixels;
    SDL_PixelFormat* format = surface->format;
    long int Gx_compute = 0;
    for (size_t i = 2; i < h - 2; i++) {
        for (size_t j = 2; j < w - 2; j++) {
            Gx_compute = 0;
            for (int x = -2; x < 3; x++) {
                for (int y = -2; y < 3; y++) {
                    // if (i + y >= 0 && i + y < h && j + x >= 0 && j + x < w) {
                    int kx = x + 2;
                    int ky = y + 2;
                    int val = pixels[(i + y) * w + (j + x)] >> 16 & 0xff;
                    Gx_compute += val * gauss[kx][ky];
                    // }
                }
            }
            pixelsOut[i * w + j] =
              SDL_MapRGB(format, Gx_compute, Gx_compute, Gx_compute);
        }
    }
}
