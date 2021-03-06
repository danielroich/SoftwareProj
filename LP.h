
#ifndef EX3_LP_H
#define EX3_LP_H

#include "sudoku_board.h"
#include "optional_cell_values.h"

int validate_ILP(Board* board);

/* is_succedded hold 1 after the functions returns if the func secceddedm 0 otherwise */
OptionalCellValues get_value_for_cell(Board* board, int row, int column,int is_integer, int* is_succedded);
int fill_board(Board* board, int is_integer, float threshold);

#endif
