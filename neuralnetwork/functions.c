#include "functions.h"

#include <math.h>

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
