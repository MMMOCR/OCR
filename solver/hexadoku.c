#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

int
is_hexboard_valid(int board[])
{
    int testvertical[273] = { 0 };
    int testsquares[273] = { 0 };
    for (size_t i = 0; i < 16; i++) {
        int testhorizontal[20] = { 0 };
        for (size_t j = 0; j < 16; j++) {
            if (board[i * 16 + j] != -1) {
                if (testvertical[j * 16 + board[i * 16 + j]] == 0) {
                    testvertical[j * 16 + board[i * 16 + j]] = 1;
                } else {
                    return 0;
                }
                if (testhorizontal[board[i * 16 + j]] == 0) {
                    testhorizontal[board[i * 16 + j]] = 1;
                } else {
                    return 0;
                }
                int index;
                if (i < 4) {
                    if (j < 4) {
                        index = 0;
                    } else if (j < 8) {
                        index = 1;
                    } else if (j < 12) {
                        index = 2;
                    } else {
                        index = 3;
                    }
                } else if (i < 8) {
                    if (j < 4) {
                        index = 4;
                    } else if (j < 8) {
                        index = 5;
                    } else if (j < 12) {
                        index = 6;
                    } else {
                        index = 7;
                    }
                } else if (i < 12) {
                    if (j < 4) {
                        index = 8;
                    } else if (j < 8) {
                        index = 9;
                    } else if (j < 12) {
                        index = 10;
                    } else {
                        index = 11;
                    }
                } else {
                    if (j < 4) {
                        index = 12;
                    } else if (j < 8) {
                        index = 13;
                    } else if (j < 12) {
                        index = 14;
                    } else {
                        index = 15;
                    }
                }
                if (testsquares[index * 16 + board[i * 16 + j]] == 0) {
                    testsquares[index * 16 + board[i * 16 + j]] = 1;
                } else {
                    return 0;
                }
            }
        }
    }
    return 1;
}

int
is_hexsolved(int board[])
{
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            if (board[i * 16 + j] == -1) { return 0; }
        }
    }
    return is_hexsolved(board);
}

int
hexsolve(int board[])
{
    if (is_hexsolved(board) == 1) { return 1; }
    for (int i = 0; i < 16; i++) {
        for (int j = 0; j < 16; j++) {
            if (board[i * 16 + j] == -1) {
                while (board[i * 16 + j] < 16) {
                    board[i * 16 + j] += 1;
                    if (is_hexsolved(board) == 1) {
                        if (hexsolve(board) == 1) { return 1; }
                    }
                }
                board[i * 16 + j] = -1;
                return 0;
            }
            if (i == 15 && j == 15 && is_hexsolved(board) == 1) { return 1; }
        }
    }
    return 0;
}

char*
decitohex(int board[])
{
    int blen = 256;
    char* board2 = calloc(256, sizeof(char));
    for (int i = 0; i < blen; i++) {
        if (board[i] < 10) {
            board2[i] = board[i] + '0';
        } else if (board[i] == 10) {
            board2[i] = 'A';
        } else if (board[i] == 11) {
            board2[i] = 'B';
        } else if (board[i] == 12) {
            board2[i] = 'C';
        } else if (board[i] == 13) {
            board2[i] = 'D';
        } else if (board[i] == 14) {
            board2[i] = 'E';
        } else if (board[i] == 15) {
            board2[i] = 'F';
        }
    }
    return board2;
}