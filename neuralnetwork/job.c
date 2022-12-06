#include "job.h"

#include "NN.h"
#include "functions.h"
#include "save.h"

#include <stdio.h>

void
job(char* path, char* sinput)
{
    double inputs[] = { sinput[0] - 48, sinput[1] - 48 };

    double hiddenLayer[hiddenNodesNb];
    double outputLayer[outputNb];

    double hiddenLayerBias[hiddenNodesNb];
    double outputLayerBias[outputNb];

    double hiddenWeights[inputNb * hiddenNodesNb];
    double outputWeights[hiddenNodesNb * outputNb];

    load_model(hiddenLayer, outputLayer, hiddenLayerBias, outputLayerBias,
               (double*) hiddenWeights, (double*) outputWeights, path);

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

    printf("I think the result of %c XOR %c is: %g\n", (char) inputs[0] + 48,
           (char) inputs[1] + 48, outputLayer[0]);
}