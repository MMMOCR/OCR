#include "WriteonIm.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <err.h>

int
extract_path(char *path, int *board)
{
    char *line;
    size_t rsize;
    ssize_t readbytes;
    int index;
    FILE *file;

    if ((file = fopen(path, "r")) == NULL) { return 0; }

    index = 0;
    readbytes = 0;
    while ((readbytes = getline(&line, &rsize, file)) != -1) {
        for (ssize_t i = 0; i < readbytes; i++) {
            char c = line[i];
            if (c == '.') {
                board[index++] = 0;
            } else if (c >= '1' && c <= '9') {
                board[index++] = c - '0';
            }
        }
    }
    return 1;
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

void apply_surface( int x, int y, SDL_Surface* source, SDL_Surface* destination ) {
    SDL_Rect offset;

    offset.x = x;
    offset.y = y;
    SDL_BlitSurface(source, NULL, destination, &offset);
}

void write_on_im(char *path, char *path2, char* empty, char* solved)
{
    if (TTF_Init() == -1) {
        printf("SDL_ttf init fail: %s\n", TTF_GetError());
    }

    int* emptygrid = malloc(sizeof(int) * (81));
    int* solvedgrid = malloc(sizeof(int) * (81));
    extract_path(empty,emptygrid);
    extract_path(solved,solvedgrid);

    SDL_Surface* image = load_image(path);
    int height = image->h;
    int width = image->w;
    SDL_Color color = { 0, 255,0 };
    int h = height/9;
    TTF_Font *font = TTF_OpenFont("Roboto-Black.ttf", h/2);
    int w = width/9;
    int coordx = (width/9)/3;
    int coordy = (height/9)/4;

    for(int i = 0; i< 9; i++)
    {
        for(int j = 0; j<9; j++)
        {
            if (emptygrid[i*9+j]==0)
            {
                char c = solvedgrid[i * 9 + j] + '0';
                char *str = &c;
                SDL_Surface *surface = TTF_RenderText_Solid(font, str, color);
                apply_surface(j * w + coordx, i * h + coordy, surface, image);
            }
        }
    }
    IMG_SavePNG(image,path2);
    TTF_CloseFont(font);
}

int main(int argc,char **argv)
{
    write_on_im(argv[1],argv[2],argv[3],argv[4]);
}

