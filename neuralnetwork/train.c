#include "train.h"
#include "functions.h"
#include "tools.h"
#include "save.h"
#include <stdio.h>
#include <stddef.h>
void
train(char* path){

double hiddenLayer[hiddenNodesNb];
double outputLayer[outputNb];

double hiddenLayerBias[hiddenNodesNb];
double outputLayerBias[outputNb];

double hiddenWeights[inputNb][hiddenNodesNb];
double outputWeights[hiddenNodesNb][outputNb];

double training_inputs[trainingSetsNb][inputNb] = { { 0.0f, 0.0f },
	{ 1.0f, 0.0f },
	{ 0.0f, 1.0f },
	{ 1.0f, 1.0f } };
double training_outputs[trainingSetsNb][outputNb] = { { 0.0f },
	{ 1.0f },
	{ 1.0f },
	{ 0.0f } };

for (int i = 0; i < inputNb; i++) {
			for (int j = 0; j < hiddenNodesNb; j++) {
				hiddenWeights[i][j] = init_weights();
			}
		}
		for (int i = 0; i < hiddenNodesNb; i++) {
			for (int j = 0; j < outputNb; j++) {
				outputWeights[i][j] = init_weights();
			}
		}
		for (int i = 0; i < outputNb; i++) {
			outputLayerBias[i] = init_weights();
		}

		int trainingSetOrder[] = { 0, 1, 2, 3 };

		// Train the NN for the defined number of epochs
		for (int epoch = 0; epoch < epochNb; epoch++) {
			shuffle(trainingSetOrder, trainingSetsNb);

			for (int x = 0; x < trainingSetsNb; x++) {
				int i = trainingSetOrder[x];

				// Forward pass
				// Compute hidden layer activation
				for (int j = 0; j < hiddenNodesNb; j++) {
					double activation = hiddenLayerBias[j];

					for (int k = 0; k < inputNb; k++) {
						activation += training_inputs[i][k] * hiddenWeights[k][j];
					}

					// /!\ function used on the hidden layer must be called in a way to be
					// changed in the NN config in NN.h
					hiddenLayer[j] = sigmoid(activation);
				}
				// Compute output layer activation
				for (int j = 0; j < outputNb; j++) {
					double activation = outputLayerBias[j];
					for (int k = 0; k < hiddenNodesNb; k++) {
						activation += hiddenLayer[k] * outputWeights[k][j];
					}

					// /!\ function used on the output layer must be called in a way to be
					// changed in the NN config in NN.h
					outputLayer[j] = sigmoid(activation);
				}
				printf("Input : %g,%g ==> Output : %g || Expected output : %g\n",
						training_inputs[i][0], training_inputs[i][1], outputLayer[0],
						training_outputs[i][0]);

				// Retropropagation inplem

				// Compute chnage in output weights

				double deltaOutput[outputNb];
				for (int j = 0; j < outputNb; j++) {
					double error = (training_outputs[i][j] - outputLayer[j]);
					deltaOutput[j] = error * dSigmoid(outputLayer[j]);
				}

				// COmpute change in hidden weights
				double deltaHidden[hiddenNodesNb];
				for (int j = 0; j < hiddenNodesNb; j++) {
					double error = 0.0f;
					for (int k = 0; k < outputNb; k++) {
						error += deltaOutput[k] * outputWeights[j][k];
					}
					deltaHidden[j] = error * dSigmoid(hiddenLayer[j]);
				}

				// Apply change in output weights
				for (int j = 0; j < outputNb; j++) {
					outputLayerBias[j] += deltaOutput[j] * learningRate;
					for (int k = 0; k < hiddenNodesNb; k++) {
						outputWeights[k][j] += hiddenLayer[k] * deltaOutput[j] * learningRate;
					}
				}
				// Apply change hidden weights
				for (int j = 0; j < hiddenNodesNb; j++) {
					hiddenLayerBias[j] += deltaHidden[j] * learningRate;
					for (int k = 0; k < inputNb; k++) {
						hiddenWeights[k][j] +=
							training_inputs[i][k] * deltaHidden[j] * learningRate;
					}
				}
			}
		}
		save_model(hiddenLayer,outputLayer,hiddenLayerBias,outputLayerBias,
				hiddenWeights,outputWeights,path);
}
