#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ncurses.h>

#define WIN_SCALE 2  // Width scaling value to square tetrominos
#define PAIR_BLACK_WHITE 1  // Index of white foreground to black background color pair

// 2d coordinate
typedef struct
{
	int y;
	int x;
}
Position;

// 2d board
typedef struct {
    unsigned int rows;
    unsigned int cols;
    bool **matrix;
}
Board;

void init_ncurses(void);

WINDOW* init_boardwin(int rows, int cols);
WINDOW* init_nextwin(WINDOW *boardwin, int size);
WINDOW* init_holdwin(WINDOW *boardwin, int size);
WINDOW* init_scorewin(WINDOW* nextwin);

void sleep_ms(int milliseconds);

Board* init_board(int rows, int cols);
void free_board(Board *board);

void draw_board(Board *board, WINDOW *window, Position start, bool additive, chtype block);
void draw_block(Position pos, WINDOW *window, chtype block);
