#pragma once
#include <stddef.h>
double
sigmoid(double x);

double
dSigmoid(double x);

double
relu(double x);

double
drelu(double x);

void
softmax(double*, size_t len);

double
dsoftmax(double*, size_t len, size_t i);
