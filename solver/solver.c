#include "solver.h"

#include <err.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 9
#define HEIGHT 9

int
is_board_valid(int board[])
{
    int testvertical[90] = { 0 };
    int testsquares[90] = { 0 };
    for (size_t i = 0; i < 9; i++) {
        int testhorizontal[10] = { 0 };
        for (size_t j = 0; j < 9; j++) {
            if (board[i * 9 + j] != 0) {
                if (testvertical[j * 9 + board[i * 9 + j]] == 0) {
                    testvertical[j * 9 + board[i * 9 + j]] = 1;
                } else {
                    return 0;
                }
                if (testhorizontal[board[i * 9 + j]] == 0) {
                    testhorizontal[board[i * 9 + j]] = 1;
                } else {
                    return 0;
                }

                int index;
                if (i < 3) {
                    if (j < 3) {
                        index = 0;
                    } else if (j >= 6) {
                        index = 2;
                    } else {
                        index = 1;
                    }
                } else if (i >= 6) {
                    if (j < 3) {
                        index = 3;
                    } else if (j >= 6) {
                        index = 5;
                    } else {
                        index = 4;
                    }
                } else {
                    if (j < 3) {
                        index = 6;
                    } else if (j >= 6) {
                        index = 8;
                    } else {
                        index = 7;
                    }
                }

                if (testsquares[index * 9 + board[i * 9 + j]] == 0) {
                    testsquares[index * 9 + board[i * 9 + j]] = 1;
                } else {
                    return 0;
                }
            }
        }
    }
    return 1;
}

int
is_solved(int board[])
{
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (board[i * 9 + j] == 0) { return 0; }
        }
    }
    return is_board_valid(board);
}

int
solve(int board[])
{
    if (is_solved(board) == 1) { return 1; }
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            if (board[i * 9 + j] == 0) {
                while (board[i * 9 + j] < 9) {
                    board[i * 9 + j] += 1;
                    if (is_board_valid(board) == 1) {
                        if (solve(board) == 1) { return 1; }
                    }
                }
                board[i * 9 + j] = 0;
                return 0;
            }
            if (i == 8 && j == 8 && is_solved(board) == 1) { return 1; }
        }
    }
    return 0;
}

int
extract_path(char *path, int *board)
{
    char *line;
    size_t rsize;
    ssize_t readbytes;
    int index;
    FILE *file;

    if ((file = fopen(path, "r")) == NULL) { return 0; }

    index = 0;
    readbytes = 0;
    while ((readbytes = getline(&line, &rsize, file)) != -1) {
        for (ssize_t i = 0; i < readbytes; i++) {
            char c = line[i];
            if (c == '.') {
                board[index++] = 0;
            } else if (c >= '1' && c <= '9') {
                board[index++] = c - '0';
            }
        }
    }

    return 1;
}

int
save_result(char *path, int *board)
{
    FILE *file;
    char *savepath;

    savepath = malloc(strlen(path) + 7);
    strcpy(savepath, path);
    strcat(savepath, ".result");

    if ((file = fopen(savepath, "w")) == NULL) { return 0; }

    for (int i = 0; i < WIDTH * HEIGHT; ++i) {
        if (i > 0)
            if (i % 9 == 0) {
                fprintf(file, "\n");
            } else if (i % 3 == 0) {
                fprintf(file, " ");
            }

        fprintf(file, "%d", board[i]);
    }

    fprintf(file, "\n");
    fclose(file);
    return 1;
}

int
main(int argc, char *argv[])
{
    if (argc != 2) errx(1, "filename\n");
    int *board = malloc(sizeof(int) * (WIDTH * HEIGHT));

    if (!extract_path(argv[1], board)) errx(1, "cannot open file\n");
    if (!solve(board)) errx(1, "cannot solve file\n");
    if (!save_result(argv[1], board)) errx(1, "cannot save file\n");

    free(board);
    return 0;
}