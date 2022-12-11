#include "erosion_dilation.h"
#include "imageutils.h"

#include <libgen.h>

void
resize_to_image(SDL_Surface* surface, int w, int h, char * s)
{
    // SDL_Surface* out = SDL_CreateRGBSurface(0, surface->w,
    //     surface->h, 32, 0, 0, 0, 0);
    // erode(surface->pixels, out->pixels, 2, out->w, out->h);

    SDL_Rect src = { 0, 0, w, h };
    SDL_Surface* cropped = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);

    SDL_BlitScaled(surface, NULL, cropped, &src);
    // erode(cropped->pixels, out->pixels, 3, out->w, out->h);

    IMG_SavePNG(cropped, s);
    SDL_FreeSurface(cropped);
}
