#pragma once
#include <stddef.h>
#include <SDL2/SDL_image.h>

#define OUTPUT_COUNT 10
#define INPUT_COUNT 784

struct nn_sizes
{
    size_t hidden_weights_count;
    size_t hidden_bias_count;
    size_t output_weights_count;
    size_t output_bias_count;

    size_t hidden_count;
};

struct
{
    double *hidden_weights;
    double *hidden_bias;
    double *output_weights;
    double *output_bias;

    struct nn_sizes sizes;
} typedef neural_network;

void
save(neural_network *nn, char *path);
neural_network
load(char *path);