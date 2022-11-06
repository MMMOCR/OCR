#include "job.h"
#include "save.h"
#include "NN.h"
#include "functions.h"

#include <stdio.h>

void
job(char* path, char* sinput)
{
    double inputs[] = {sinput[0],sinput[1]};

    double hiddenLayer[hiddenNodesNb];
    double outputLayer[outputNb];

    double hiddenLayerBias[hiddenNodesNb];
    double outputLayerBias[outputNb];

    double hiddenWeights[inputNb][hiddenNodesNb];
    double outputWeights[hiddenNodesNb][outputNb];

    load_model(hiddenLayer, outputLayer, hiddenLayerBias, outputLayerBias, (double**)hiddenWeights, (double**)outputWeights, path);

    for (int j = 0; j < hiddenNodesNb; j++) {
                double activation = hiddenLayerBias[j];

                for (int k = 0; k < inputNb; k++) {
                    activation += inputs[k] * hiddenWeights[k][j];
                }

                // /!\ function used on the hidden layer must be called in a way
                // to be changed in the NN config in NN.h
                hiddenLayer[j] = sigmoid(activation);
            }
            // Compute output layer activation
            for (int j = 0; j < outputNb; j++) {
                double activation = outputLayerBias[j];
                for (int k = 0; k < hiddenNodesNb; k++) {
                    activation += hiddenLayer[k] * outputWeights[k][j];
                }

                // /!\ function used on the output layer must be called in a way
                // to be changed in the NN config in NN.h
                outputLayer[j] = sigmoid(activation);
            }

    printf("I think the result of %g XOR %g is: %g\n", inputs[0], inputs[1], outputLayer[0]);
}
