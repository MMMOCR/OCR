#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// Activation function and its derivative
double sigmoid(double x) { return 1 / (1 + exp(-x)); }
double dSigmoid(double x) { return x * (1 — x); }

//Shuffling function allows to suffle the data set and optimize learning speed
void shuffle(int *array,size_t n){
    if(n > 1){
	size_t i;
	for(i = 0; i<n-1; i++){
	    size_t j = i + rand() / (RAND_MAX / (n-i)+1);
	    int t = array[j];
	    array[j] = array[i];
	    array[i] = t;
	}
    }
}
// Init weigths and biases between 0.0 1.0
double init_weight(){ return ((double) rand())/((double) RAND_MAX)}

//XOR Table
//+===+=======+
//|0|0|   0   |
//|1|0|   1   |
//|0|1|   1   |
//|1|1|   0   |
//+=+=+=======+

//We are following this plan for the XOR POC : https://cloud.mickflix.tk/s/3PXSztNwDPSozMi/preview

#DEFINE inputNb 2 // for XOR we only have 2 input for 2 bits
#DEFINE hiddenNodesNb 2 //minimum configuration for XOR
#DEFINE outputNb 1 // for XOR we only have 1 bit of output
#DEFINE trainingSetsNb 4
#DEFINE learningRate 0.1; //The bigger, the faster, the less precise

double hiddenLayer[hiddenNodesNb];
double outputLayer[outputNb];

double hiddenLayerBias[hiddenNodesNb];
double outputLayerBias[outputNb];

double hiddenWeights[numInputs][numHiddenNodes];
double outputWeights[numHiddenNodes][numOutputs];

double training_inputs[trainingSetsNb][inputNb] = { {0.0f,0.0f},{1.0f,0.0f},{0.0f,1.0f},{1.0f,1.0f} };
double training_outputs[trainingSetsNb][inputNb] = { {0.0f},{1.0f},{1.0f},{1.0f,1.0f} };
int
main(void){
    for(int i =0; i<inputNb; i++){
	for(int j = 0; i < hiddenNodesNb ; j++ ){
	    hiddenWeights[i][j] = init_weights();
	}
    }
    for(int i =0; i<hiddenNodesNb; i++){
	for(int j = 0; i < outputNb ; j++ ){
	    outputWeights[i][j] = init_weights();
	}
    }
    for(int i = 0; i < outputNb ; i++ ){
	outputLayerBias[i] = init_weights();
    }

    int trainingSetOrder[] = {0,1,2,3};

    //Train the NN for the defined number of epochs
    for(int epoch = 0; epoch < epochNB; epoch++){
	shuffle(trainingSetOrder, trainingSetsNb);

	for(int x = 0; x< tainingSetsNb; x++){
	    int i = trainingSetOrder[x];

	    //Forward pass
	    //Compute hidden layer activation
	    for(int j = ; j<hiddenNodesNb; j++){
		double activation = hiddenLayerBias[j];

		for(int k = 0 ; k < inputNb; k++){
		    activation += training_inputs[i][k] * hiddenWeights[k][j];
		}
		//#timestamp 33min30
	    }





	}

    }
}
