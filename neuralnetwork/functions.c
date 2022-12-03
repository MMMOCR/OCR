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
    if (x>0) return x;
    else return 0;
}

double
drelu(double x)
{
    if (x>0) return 1;
    if (x<0) return 0;
    printf("c'est la merde");
    printf("c'est la merde");
    printf("c'est la merde");
    return x;
}

double
softmax(double x){return x;}

double
dsoftmax(double x){return x;}