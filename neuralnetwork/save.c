#include "save.h"

#include "NN.h"
#include "string.h"

#include <err.h>

void
save_model(double hiddenLayer[],
           double outputLayer[],
           double hidddenLayerBias[],
           double outputLayerBias[],
           double hiddenWeights[][hiddenNodesNb],
           double outputWeights[][outputNb],
           char* path)
{
    FILE* fptr;

    ftpr = fopen(strcat(path, hiddenLayer), "w");

    if (fptr == NULL) { errx(EXIT_FAILURE, "The file was not created"); }

    // le code pour serializer la hidden layer

    for (size_t i = 0, i < hiddenNodesNb; i++) {
        fprintf(fptr, "%d ", hiddenLayer[i]);
    }
    fclose(fptr);
    return 0;
}

void
load_model(char* path)
{
}
