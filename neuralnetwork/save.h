#pragma once
#include "NN.h"

#include <stddef.h>
#include <stdio.h>

void
save_model(double *hiddenLayer,
           double outputLayer[],
           double *hiddenLayerBias,
           double outputLayerBias[],
           double *hiddenWeights,
           double *outputWeights,
           char *path,
           int hiddenNodesNb,
           size_t inputNb);

void
load_model(double hiddenLayer[],
           double outputLayer[],
           double hiddenLayerBias[],
           double outputLayerBias[],
           double *hiddenWeights,
           double *outputWeights,
           char *path,
           int hiddenNodesNb,
           size_t inputNb);

void
load_array(FILE *file, double array[], size_t len);

void
load_2darray(FILE *file, double *array, size_t x, size_t y);
