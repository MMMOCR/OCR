#include "otsu.h"

#include <SDL2/SDL_stdinc.h>

void
to_black(SDL_Surface* surface,
         int threshold,
         long int x,
         long int y,
         long int w,
         long int h)
{
    // printf("%li %li %li %li\n", x, y, w, h);
    Uint32* pixels = surface->pixels;
    int old_w = surface->w, old_h = surface->h;
    SDL_PixelFormat* format = surface->format;
    SDL_LockSurface(surface);
    for (size_t i = x; i < w; i++) {
        for (size_t j = y; j < h; j++) {
            int val = pixels[i * old_w + j] >> 16 & 0xff;
            int new_val = 0;
            if (val > threshold) new_val = 255;
            pixels[i * old_w + j] =
              SDL_MapRGB(format, new_val, new_val, new_val);
        }
    }
    SDL_UnlockSurface(surface);
}

void
multiple(long int w, long int h, SDL_Surface* surface)
{

    int global = otsu_treshold(surface->w * surface->h, surface->pixels, 0);
    printf("%li %li\n", w, h);
    SDL_Rect crop;
    int ret[9];
    int k = 0;

    for (size_t i = 0; i < 3; i++) {
        for (size_t j = 0; j < 3; j++) {
            crop.x = i * w / 3, crop.y = j * h / 3;
            crop.w = (i + 1) * w / 3, crop.h = (j + 1) * h / 3;
            SDL_Surface* new = SDL_CreateRGBSurface(
              0, surface->w / 3, surface->h / 3, 32, 0, 0, 0, 0);

            SDL_BlitSurface(surface, &crop, new, NULL);

            // printf("%i %i\n", new->w, new->h);
            int threshold = otsu_treshold(new->w* new->h, new->pixels, 0);
            ret[k] = threshold;
            k++;
        }
    }
    int x, y, ww, hh;
    k = 0;
    for (size_t i = 0; i < 3; i++) {
        for (size_t j = 0; j < 3; j++) {
            x = i * w / 3, y = j * h / 3;
            ww = (i + 1) * w / 3, hh = (j + 1) * h / 3;
            // printf("%d\n", (int) (ret[k]*0.85+global*0.25));
            // to_black(surface, (int)(ret[k]*0.85+global*0.19), y,x, hh, ww);
            to_black(surface, (int) (ret[k] * 0.5 + global * 0.5), y, x, hh,
                     ww);
            // to_black(surface, global, y,x, hh, ww);
            // to_black(surface, ret[k], y,x, hh, ww);
            k++;
        }
    }
}

int
otsu_treshold(long int len, Uint32* pixels, int override_treshold)
{
    int treshold = 0;
    int long histogram[256];
    long int pbg = 0, pfg = 0, var_max = 0, sumbg = 0, sumfg = 0;
    long double meanbg = 0.0, meanfg = 0.0;
    long double sumvarbg = 0.0, sumvarfg = 0.0, var = 0.0;
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
        for (size_t i = 1; i <= 255; i++) {
            for (size_t j = 0; j < i; j++) {
                long int val = histogram[j];
                if (val != 0) {
                    pbg += histogram[j];
                    sumbg += j * histogram[j];
                }
            }
            for (size_t j = i; j < 255; j++) {
                pfg += histogram[j];
                sumfg += j * histogram[j];
            }
            if (pbg == 0 || pfg == 0) continue;
            meanbg = (long double) sumbg / pbg;
            meanfg = (long double) sumfg / pfg;
            for (size_t j = 0; j < i; j++) {
                sumvarbg += (histogram[j]) * (j - meanbg) * (j - meanbg);
            }
            for (size_t j = i; j < 255; j++) {
                sumvarfg += (histogram[j]) * (j - meanfg) * (j - meanfg);
            }
            var = pfg * pbg * (meanbg - meanfg) * (meanbg - meanfg);
            if (var > var_max) {
                treshold = i;
                var_max = var;
            }
            pbg = 0, pfg = 0, sumfg = 0.0, sumbg = 0.0, sumvarbg = 0.0,
            sumvarfg = 0.0;
        }
    }
    printf("%d\n", treshold);
    return treshold;
}
