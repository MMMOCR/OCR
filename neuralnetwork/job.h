#pragma once
#include "NN.h"

int
job(neural_network nn, double *input);

double
fscore(neural_network nn, double **inputs, int *real, size_t len);