#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "engine.h"

#define NUM_BLOCKS 4  // Number of blocks in a tetromino

chtype C_BLOCK;
chtype C_GHOST;

typedef struct {
    bool game_over;
    unsigned int score;
    unsigned int lines_cleared;
    unsigned int ms_since_last_fall;  // Time since current piece has dropped one row
    unsigned int pieces;  // Num of pieces played in game
}
Game;

// A tetromino can be represented by the index of its 4 blocs in a 2x2, 3x3 or
// 4x4 matrix.
// For example a T piece is a 3x3 matrix with indices [1, 3, 4, 5] set to true.
// idx |  T
// --- | ---
// 012 | 010
// 345 | 111
// 678 | 000
typedef struct
{
	int blocks[NUM_BLOCKS];  // Indices of each blocks in their matrix
	size_t size;  // Matrix size (2 for O, 4 for I, 3 for everything else)
}
Tetromino;

typedef struct
{
    Tetromino tetromino;
    Position pos;
}
Piece;

int fall_speed_ms(int level);
int get_level(int lines_cleared);
void draw_score(WINDOW* scorewin, Game *game);
void update_score(int *score, int level, int line_cleared);
void init_game(Game *game);

Position tetromino_index_pos(Tetromino *tetromino, int index);
Position piece_index_pos(Piece *piece, int index);

void draw_piece(Piece *piece, WINDOW *window, chtype block, bool additive);
void draw_ghost(Piece *piece, Board *board, WINDOW *window);
bool check_collision(Piece *piece, Board *board);

void rotate_right(Tetromino *t);
void shuffle_queue(Tetromino *queue, size_t size);

void push_next(Piece *next, Tetromino *queue, size_t qsize, int *pieces);
void push_player(Piece *player, Tetromino *tetromino, int board_cols);
void lock(Piece *piece, Board *board);
int clear_complete_rows(Board *board);

