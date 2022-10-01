#include <SDL.h>
#include <SDL_image.h>
#include <math.h>
#include "linesdetection.h"


void
draw_line(int *pixels, long int w, long int x1, long int y1, long int x2, long int y2, Uint32 color)
{
    int i, dx, dy, maxmove;
    int d, dinc1, dinc2;
    int x, xinc1, xinc2;
    int y, yinc1, yinc2;

    dx = x1 > x2 ? x1 - x2 : x2 - x1;
    dy = y1 > y2 ? y1 - y2 : y2 - y1;

    if (dx >= dy) {
        maxmove = dx + 1;
        d = (2 * dy) - dx;
        dinc1 = 2 * dy;
        dinc2 = (dy - dx) * 2;
        xinc1 = 1;
        xinc2 = 1;
        yinc1 = 0;
        yinc2 = 1;
    }
    else {
        maxmove = dy + 1;
        d = (2 * dx) - dy;
        dinc1 = 2 * dx;
        dinc2 = (dx - dy) * 2;
        xinc1 = 0;
        xinc2 = 1;
        yinc1 = 1;
        yinc2 = 1;
    }

    if (x1 > x2) {
        xinc1 = -xinc1;
        xinc2 = -xinc2;
    }

    if (y1 > y2) {
        yinc1 = -yinc1;
        yinc2 = -yinc2;
    }

    x = x1;
    y = y1;

    for (i = 0; i < maxmove; ++i) {
        pixels[y * w + x] = color;
        if (d < 0) {
            d += dinc1;
            x += xinc1;
            y += yinc1;
        } else {
            d += dinc2;
            x += xinc2;
            y += yinc2;
        }
    }
}

int
detect_line(int *pixels, long int w, long int h, SDL_PixelFormat *format)
{
    long unsigned int diag;
    long unsigned int *mat;
    long unsigned int k;
    long unsigned int max_size;
    long unsigned int x0, y0, x1, y1, x2, y2;
    float m, n;
    Uint8 r, g, b;

    printf("%lu, %lu\n", w, h);

    SDL_Init(SDL_INIT_VIDEO);

    diag = sqrt(w * w + h * h);
    mat = calloc(diag * ANGLE, sizeof(long unsigned int));

    max_size = w > h ? w : h;

    if (mat == NULL) {
        printf("error\n");
        return 3;
    }

    for (size_t j = 0; j < (size_t)h; j++) {
        for (size_t i = 0; i < (size_t)w; i++) {
            SDL_GetRGB(pixels[j * w + i], format, &r, &g, &b);
            if (r == 0 && g == 0 && b == 0) {
                for (size_t theta = 0; theta < ANGLE; theta++) {
                    k = (long unsigned int)((double)j * sin((double)theta * PI/180) + (double)i * cos((double)theta * PI/180));
                    // printf("%lu, %lu, %lu, %lu\n", i, j, k, theta);
                    if (k < max_size) {
                        mat[k * ANGLE + theta] += 1;
                    }
                }
            }
        }
    }


    int kl = 0;
    for (size_t i = 0; i < max_size; i++) {
        for (size_t j = 0; j < ANGLE; j++) {
            if (mat[i * ANGLE +j] > 600) {
                // printf("mat[%lu][%lu] = %lu\n", i, j, mat[i * ANGLE + j]);
                m = cos(j * PI /180);
                n = sin(j * PI /180);
                x0 = m * i;
                y0 = n * i;
                x1 = x0 + 1000 * (-n);
                y1 = y0 + 1000 * (m);
                x2 = x0 - 1000 * (-n);
                y2 = x0 - 1000 * (m);
                if ( kl < 50 ){
                    printf("k: %lu, m: %f, n: %f, theta: %lu, x0: %lu, y0: %lu, x1: %lu, y1: %lu, x2: %lu, y2: %lu\n", mat[i * ANGLE +j], m, n, j, x0, y0, x1, y1, x2, y2);
                    kl +=1;
                }
                draw_line(pixels, w, x1, y1, x2, y2, SDL_MapRGB(format, 255, 0, 0));
            }
        }
    }

    printf("aaa\n");

    // draw_line(pixels, w, 0, 0, 1000, 1000, SDL_MapRGB(format, 255, 0, 0));

    SDL_Window * win;
    // win = SDL_CreateWindow("Hello World", 0, 0, w, h, 0);

    SDL_Renderer *renderer = SDL_CreateRenderer(win == NULL ? NULL : win, -1, SDL_RENDERER_ACCELERATED);
    SDL_Surface *surface = SDL_CreateRGBSurfaceFrom((void *)pixels, w, h, 32, format->BytesPerPixel * w, format->Rmask,  format->Gmask, format->Bmask, format->Amask);
    surface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_ARGB8888, 0);
    IMG_SavePNG(surface, "./test3.png");
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 0);

    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

