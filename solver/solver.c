#include "solver.h"

#include <err.h>
#include <stddef.h>
#include <stdio.h>

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

int*
solver(int board[])
{
    solve(board);
    // for (size_t i = 0; i < 80; i++)
    // {
    // 	board[i] += 0x30;
    // }
    return board;
}

int
main(int argc, char* argv[])
{
    if (argc != 2) errx(1, "suce\n");
    int board2[] = { 4, 0, 8, 0, 7, 0, 0, 5, 2, 0, 0, 0, 9, 0, 0, 1, 0,
                     7, 0, 0, 6, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 5, 0, 0,
                     0, 6, 5, 0, 4, 0, 0, 0, 0, 0, 8, 0, 8, 0, 2, 0, 7,
                     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 9, 0, 0, 0, 0,
                     0, 3, 0, 0, 0, 2, 1, 3, 0, 5, 9, 0, 0 };
    int board[] = { 0, 0, 0, 5, 0, 0, 0, 4, 8, 6, 0, 2, 0, 9, 4, 0, 0,
                    0, 0, 4, 5, 0, 0, 0, 6, 0, 0, 0, 7, 4, 9, 3, 8, 1,
                    6, 0, 8, 2, 1, 6, 4, 0, 3, 5, 0, 9, 0, 0, 0, 0, 0,
                    8, 7, 0, 2, 0, 0, 0, 0, 0, 4, 0, 0, 0, 1, 0, 0, 8,
                    5, 0, 0, 6, 0, 0, 0, 4, 7, 0, 9, 0, 1 };
    printf("%ls\n", solver(board2));
    for (size_t i = 0; i < 81; i++) {
        printf("%i", board2[i]);
    }

    return 0;
}