#include "job.h"

#include "NN.h"
#include "train.h"

#include <stdlib.h>

int
job(neural_network nn, double *input)
{
    struct training travailfamillepatrie = { 0 };
    travailfamillepatrie.nn = nn;
    printf("hidden size: %f\n",nn.sizes.hidden_count);
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
    printf(" Output : [%f,%f,%f,%f,%f,%f,%f,%f,%f,%f]",
                       travailfamillepatrie.output_layer[0], travailfamillepatrie.output_layer[1], travailfamillepatrie.output_layer[2],
                       travailfamillepatrie.output_layer[3], travailfamillepatrie.output_layer[4], travailfamillepatrie.output_layer[5],
                       travailfamillepatrie.output_layer[6], travailfamillepatrie.output_layer[7], travailfamillepatrie.output_layer[8],
                       travailfamillepatrie.output_layer[9]);
    printf("result : %lu\n",max_index);
    return max_index;
}

double
fscore(neural_network nn, double **inputs, int *real, size_t len)
{
    int success = 0;
    int errors;

    for (size_t i = 0; i < len; ++i) {
        if (job(nn, inputs[i]) == real[i]) success++;
    }

    errors = len - success;
    return (success / (success + (errors / 2)));
}