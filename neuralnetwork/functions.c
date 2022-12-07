#include "functions.h"

#include <math.h>

// All activation function and derivative used in the neural network are here :

inline double
sigmoid(double x)
{
    return 1 / (1 + exp(-x));
}
inline double
dSigmoid(double x)
{
    return x * (1 - x);
}

inline double
relu(double x)
{
    if (x > 0) return x;
    else
        return 0;
}

inline double
drelu(double x)
{
    if (x > 0) return 1;
    else
        return 0;
}

void
softmax(double *layer, size_t len)
{
    double sum = 0;
    for (double *i = layer; i < layer + len; i++) {
        sum += exp(*i);
    }

    for (double *i = layer; i < layer + len; i++) {
        *i = (exp(*i)) / sum;
    }
}

double
dsoftmax(double *layer, size_t len, size_t target)
{
    double sum = 0;
    for (double *i = layer; i < layer + len; i++) {
        sum += exp(*i);
    }
    double t = exp(*(layer + target));

    return t * (sum - t) / (pow(sum, 2));
}

void
softmax_normalized(double *layer, size_t len)
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
dsoftmax_normalized(double *layer, size_t len, size_t target)
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