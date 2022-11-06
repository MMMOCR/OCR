#pragma once

int is_board_valid(int board[]);
int is_solved(int board[]);
int solve(int board[]);

int extract_path(char *path, int *board);
int save_result(char *path, int *board);