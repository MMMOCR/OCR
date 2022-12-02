#include "train.h"

#include "functions.h"
#include "save.h"
#include "tools.h"
#include "loadset.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <err.h>

void
train(char* path,int hiddenNodesNb,char* trainingsetpath, int epochNb)
{
    float learningRate = 0.1;
    srand((unsigned int) time(NULL));
    
    //load training set
    /*
    double training_inputs[trainingSetsNb][inputNb] = {
        { 0.0f, 0.0f }, { 1.0f, 0.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f }
    };
    double training_outputs[trainingSetsNb][outputNb] = {
        { 0.0f }, { 1.0f }, { 1.0f }, { 0.0f }
    };
    */
    double *training_inputs;
    char *training_outputs;
    size_t trainingSetsNb;
    size_t inputNb;
    printf("Loading the training set\n");
    if (!dostuff(trainingsetpath, &training_inputs, &training_outputs, &inputNb, &trainingSetsNb)){
    printf("Training set loaded, inputNb : %lu\n",inputNb);

    //init layers:
    //static: (deprecated)
    /*
    double hiddenLayer[hiddenNodesNb];
    double outputLayer[outputNb];

    double hiddenLayerBias[hiddenNodesNb];
    double outputLayerBias[outputNb];

    double hiddenWeights[inputNb][hiddenNodesNb];
    double outputWeights[hiddenNodesNb][outputNb];
    */
   //dynamic
    double *hiddenLayer = malloc(hiddenNodesNb * sizeof(double));
    //double *outputLayer = malloc(outputNb * sizeof(double));
    double outputLayer[outputNb];
    
    double *hiddenLayerBias = malloc(hiddenNodesNb * sizeof(double));
    //double *outputLayerBias = malloc(outputNb * sizeof(double));
    double outputLayerBias[outputNb];

    double *hiddenWeights = malloc(inputNb * hiddenNodesNb * sizeof(double*));
    double *outputWeights = malloc(hiddenNodesNb * outputNb * sizeof(double*));

    for (size_t i = 0; i < inputNb; i++) {
        for (int j = 0; j < hiddenNodesNb; j++) {
            hiddenWeights[i * hiddenNodesNb +j] = init_weights();
        }
    }
    for (int i = 0; i < hiddenNodesNb; i++) {
        for (int j = 0; j < outputNb; j++) {
            outputWeights[i * outputNb+ j] = init_weights();
        }
    }
    for (int i = 0; i < outputNb; i++) {
        outputLayerBias[i] = init_weights();
    }

    int *trainingSetOrder = malloc(trainingSetsNb * sizeof(int));
    

    // Train the NN for the defined number of epochs
    for (int epoch = 0; epoch < epochNb; epoch++) {
        shuffle(trainingSetOrder, trainingSetsNb);

        //train the NN for each value of the training set
        for (size_t x = 0; x < trainingSetsNb; x++) {
            int i = trainingSetOrder[x];

            // Forward pass
            // Compute hidden layer activation
            for (int j = 0; j < hiddenNodesNb; j++) {
                double activation = hiddenLayerBias[j];

                for (size_t k = 0; k < inputNb; k++) {
                    activation += training_inputs[i * inputNb + k] * hiddenWeights[k * hiddenNodesNb +j];
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
        
            printf(" Output : [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f] || Expected output : [%d,%d,%d,%d,%d,%d,%d,%d,%d,%d]\n",
                   outputLayer[0],outputLayer[1],outputLayer[2],outputLayer[3],outputLayer[4],outputLayer[5],
                   outputLayer[6],outputLayer[7],outputLayer[8],outputLayer[9],
                   training_outputs[i * outputNb + 0],training_outputs[i * outputNb + 1],training_outputs[i * outputNb + 2]
                   ,training_outputs[i * outputNb + 3],training_outputs[i * outputNb + 4],training_outputs[i * outputNb + 5],
                   training_outputs[i * outputNb + 6],training_outputs[i * outputNb + 7],training_outputs[i * outputNb + 8],training_outputs[i * outputNb + 9]);
            
            // Retropropagation implem

            // Compute change in output weights

            double deltaOutput[outputNb];
            for (int j = 0; j < outputNb; j++) {
                double error = (training_outputs[i * outputNb + j] - outputLayer[j]);
                deltaOutput[j] = error * dSigmoid(outputLayer[j]);
            }

            // Compute change in hidden weights
            double deltaHidden[hiddenNodesNb];
            for (int j = 0; j < hiddenNodesNb; j++) {
                double error = 0.0f;
                for (int k = 0; k < outputNb; k++) {
                    error += deltaOutput[k] * outputWeights[j * outputNb + k];
                }
                deltaHidden[j] = error * dSigmoid(hiddenLayer[j]);
            }

            // Apply change in output weights
            for (int j = 0; j < outputNb; j++) {
                outputLayerBias[j] += deltaOutput[j] * learningRate;
                for (int k = 0; k < hiddenNodesNb; k++) {
                    outputWeights[k * outputNb + j] +=
                      hiddenLayer[k] * deltaOutput[j] * learningRate;
                }
            }
            // Apply change hidden weights
            for (int j = 0; j < hiddenNodesNb; j++) {
                hiddenLayerBias[j] += deltaHidden[j] * learningRate;
                for (size_t k = 0; k < inputNb; k++) {
                    hiddenWeights[k * hiddenNodesNb + j] +=
                      training_inputs[i * inputNb + k] * deltaHidden[j] * learningRate;
                }
            }
        }
    }
    save_model(hiddenLayer, outputLayer, hiddenLayerBias, outputLayerBias,
               hiddenWeights, outputWeights, path, hiddenNodesNb, inputNb);
    }
    else{
        errx(EXIT_FAILURE,
                     "The dataset you are trying to load is bullshit mate!");
    }
    
}
