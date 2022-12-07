#include "train.h"
#include "loadset.h"
#include "save.h"
#include "tools.h"
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void
forward_propagation(struct training *t, int input)
{
    for (int j = 0; j < t->hidden_count; j++) {
        double activation = t->nn.hidden_bias[j];

        for (size_t k = 0; k < INPUT_COUNT; k++) {
            activation += t->training_inputs[input * INPUT_COUNT + k] *
                          t->nn.hidden_weights[k * t->hidden_count + j];
        }

        t->hidden_layer[j] = sigmoid(activation);
    }

    for (int j = 0; j < OUTPUT_COUNT; j++) {
        double activation = t->nn.output_bias[j];

        for (int k = 0; k < t->hidden_count; k++) {
            activation += t->hidden_layer[k] * t->nn.output_weights[k * OUTPUT_COUNT + j];
        }

        t->output_layer[j] = sigmoid(activation);
    }
}

void
backward_propagation(struct training *t, int input)
{
    double deltaOutput[OUTPUT_COUNT];
    for (int j = 0; j < OUTPUT_COUNT; j++) {
        double error = (t->training_outputs[input * OUTPUT_COUNT + j] - t->output_layer[j]);
        deltaOutput[j] = error * dSigmoid(t->output_layer[j]);
    }

    // Compute change in hidden weights
    double deltaHidden[t->hidden_count];
    for (int j = 0; j < t->hidden_count; j++) {
        double error = 0.0f;
        for (int k = 0; k < OUTPUT_COUNT; k++) {
            error += deltaOutput[k] * t->nn.output_weights[j * OUTPUT_COUNT + k];
        }
        deltaHidden[j] = error * dSigmoid(t->hidden_layer[j]);
    }

    // Apply change in output weights
    for (int j = 0; j < OUTPUT_COUNT; j++) {
        t->nn.output_bias[j] += deltaOutput[j] * LEARNING_RATE;
        for (int k = 0; k < t->hidden_count; k++) {
            t->nn.output_weights[k * OUTPUT_COUNT + j] +=
                    t->hidden_layer[k] * deltaOutput[j] * LEARNING_RATE;
        }
    }
    // Apply change hidden weights
    for (int j = 0; j < t->hidden_count; j++) {
        t->nn.hidden_bias[j] += deltaHidden[j] * LEARNING_RATE;
        for (size_t k = 0; k < INPUT_COUNT; k++) {
            t->nn.hidden_weights[k * t->hidden_count + j] +=
                    t->training_inputs[input * INPUT_COUNT + k] * deltaHidden[j] *
                    LEARNING_RATE;
        }
    }
}

void
init(struct training *t)
{
    t->nn.sizes.hidden_bias_count = t->hidden_count;
    t->nn.sizes.output_bias_count = OUTPUT_COUNT;
    t->nn.sizes.hidden_weights_count = INPUT_COUNT * t->hidden_count;
    t->nn.sizes.output_weights_count = t->hidden_count * OUTPUT_COUNT;
    t->nn.sizes.hidden_count = t->hidden_count;

    t->hidden_layer = malloc(t->hidden_count * sizeof(double));
    t->nn.hidden_bias = calloc(sizeof(double), t->nn.sizes.hidden_bias_count);
    t->nn.output_bias = calloc(sizeof(double), t->nn.sizes.output_bias_count);

    t->nn.hidden_weights =
            malloc(t->nn.sizes.hidden_weights_count * sizeof(double));
    t->nn.output_weights =
            malloc(t->nn.sizes.output_weights_count * sizeof(double));

    for (size_t i = 0; i < INPUT_COUNT; i++) {
        for (int j = 0; j < t->hidden_count ; j++) {
            t->nn.hidden_weights[i * t->hidden_count  + j] = init_weights();
        }
    }
    for (int i = 0; i < t->hidden_count ; i++) {
        for (int j = 0; j < OUTPUT_COUNT; j++) {
            t->nn.output_weights[i * OUTPUT_COUNT + j] = init_weights();
        }
    }
}

void
train(char *path, int hiddenNodesNb, char *trainingsetpath, int epochNb)
{
    struct training t;
    t.hidden_count = hiddenNodesNb;
    srand((unsigned int) time(NULL));

    printf("Loading the training set\n");
    if (!dostuff(trainingsetpath, &t.training_inputs, &t.training_outputs, 0,
                 &t.training_count)) {
        printf("Training set loaded, inputNb : %d\n", INPUT_COUNT);
        init(&t);

        t.training_order = malloc(t.training_count * sizeof(int));
        for (size_t i = 0; i < t.training_count; i++) {
            t.training_order[i] = i;
        }

        for (int epoch = 0; epoch < epochNb; epoch++) {
            printf("epoch: %d\n", epoch);
            shuffle(t.training_order, t.training_count);
            for (size_t x = 0; x < t.training_count; x++) {
                int i = t.training_order[x];

                forward_propagation(&t, i);
                backward_propagation(&t, i);
            }
        }

//        save(&t.nn, "/home/rigole/Desktop/ogboi/og");
//        neural_network loldead = load("/home/rigole/Desktop/ogboi/og");

        save_model(t.hidden_layer, t.output_layer, t.nn.hidden_bias, t.nn.output_bias,
                   t.nn.hidden_weights, t.nn.output_weights, path, t.hidden_count, INPUT_COUNT);
    } else {
        errx(EXIT_FAILURE,
             "The dataset you are trying to load is bullshit mate!");
    }
}
