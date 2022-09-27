#include <math.h>
#include <stdio.h>
#include <stdlib.h>

// Activation function and its derivative
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

// Shuffling function allows to suffle the data set and optimize learning speed
void
shuffle(int *array, size_t n)
{
  if (n > 1) {
    size_t i;
    for (i = 0; i < n - 1; i++) {
      size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
      int t = array[j];
      array[j] = array[i];
      array[i] = t;
    }
  }
}
// Init weigths and biases between 0.0 1.0
double
init_weights()
{
  return ((double) rand()) / ((double) RAND_MAX);
}

// XOR Table
//+===+=======+
//|0|0|   0   |
//|1|0|   1   |
//|0|1|   1   |
//|1|1|   0   |
//+=+=+=======+

// We are following this plan for the XOR POC :
// https://cloud.mickflix.tk/s/3PXSztNwDPSozMi/preview

#define inputNb 2 // for XOR we only have 2 input for 2 bits
#define hiddenNodesNb 2 // minimum configuration for XOR
#define outputNb 1 // for XOR we only have 1 bit of output
#define trainingSetsNb 4
#define learningRate 0.1 // The bigger, the faster, the less precise
#define epochNb 1000000

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
int
main(void)
{
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
}
