
#ifndef SOFTWAREPROJ_SODUKO_BOARD_H
#define SOFTWAREPROJ_SODUKO_BOARD_H

int const BOARD_NULL_VALUE = 0;

typedef struct _Board {
    int num_of_rows;
    int num_of_columns;
    int size;
    int** solved_board;
    int** fixed_board;
    int** cur_board;
    int count_filled;
} Board;

int set_value(int x, int y, int value, Board* board);
int get_value(int x, int y, Board* board);

#endif 
