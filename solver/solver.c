#include "solver.h"

#include <err.h>
#include <stddef.h>
#include <stdio.h>

int
is_board_valid(char board[])
{
    char testvertical[90] = { 0 };
    char testsquares[90] = { 0 };
    for (size_t i = 0; i < 9; i++) {
	char testhorizontal[10] = { 0 };
	for (size_t j = 0; j < 9; j++) {
	    if (board[i * 9 + j] != 0) {
		if (testvertical[j * 9 + (int) (board[i * 9 + j] - 48)] == 0) {
		    testvertical[j * 9 + (int) (board[i * 9 + j] - 48)] = 1;
		} else {
		    return 0;
		}
		if (testhorizontal[(int) (board[i * 9 + j] - 48)] == 0) {
		    testhorizontal[(int) (board[i * 9 + j] - 48)] = 1;
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

		if (testsquares[index * 9 + (int) (board[i * 9 + j] - 48)] ==
		    0) {
		    testsquares[index * 9 + (int) (board[i * 9 + j] - 48)] = 1;
		} else {
		    return 0;
		}
	    }
	}
    }
    return 1;
}

int
is_solved(char board[])
{
    for (int i = 0; i < 9; i++) {
	for (int j = 0; j < 9; j++) {
	    if (board[i * 9 + j] == 0) { return 0; }
	}
    }
    return is_board_valid(board);
}

int
solve(char board[])
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

char*
solver(char board[])
{
    solve(board);
    return board;
}

int
main(int argc, char* argv[])
{
    if (argc != 2) errx(1, " ");
    printf("%s\n", solver(argv[1]));
    return 0;
}