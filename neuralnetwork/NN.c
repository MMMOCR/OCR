#include "NN.h"

#include "job.h"
#include "tools.h"
#include "train.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
save(neural_network *nn, char *path)
{
    FILE *f = fopen(path, "wb");
    fwrite(&nn->sizes, sizeof(struct nn_sizes), 1, f);
    fwrite(nn->hidden_weights, sizeof(double), nn->sizes.hidden_weights_count,
           f);
    fwrite(nn->hidden_bias, sizeof(double), nn->sizes.hidden_bias_count, f);
    fwrite(nn->output_weights, sizeof(double), nn->sizes.output_weights_count,
           f);
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

    fread(nn.hidden_weights, sizeof(double), nn.sizes.hidden_weights_count, f);
    fread(nn.hidden_bias, sizeof(double), nn.sizes.hidden_bias_count, f);
    fread(nn.output_weights, sizeof(double), nn.sizes.output_weights_count, f);
    fread(nn.output_bias, sizeof(double), nn.sizes.output_bias_count, f);

    fclose(f);
    return nn;
}

int
main(int argc, char **argv)
{
    if (argc < 3) { exit_usage(); }

    if (strcmp(argv[1], "job") == 0) {
        if (argc != 4) { exit_usage(); }
        SDL_Surface *img = IMG_Load(argv[3]);
        double *array = PicToList(img);

        job(load(argv[2]), array);
    } else if (strcmp(argv[1], "train") == 0) {
        if (argc != 6) { exit_usage(); }
        train(argv[2], atoi(argv[3]), argv[4], atoi(argv[5]));
    } else {
        exit_usage();
    }

    //    double les[] =
    //    {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,152,242,255,255,255,255,255,255,255,234,136,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,152,197,101,74,74,74,74,74,74,68,39,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,152,182,45,8,8,8,8,8,8,7,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,152,180,38,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,152,180,38,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,152,180,38,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,152,180,38,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,152,181,42,5,5,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,152,216,166,150,150,146,131,96,44,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,88,141,148,148,148,150,157,168,154,85,7,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,16,26,27,27,27,31,49,93,157,182,79,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,6,54,161,185,23,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,76,218,81,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,192,137,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,183,146,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,5,60,209,111,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,7,0,0,43,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,7,8,8,8,8,13,37,114,208,131,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,33,69,74,74,74,80,112,179,219,141,10,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,116,237,255,255,255,248,221,159,71,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    //    for (int i = 0; i < 784; ++i) {
    //        les[i] /= 255;
    //    }

    // job(load(argv[2]), PicToList(IMG_Load("neuralnetwork/numbers/4.png")));
    return 0;
}