/*
    int loopShouldStop = 0;
    int ok = 1;
    int lp = 0;
    while (!loopShouldStop)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    loopShouldStop = SDL_TRUE;
                    break;
            }
        }
        if (!lp){
            SDL_RenderDrawLine(renderer, 0, 0, 1000, 1000);
            SDL_RenderPresent(renderer);
            lp = 1;
        }

        if (!ok) {
            int l =0;
            for (size_t i = 0; i < max_size; i++) {
                for (size_t j = 0; j < ANGLE; j++) {
                    if (mat[i * ANGLE +j] > 500) {
                        // printf("mat[%lu][%lu] = %lu\n", i, j, mat[i * ANGLE + j]);
                        m = cos(j * PI / 180);
                        n = sin(j * PI / 180);
                        x0 = m * i;
                        y0 = n * i;
                        x1 = x0 + 1000 * (-n);
                        y1 = y0 + 1000 * (m);
                        x2 = x0 - 1000 * (-n);
                        y2 = x0 - 1000 * (m);
                        if ( l < 50 ){
                            printf("k: %lu, m: %f, n: %f, theta: %lu, x0: %lu, y0: %lu, x1: %lu, y1: %lu, x2: %lu, y2: %lu\n", mat[i * ANGLE +j], m, n, j, x0, y0, x1, y1, x2, y2);
                            l +=1;
                        }
                        // SDL_RenderDrawPoint(renderer, x0, y0);
                        if (SDL_RenderDrawLine(renderer, x1, y1, x2, y2)){
                            printf("ERROR %s\n", SDL_GetError());
                            return -1;
                        }
                    }
                }
            }
            ok = 1; 
            printf("okkk\n");
            // SDL_RenderClear(renderer);
            // SDL_RenderCopy(renderer, texture, NULL, NULL);
            SDL_RenderPresent(renderer);
        }

    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);

    SDL_Quit();

*/



    SDL_Surface* surface_temp = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
    surface_temp = SDL_ConvertSurfaceFormat(surface_temp, SDL_PIXELFORMAT_RGB888, 0);
    SDL_RenderReadPixels(renderer, NULL, surface_temp->format->format, surface_temp->pixels, surface_temp->pitch);
    IMG_SavePNG(surface_temp, "./test.png");
    SDL_FreeSurface(surface_temp);
    
    SDL_Surface *hough_surf = SDL_CreateRGBSurface(0, max_size, ANGLE, 32, 0, 0, 0, 0);
    hough_surf = SDL_ConvertSurfaceFormat(hough_surf, SDL_PIXELFORMAT_RGB888, 0);
    SDL_LockSurface(hough_surf);
    unsigned int *hough_pixels = hough_surf->pixels;
    for (size_t i = 0; i < max_size; i++) {
        for (size_t j = 0; j < ANGLE; j++) {
            hough_pixels[i * ANGLE + j] = SDL_MapRGB(format, mat[i * ANGLE + j], 0, 0);
        }
    }
    SDL_UnlockSurface(hough_surf);
    // SDL_RenderReadPixels(renderer, NULL, hough_surf->format->format, hough_surf->pixels, hough_surf->pitch);
    IMG_SavePNG(hough_surf, "./test2.png");
    SDL_FreeSurface(hough_surf);


    // for (size_t j = 0; j < (size_t)h; j++) {
    //     for (size_t i = 0; i < (size_t)w; i++) {
    //         SDL_GetRGB(pixels[j * w + i], format, &r, &g, &b);
    //         printf("%c ", r == 0 ? '0' : '.');
    //     }
    //     printf("\n");
    // }



    free(mat);

    return 0;
}


int
main (int argc, char **argv)
{
    if (argc != 2) {
        printf("Usage: %s path_to_image\n", argv[0]);
        return 1;
    }

    SDL_Surface *image_temp = IMG_Load(argv[1]);

    image_temp = SDL_ConvertSurfaceFormat(image_temp, SDL_PIXELFORMAT_RGB888, 0);

    detect_line(image_temp->pixels, image_temp->w, image_temp->h, image_temp->format);

    SDL_FreeSurface(image_temp);

    return 0;
}
