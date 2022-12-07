#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "NN.h"
#include "tools.h"
#include "train.h"

void
save(neural_network *nn, char *path)
{
    FILE *f = fopen(path, "wb");
    fwrite(&nn->sizes, sizeof(struct nn_sizes), 1, f);
    fwrite(nn->hidden_weights, sizeof(double), nn->sizes.hidden_weights_count, f);
    fwrite(nn->hidden_bias, sizeof(double), nn->sizes.hidden_bias_count, f);
    fwrite(nn->output_weights, sizeof(double), nn->sizes.output_weights_count, f);
    fwrite(nn->output_bias, sizeof(double), nn->sizes.output_bias_count, f);
    fclose(f);
}

neural_network
load(char *path)
{
    neural_network nn;
    FILE *f = fopen(path, "rb");
    fread(&nn.sizes, sizeof(struct nn_sizes), 1, f);

    nn.hidden_weights = malloc(nn.sizes.hidden_weights_count * sizeof(double));
    nn.hidden_bias = malloc(nn.sizes.hidden_bias_count * sizeof(double));
    nn.output_weights = malloc(nn.sizes.output_weights_count * sizeof(double));
    nn.output_bias = malloc(nn.sizes.output_bias_count * sizeof(double));
    nn.hi

    fread(nn.hidden_weights, sizeof(double), nn.sizes.hidden_weights_count, f);
    fread(nn.hidden_bias, sizeof(double), nn.sizes.hidden_bias_count, f);
    fread(nn.output_weights, sizeof(double), nn.sizes.output_weights_count, f);
    fread(nn.output_bias, sizeof(double), nn.sizes.output_bias_count, f);

    fclose(f);
    return nn;
}

int
main(int argc, char** argv)
{
    if (argc > 10) { exit_usage(); }
    if (strcmp(argv[1], "job") == 0) {
        // job(argv[2], argv[3]);
        printf("I did a job");
    } else if (strcmp(argv[1], "train") == 0) {
        train(argv[2], atoi(argv[3]), argv[4], atoi(argv[5]));
    } else {
        exit_usage();
    }
    return 0;
}