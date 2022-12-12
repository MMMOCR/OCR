#include "NN.h"

#include "../utils/empty_cell.h"
#include "job.h"
#include "tools.h"
#include "train.h"

#include <dirent.h>
#include <libgen.h>
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
    int sudoku[9][9];
    for (size_t i = 0; i < 9; i++) {
        for (size_t j = 0; j < 9; j++) {
            sudoku[i][j] = 0;
        }
    }

    if (argc < 3) {
        exit_usage();
    }

    else if (strcmp(argv[1], "grid") == 0) {
        char p[128];
        strcpy(p, argv[0]);
        dirname(p);

        char pa[64] = { '/', 'c', '2', '_', 0, '_', 0, '.', 'p', 'n', 'g' };
        char pa2[64] = { '/', 'c', '_', 0, '_', 0, '.', 'p', 'n', 'g' };

        for (size_t i = 0; i < 9; i++) {
            for (size_t j = 0; j < 9; j++) {
                char pp[128];
                strcpy(pp, p);
                pa[4] = i + 0x30;
                pa[6] = j + 0x30;
                strcat(pp, "/images/detect");
                strcat(pp, pa);
                //printf("%s\n", pp);

                SDL_Surface *ss = IMG_Load(pp);
                if (!is_empty(ss)) {
                    strcpy(pp, p);
                    pa2[3] = i + 0x30;
                    pa2[5] = j + 0x30;
                    strcat(pp, "/images/detect");
                    strcat(pp, pa2);
                    SDL_Surface *sss = IMG_Load(pp);
                    double *array = PicToList(sss);

                    int res = job(load(argv[2]), array);

                    if (res == 0) { res = 8; }
                    sudoku[i][j] = res;
                    // return res;
                }
            }
        }

        FILE *f = fopen(argv[3], "wb");
        int count = 0;
        for (size_t i = 0; i < 9; i++) {
            for (size_t j = 0; j < 9; j++) {
                if (sudoku[i][j] == 0) {
                    fprintf(f, ".");
                } else {
                    fprintf(f, "%d", sudoku[i][j]);
                }
                if (j == 2 || j == 5) { fprintf(f, " "); }
            }
            fprintf(f, "\n");
        }

        fclose(f);
    }

    else if (strcmp(argv[1], "job") == 0) {

        if (argc != 5) { exit_usage(); }

        SDL_Surface *img = IMG_Load(argv[3]);
        double *array = PicToList(img);

        int res = job(load(argv[2]), array);
        if (res == 0) { res = 8; }

        return res;

    }

    else if (strcmp(argv[1], "train") == 0) {
        if (argc != 6) { exit_usage(); }
        train(argv[2], atoi(argv[3]), argv[4], atoi(argv[5]));
    }

    else {
        exit_usage();
    }
    return 0;
}