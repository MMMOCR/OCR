#pragma once
#include "NN.h"

void
save_model(double hiddenLayer[],
           double outputLayer[],
           double hidddenLayerBias[],
           double outputLayerBias[],
           double hiddenWeights[][hiddenNodesNb],
           double outputWeights[][outputNb]);
