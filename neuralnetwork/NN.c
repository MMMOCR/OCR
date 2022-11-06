// include nessesary libs
#include <math.h>
#include <string.h>

// include needed functions in other files
#include "NN.h"
#include "job.h"
#include "tools.h"
#include "train.h"

int
main(int argc, char** argv)
{
    if (argc != 3 && argc != 4) { exit_usage(); }
    if (strcmp(argv[1], "job") == 0) { job(); }
    if (strcmp(argv[1], "train") == 0) {
        train(argv[3]);
    } else {
        exit_usage();
    }
}
