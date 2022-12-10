#pragma once
#include "NN.h"
#include "functions.h"

#include <stdio.h>

#define LEARNING_RATE 0.0001

struct training
{
    neural_network nn;

    double *training_inputs;
    char *training_outputs;

    int *training_order;
    size_t training_count;

    double *hidden_layer;
    double input_layer[INPUT_COUNT];
    double output_layer[OUTPUT_COUNT];
};

void
train(char *path, int hiddenNodesNb, char *trainingsetpath, int epochNb);
void
init(struct training *t);
void
forward_propagation(struct training *t, double *inputs, int input);
void
backward_propagation(struct training *t, int input);
