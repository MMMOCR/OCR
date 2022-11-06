#include "utils/imageutils.h"

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void
help(char *name)
{
    printf("Usage: %s [OPTION...]\n", name);
    printf("\t -f, --file <path> \t the path of the image\n");
}

int
main(int argc, char *argv[])
{
    if (argc < 2 || strcmp(argv[1], "--help") == 0) {
        help(argv[0]);
        return 0;
    }
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-f") == 0 || strcmp(argv[i], "--file")) {
            i++;
            if (i > argc) {
                help(argv[0]);
                return 0;
            }
            char *filepath = argv[i];
            if (fopen(filepath, "r")) {
                image_utils(filepath);
            } else {
                errx(1, "File does not exists");
                return 0;
            }
            continue;
        }
    }
    return 0;
}
