//
// Created by rigole on 03/12/22.
//

#ifndef SYPBC_SYPBC_IMPL_H
#define SYPBC_SYPBC_IMPL_H

#include "../../sypbc.h"
#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_ttf.h>

static struct {
    SDL_Surface *screen;
    SDL_Rect clip;
} sdl;

static void
rect(SDL_Surface *surface, short x, short y, unsigned short w, unsigned short h, unsigned short r, rgba col)
{
    if (r == 0) {
        rectangleRGBA(surface, x, y, x + w, y + h, col.r, col.g, col.b, col.a);
    } else {
        roundedRectangleRGBA(surface, x, y, x + w, y + h, r, col.r, col.g, col.b, col.a);
    }
}

static void
fill_rect(SDL_Surface *surface, short x, short y, unsigned short w, unsigned short h, unsigned short r, rgba col)
{
    if (r == 0) {
        boxRGBA(surface, x, y, x + w, y + h, col.r, col.g, col.b, col.a);
    } else {
        roundedBoxRGBA(surface, x, y, x + w, y + h, r, col.r, col.g, col.b, col.a);
    }
}

static void
line(SDL_Surface *surface, short x0, short y0, short x1,
                   short y1, unsigned int line_thickness, rgba col)
{
    thickLineRGBA(surface, x0, y0, x1, y1, line_thickness, col.r, col.g, col.b, col.a);
}

static void
circle(SDL_Surface *surface, short x, short y, int radius, rgba col)
{
    aacircleRGBA(surface, x, y, radius, col.r, col.g, col.b, col.a);
}

static void
filled_circle(SDL_Surface *surface, short x, short y, int radius, rgba col)
{
    aacircleRGBA(surface, x, y, radius, col.r, col.g, col.b, col.a);
    filledCircleRGBA(surface, x, y, radius, col.r, col.g, col.b, col.a);
}

static void
scissors(SDL_Surface *surface, SDL_Rect *clip, float x, float y, float w, float h)
{
    clip->x = x;
    clip->y = y;
    clip->w = w  + 1;
    clip->h = h;
    SDL_SetClipRect(surface, clip);
}

#endif //SYPBC_SYPBC_IMPL_H
