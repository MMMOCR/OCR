#include "tools.h"

#include "functions.h"

#include <err.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
// All tools used by the NN are here with an explication on their use
//

// Shuffling function allows to suffle the data set and optimize learning speed
// this avoid the NN to learn the order in wich the training set is provided
void
shuffle(int *array, size_t n)
{
    if (n > 1) {
        size_t i;
        for (i = 0; i < n - 1; i++) {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            int t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}

// Init weigths and biases between 0.0 1.0 randomly
double
init_weights()
{
    return ((double) rand()) / ((double) RAND_MAX * 2 - 1);
}

// When shit happens display man
void
exit_usage()
{
    errx(EXIT_FAILURE,
         "MAN :\n./NN train <pathToTrainingSet> <Nb of epoch>\n./NN job "
         "<PathTo.png> <PathToParam>");
}

void
delay(int number_of_seconds)
{
    // Converting time into milli_seconds
    int milli_seconds = 1000 * number_of_seconds;
    // Storing start time
    clock_t start_time = clock();

    // looping till required time is not achieved
    while (clock() < start_time + milli_seconds)
        ;
}