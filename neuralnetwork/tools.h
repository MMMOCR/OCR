#pragma once
#include <SDL2/SDL_image.h>
#include <stddef.h>
void
shuffle(int* array, size_t n);

double
init_weights();

void
exit_usage();

void
delay(int number_of_seconds);

double*
PicToList(SDL_Surface* image);