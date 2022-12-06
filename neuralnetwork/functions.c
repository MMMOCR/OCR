#include "functions.h"

#include <math.h>
#include <stdio.h>

// All activation function and derivative used in the neural network are here :

double
sigmoid(double x)
{
    return 1 / (1 + exp(-x));
}
double
dSigmoid(double x)
{
    return x * (1 - x);
}

double
relu(double x)
{
    if (x > 0) return x;
    else
        return 0;
}

double
drelu(double x)
{
    if (x > 0) return 1;
    else
        return 0;
}

void
softmax(double *layer, size_t len)
{
    double max = *layer;
    for (double *i = layer; i < layer + len; i++) {
        if (max < *i) { max = *i; }
    }

    double sum = 0;
    for (double *i = layer; i < layer + len; i++) {
        sum += exp(*i - max);
    }

    for (double *i = layer; i < layer + len; i++) {
        *i = (exp(*i - max)) / exp(sum);
    }
}

double
dsoftmax(double *layer, size_t len, size_t target)
{
    double max = *layer;
    for (double *i = layer; i < layer + len; i++) {
        if (max < *i) { max = *i; }
    }
    double sum = 0;
    for (double *i = layer; i < layer + len; i++) {
        sum += exp(*i - max);
    }
    double t = exp(*(layer + target) - max);

    return t * (sum - t) / (pow(sum, 2) + 10E-10);
}