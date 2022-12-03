#include "job.h"

#include "NN.h"
#include "functions.h"
#include "save.h"

#include <stdio.h>
#include <stdlib.h>

int
job(char *path, double *inputs, int hiddenNodesNb, int inputNb)
{
    /* static
    double hiddenLayer[hiddenNodesNb];
    double outputLayer[outputNb];

    double hiddenLayerBias[hiddenNodesNb];
    double outputLayerBias[outputNb];

    double hiddenWeights[inputNb * hiddenNodesNb];
    double outputWeights[hiddenNodesNb * outputNb];
    */
    // dynamic
    double *hiddenLayer = malloc(hiddenNodesNb * sizeof(double));
    // double *outputLayer = malloc(outputNb * sizeof(double));
    double outputLayer[outputNb];

    double *hiddenLayerBias = malloc(hiddenNodesNb * sizeof(double));
    // double *outputLayerBias = malloc(outputNb * sizeof(double));
    double outputLayerBias[outputNb];

    double *hiddenWeights = malloc(inputNb * hiddenNodesNb * sizeof(double *));
    double *outputWeights = malloc(hiddenNodesNb * outputNb * sizeof(double *));

    load_model(hiddenLayer, outputLayer, hiddenLayerBias, outputLayerBias,
               (double *) hiddenWeights, (double *) outputWeights, path,
               hiddenNodesNb, inputNb);

    for (int j = 0; j < hiddenNodesNb; j++) {
        double activation = hiddenLayerBias[j];

        for (int k = 0; k < inputNb; k++) {
            activation += inputs[k] * hiddenWeights[k * hiddenNodesNb + j];
        }

        // /!\ function used on the hidden layer must be called in a way
        // to be changed in the NN config in NN.h
        hiddenLayer[j] = sigmoid(activation);
    }
    // Compute output layer activation
    for (int j = 0; j < outputNb; j++) {
        double activation = outputLayerBias[j];
        for (int k = 0; k < hiddenNodesNb; k++) {
            activation += hiddenLayer[k] * outputWeights[k * outputNb + j];
        }

        // /!\ function used on the output layer must be called in a way
        // to be changed in the NN config in NN.h
        outputLayer[j] = sigmoid(activation);
    }

    // printf("I think the result of %c XOR %c is: %g\n", (char) inputs[0] +
    // 48,(char) inputs[1] + 48, outputLayer[0]);
    int maxi = 0;
    double max = outputLayer[maxi];
    for (; maxi < outputNb; maxi++) {
        if (outputLayer[maxi] > max) { max = outputLayer[maxi]; }
    }
    return maxi;
}