#include "save.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include "NN.h"
#include "string.h"
#include <time.h>
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
    (void)hiddenWeights;
    (void)outputLayer;
    (void)outputLayerBias;
    (void)hidddenLayerBias;
    (void)outputWeights;
    FILE* fptr;
    int length = snprintf( NULL, 0, "%ld", time(NULL) );
    char* str = malloc( length + 1 );
    snprintf( str, length + 1, "%ld", time(NULL) );
    fptr = fopen(strcat(path, str), "w");
    free(str);

    if (fptr == NULL) { errx(1, "The file was not created"); }

    // le code pour serializer la hidden layer

    for (size_t i = 0; i < hiddenNodesNb; i++) {
        fprintf(fptr, "%f ", hiddenLayer[i]);
    }
    fclose(fptr);
}

void
load_model(char* path)
{
    FILE *file = fopen(path, "r");
    fclose(file);
}
