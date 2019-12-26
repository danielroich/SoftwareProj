#include <stdio.h>
#include "parser.h"

int main() {
    /*seed?*/ 
    char* command = malloc((sizeof(char)*1024)); 
    Board* board = malloc((sizeof(Board)));
    create_empty_board(board,3,3);
    restart(board);
    print_board(board);
    if(feof(stdin)){exit_game(board);}
    while(1==1){
        fgets(command,1024,stdin);
        parser(command);
        if(feof(stdin)){exit_game(board);}
    }
    return 0;
}