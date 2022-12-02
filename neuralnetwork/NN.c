// include nessesary libs
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// include needed functions in other files
#include "NN.h"
#include "job.h"
#include "tools.h"
#include "train.h"

int
main(int argc, char** argv)
{
    if (argc > 10) { exit_usage(); }
    if (strcmp(argv[1], "job") == 0) {
        //job(argv[2], argv[3]);
        printf("I did a job");
    } else if (strcmp(argv[1], "train") == 0) {
        train(argv[2],atoi(argv[3]), argv[4], atoi(argv[5]));
    } else {
        exit_usage();
    }
    return 0;
}

//normaliser la couleur des pixels en -1 et 1 car softmax centrer -1 1 donc conv plus vite + eviter explosion de grdient 
