#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int
extract_path(char* path, int* board);

SDL_Surface*
load_image(const char* path);

void
apply_surface(int x, int y, SDL_Surface* source, SDL_Surface* destination);