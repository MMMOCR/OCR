#pragma once
#include "NN.h"

void
save_model(double hiddenLayer[],
           double outputLayer[],
           double hiddenLayerBias[],
           double outputLayerBias[],
           double hiddenWeights[][hiddenNodesNb],
           double outputWeights[][outputNb],
           char *filename);
