#include "game.h"

chtype C_BLOCK = ' '|A_REVERSE|COLOR_PAIR(PAIR_BLACK_WHITE);
chtype C_GHOST = ':'|COLOR_PAIR(PAIR_BLACK_WHITE);

// Calculate fall speed of given level (using the marathon speed curve) in ms
int fall_speed_ms(int level)
{
    double seconds = pow(0.8 - (level - 1) * 0.007, level - 1);
    int ms = seconds * 1000;
    return ms > 0 ? ms : 1;
}

void init_game(Game *game)
{
    game->game_over = false;
    game->score = 0;
    game->level = 1;
    game->ms_since_last_fall = 0;
    game->pieces = 0;
}

Position tetromino_index_pos(Tetromino* tetromino, int index)
{
    Position pos = {
        tetromino->blocks[index] / tetromino->size,
        tetromino->blocks[index] % tetromino->size
    };
    return pos;
}

Position piece_index_pos(Piece* piece, int index)
{
    Position pos = tetromino_index_pos(&piece->tetromino, index);
    pos.y += piece->pos.y;
    pos.x += piece->pos.x;
    return pos;
}

void draw_piece(Piece* piece, WINDOW* window, chtype block, bool additive)
{
    Board *board = init_board(NUM_BLOCKS, NUM_BLOCKS);

    for (int i = 0; i < NUM_BLOCKS; i++) {
        Position pos = tetromino_index_pos(&piece->tetromino, i);

        // Block is above the board
        if (pos.y + piece->pos.y < 0) {
            continue;
        }

        board->matrix[pos.y][pos.x] = true;
    }

    draw_board(board, window, piece->pos, additive, block);
    free_board(board);
}

void draw_ghost(Piece *piece, Board *board, WINDOW *window)
{
    int distance = board->rows;

    for (int i = 0; i < NUM_BLOCKS; i++) {
        Position pos = piece_index_pos(piece, i);
        bool collide = false;

        // Find the closest block bellow this piece
        for (int y = pos.y; y < board->rows; y++) {
            if (board->matrix[y][pos.x]) {
                int new_distance = y - 1 - pos.y;
                distance = new_distance < distance ? new_distance : distance;
                bool collide = true;
            }
        }

        // No blocks, we collide with the bottom of the board
        if (!collide) {
            int new_distance = board->rows - 1 - pos.y;
            distance = new_distance < distance ? new_distance : distance;
        }
    }

    Piece ghost_piece;
    ghost_piece.tetromino = piece->tetromino;
    ghost_piece.pos.y = piece->pos.y + distance;
    ghost_piece.pos.x = piece->pos.x;
    draw_piece(&ghost_piece, window, C_GHOST, true);
}

bool check_collision(Piece *piece, Board *board)
{
    Position pos;
    for (int i = 0; i < NUM_BLOCKS; i++) {
        pos = piece_index_pos(piece, i);

        // Order matter to not read out of the array
        if (   pos.x >= board->cols
            || pos.x < 0
            || pos.y >= board->rows
            || board->matrix[pos.y][pos.x]
        ) {
            return true;
        }
    }
    return false;
}

// Rotate tetromino 90 degree clockwise
void rotate_right(Tetromino *t)
{
    for (int i = 0; i < NUM_BLOCKS; i++) {
        int y = t->blocks[i] / t->size;
        int x = t->blocks[i] % t->size;
        t->blocks[i] = t->size - 1 - y + (x * t->size);
    }
}

// Shuffle the bag of tetrominos as suggested by the tetris guidelines
void shuffle_queue(Tetromino *queue, size_t size)
{
    Tetromino t;
    size_t j;
    for (size_t i = 0; i < size; i++) {
        j = rand() % size;
        t = queue[j];
        queue[j] = queue[i];
        queue[i] = t;
    }
}

// Push the next tetromino in queue to the next piece and increment queue piece
void push_next(Piece *next, Tetromino *queue, size_t qsize, int *pieces)
{
    // Queue is considered empty if current piece index is the last of the queue
    if (*pieces % qsize == qsize) {
        shuffle_queue(queue, qsize);
    }

    Position pos = {0, 0};
    next->pos = pos;
    next->tetromino = queue[((*pieces) + 1) % qsize];

    (*pieces)++;
}

// Set a new tetromino as player piece and reset player position
void push_player(Piece *player, Tetromino *tetromino, int board_cols)
{
    player->tetromino = *tetromino;

    player->pos.y = 0;
    player->pos.x = board_cols / 2 - player->tetromino.size / 2;

    if (player->tetromino.size == 3 && player->tetromino.blocks[0] > 2) {
        player->pos.y--;
    }

    if (player->tetromino.size % 2) {
        player->pos.x--;
    }
}

// Add piece to the board
void lock(Piece *piece, Board *board)
{
    Position pos;
    for (int i = 0; i < NUM_BLOCKS; i++) {
        pos = piece_index_pos(piece, i);
        board->matrix[pos.y][pos.x] = true;
    }
}

int clear_complete_rows(Board *board)
{
    int count = 0;
    for (int y = 0; y < board->rows; y++) {

        // A row is considered completed if all columns items are true
        bool is_complete = true;
        for (int x = 0; x < board->cols; x++) {
            if (!board->matrix[y][x]) {
                is_complete = false;
                break;
            }
        }

        // When a row is completed, lower all aboves rows by 1 level
        if (is_complete) {
            for (int cy = y; cy > 0; cy--) {
                for (int cx = 0; cx < board->cols; cx++) {
                    if (cy > 0) {
                        board->matrix[cy][cx] = board->matrix[cy - 1][cx];
                    } else {
                        board->matrix[cy][cx] = false;
                    }
                }
            }
            count++;
        }
    }
    return count;
}
