//
// Created by Daniel on 12/21/2019.
//

#ifndef EX3_SUDOKU_BOARD_ACTIONS_H
#define EX3_SUDOKU_BOARD_ACTIONS_H

#include "backtracking_core_logic.h"
#include "sudoku_board.h"
#include "puzzle_generator.h"

void print_board(Board* board);
void hint(int x, int y, Board* board);
int set_value_user(int x, int y, int value, Board* board);
void validate_board(Board* board);
void restart(Board* board);
void exit_game(Board* board);

#endif //EX3_