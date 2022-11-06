#include "save.h"

#include "NN.h"
#include "string.h"

#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void
save_model(double hiddenLayer[],
           double outputLayer[],
           double hiddenLayerBias[],
           double outputLayerBias[],
           double hiddenWeights[][hiddenNodesNb],
           double outputWeights[][outputNb],
           char* path)
{
    (void) hiddenWeights;
    (void) outputLayer;
    (void) outputLayerBias;
    (void) hiddenLayerBias;
    (void) outputWeights;
    FILE* fptr;
    int length = snprintf(NULL, 0, "%ld", time(NULL));
    char* str = malloc(length + 1);
    snprintf(str, length + 1, "%ld", time(NULL));
    fptr = fopen(strcat(path, str), "w");
    free(str);

    if (fptr == NULL) { errx(1, "The file was not created"); }

    // le code pour serializer les layers
    fprintf(fptr, "outputLayer:\n");
    for (size_t i = 0; i < outputNb; i++) {
        fprintf(fptr, "%f ", outputLayer[i]);
    }

    fprintf(fptr, "\n");
    fprintf(fptr, "outputLayerBias:\n");

    for (size_t i = 0; i < outputNb; i++) {
        fprintf(fptr, "%f ", outputLayerBias[i]);
    }

    fprintf(fptr, "\n");
    fprintf(fptr, "outputWeights\n");

    for (size_t i = 0; i < hiddenNodesNb; i++) {
        for (size_t j = 0; j < outputNb; j++){
            fprintf(fptr, "%f ", outputWeights[i][j]);
        }
        fprintf(fptr, "\n");
    }

    fprintf(fptr, "\n");
    fprintf(fptr, "hiddenLayer:\n");

    for (size_t i = 0; i < hiddenNodesNb; i++) {
        fprintf(fptr, "%f ", hiddenLayer[i]);
    }

    fprintf(fptr, "\n");
    fprintf(fptr, "hiddenLayerBias:\n");

    for (size_t i = 0; i < hiddenNodesNb; i++) {
        fprintf(fptr, "%f ", hiddenLayerBias[i]);
    }

    fprintf(fptr, "\n");
    fprintf(fptr, "hiddenWeights:\n");

    for (size_t i = 0; i < inputNb; i++) {
        for (size_t j = 0; j < hiddenNodesNb; j++){
            fprintf(fptr, "%f ", hiddenWeights[i][j]);
        }
        fprintf(fptr, "\n");
    }

    fclose(fptr);
}

void
load_model(char* path)
{
    FILE* file = fopen(path, "r");
    fclose(file);
}
