#include "NN.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE >= 200809L
#endif

size_t
line_number(const char *f, size_t len)
{
    size_t k = 0;
    for (const char *i = f; i < f + len; i++) {
        if (*i == 0x0a) { k++; }
    }

    return k;
}

size_t
count_comas(const char *line)
{
    size_t k = 1;
    for (const char *i = line; *i != 0; i++) {
        if (*i == 0x2c) { k++; }
    }

    return k;
}

void
get_nth_coma(const char *line, size_t n, char **dest)
{
    const char *i = line, *j;
    for (; n > 0; ++i) {
        if (*i == 0x2c) { --n; }
    }
    for (j = i; *j != 0 && *j != 0x2c; j++) {}
    *dest = calloc(j - i + 1, sizeof(char));
    memcpy(*dest, i, j - i);
    (*dest)[j - i] = 0;
}

int
dostuff(const char *path,
        double **train_inputs,
        char **train_outputs,
        size_t *inputNb,
        size_t *trainingSetsNb)
{
    FILE *fd;
    char *line = NULL, *file = NULL;
    // size_t rsize;
    size_t len_file, comas, linenumber, len_line = 0, k = 0, l = 0;
    ssize_t readbytes;
    char *cur;

    // open file
    if ((fd = fopen(path, "r")) == NULL) { return 1; }

    // get file size
    fseek(fd, 0, SEEK_END);
    len_file = ftell(fd);
    fseek(fd, 0, SEEK_SET);

    // create buff for file
    if ((file = calloc(len_file, sizeof(char))) == NULL) { return 1; };

    // read whole file in buff and get training set number
    ftell(fd);
    fread(file, sizeof(char), len_file, fd);
    linenumber = line_number(file, len_file);
    free(file);
    fseek(fd, 0, SEEK_SET);
    ftell(fd);

    // read first line to get input number
    readbytes = getline(&line, &len_line, fd);
    if (!readbytes) { return 1; }

    // get input number
    comas = count_comas(line);
    free(line);
    line = NULL;

    *inputNb = comas - 2;
    *trainingSetsNb = linenumber - 1;

    // define arrays of good size
    if (((*train_inputs) =
           calloc(*trainingSetsNb * *inputNb, sizeof(double))) == NULL) {
        printf("train_inputs failed\n");
        return 1;
    };
    if (((*train_outputs) = calloc(*trainingSetsNb * OUTPUT_COUNT, sizeof(char))) ==
        NULL) {
        printf("train_outputs failed\n");
        return 1;
    };

    // read each line

    while ((readbytes = getline(&line, &len_line, fd)) != -1) {
        l = 0;
        get_nth_coma(line, 1, &cur);
        (*train_outputs)[k * OUTPUT_COUNT + (char) atoi(cur)] = 1;
        free(cur);
        for (size_t i = 2; i < comas; i++, l++) {
            get_nth_coma(line, i, &cur);
            (*train_inputs)[k * *inputNb + l] = (double) ((atof(cur)) / 255);
            free(cur);
        }

        free(line);
        line = NULL;
        len_line = 0;
        k++;
    }

    fclose(fd);
    return 0;
}

void
print_char_array(char *a, size_t lines, size_t columns)
{
    for (size_t i = 0; i < lines; ++i) {
        printf("[");
        for (size_t j = 0; j < columns; ++j)
            printf("%hhi", a[columns * i + j]);
        printf("]\n");
    }
}

void
print_double_array(double *a, size_t lines, size_t columns)
{
    for (size_t i = 0; i < lines; ++i) {
        printf("[");
        for (size_t j = 0; j < columns; ++j) {
            printf("%f ", a[columns * i + j]);
        }
        printf("]\n");
    }
}
/*
int main() {
    double * training_inputs;
    char * training_outputs;
    size_t inputNb, trainingSetsNb;

    int k = dostuff("test.txt", &training_inputs, &training_outputs, &inputNb,
&trainingSetsNb); print_double_array(training_inputs, trainingSetsNb, inputNb);
    print_char_array(training_outputs, trainingSetsNb, OUTPUT_COUNT);
    free(training_inputs);
    free(training_outputs);
    return k;
}
*/