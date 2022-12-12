#include "job.h"

#include "NN.h"
#include "train.h"

#include <stdlib.h>

int
job(neural_network nn, double *input)
{
    struct training travailfamillepatrie = { 0 };
    travailfamillepatrie.nn = nn;
    travailfamillepatrie.hidden_layer =
      malloc(nn.sizes.hidden_count * sizeof(double));
    forward_propagation(&travailfamillepatrie, input, -1);

    size_t max_index = 0;
    double max = travailfamillepatrie.output_layer[0];

    for (size_t i = 1; i < OUTPUT_COUNT; i++) {
        double val = travailfamillepatrie.output_layer[i];
        if (val > max) {
            max = val;
            max_index = i;
        }
    }
    return max_index;
}

int
real_index(char *real, int index)
{
    for (int i = 0; i < 9; ++i) {
        if (real[index + i] == '\001') { return i; }
    }
    return -1;
}

double
fscore(neural_network nn, double *inputs, char *real, size_t len)
{
    double success = 0;
    double errors;
    for (size_t i = 0; i < len; ++i) {
        int res = job(nn, inputs + i * 784);
        int realind = real_index(real, i * 10);
        if (res == realind) { success++; }
    }

    errors = ((double) len) - success;
    double fscoree = success / (success + (errors / 2));
    printf("fscore : %.2f \n", fscoree);

    return fscoree;
}