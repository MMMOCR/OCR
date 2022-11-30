// include nessesary libs
#include <math.h>
#include <stdlib.h>
#include <string.h>

// include needed functions in other files
#include "NN.h"
#include "job.h"
#include "tools.h"
#include "train.h"

int
main(int argc, char** argv)
{
    if (argc != 4) { exit_usage(); }
    if (strcmp(argv[1], "job") == 0) {
        job(argv[2], argv[3]);
    } else if (strcmp(argv[1], "train") == 0) {
        train(argv[2], atoi(argv[3]));
    } else {
        exit_usage();
    }
    return 0;
}

//normaliser la couleur des pixels en -1 et 1 car softmax centrer -1 1 donc conv plus vite + eviter explosion de grdient 
