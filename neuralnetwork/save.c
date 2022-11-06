#include "save.h"

#include "NN.h"
#include "string.h"

#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
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
    FILE* fptr;
    int length = snprintf(NULL, 0, "%ld", time(NULL));
    char* str = malloc(length + 1);
    snprintf(str, length + 1, "%ld", time(NULL));
    fptr = fopen(strcat(path, str), "w");
    free(str);

    if (fptr == NULL) { errx(1, "The file was not created"); }

    // le code pour serializer les layers
    for (size_t i = 0; i < outputNb; i++) {
        fprintf(fptr, "%f\n", outputLayer[i]);
    }

    for (size_t i = 0; i < outputNb; i++) {
        fprintf(fptr, "%f\n", outputLayerBias[i]);
    }

    for (size_t i = 0; i < hiddenNodesNb; i++) {
        for (size_t j = 0; j < outputNb; j++) {
            fprintf(fptr, "%f\n", outputWeights[i][j]);
        }
    }

    for (size_t i = 0; i < hiddenNodesNb; i++) {
        fprintf(fptr, "%f\n", hiddenLayer[i]);
    }

    for (size_t i = 0; i < hiddenNodesNb; i++) {
        fprintf(fptr, "%f\n", hiddenLayerBias[i]);
    }

    for (size_t i = 0; i < inputNb; i++) {
        for (size_t j = 0; j < hiddenNodesNb; j++) {
            fprintf(fptr, "%f\n", hiddenWeights[i][j]);
        }
    }

    fclose(fptr);
}

void
load_model(double hiddenLayer[],
           double outputLayer[],
           double hiddenLayerBias[],
           double outputLayerBias[],
           double* hiddenWeights,
           double* outputWeights,
           char* path)
{
    FILE* file = fopen(path, "r");

    load_array(file, outputLayer, outputNb);
    load_array(file, outputLayerBias, outputNb);
    load_2darray(file, outputWeights, outputNb, hiddenNodesNb);

    load_array(file, hiddenLayer, hiddenNodesNb);
    load_array(file, hiddenLayerBias, hiddenNodesNb);
    load_2darray(file, hiddenWeights, hiddenNodesNb, inputNb);
    fclose(file);
}

void
load_array(FILE* file, double array[], size_t len)
{
    char* line = NULL;
    size_t slen = 0;
    ssize_t read;
    for (size_t i = 0; i < len; i++) {
        read = getline(&line, &slen, file);
        if (read != -1) {
            array[i] = (double) atoi(line);
        } else {
            errx(EXIT_FAILURE, "the model you are trying to load is no good");
        }
    }
}
void
load_2darray(FILE* file, double*array, size_t x, size_t y)
{
    char* line = NULL;
    size_t len = 0;
    ssize_t read;
    for (size_t i = 0; i < y; i++) {
        for (size_t j = 0; j < x; j++) {
            read = getline(&line, &len, file);
            if (read != -1) {
                printf("%lu,%lu\n", i, j);
                array[i * x + j] = (double) atoi(line);
            } else {
                errx(EXIT_FAILURE,
                     "the model you are trying to load is no good");
            }
        }
    }
}
