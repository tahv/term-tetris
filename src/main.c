#include <stdlib.h>
#include "game.h"

int main(void)
{
    srand(time(NULL));
    init_ncurses();

    Tetromino queue[] = {
        {{4, 5, 6, 7}, 4},  // I
        {{0, 1, 2, 3}, 2},  // O
        {{1, 3, 4, 5}, 3},  // T
        {{4, 5, 6, 7}, 3},  // S
        {{3, 4, 7, 8}, 3},  // Z
        {{0, 3, 4, 5}, 3},  // J
        {{2, 3, 4, 5}, 3},  // L
    };
    const int QUEUE_SIZE = sizeof(queue) / sizeof(*queue);
    shuffle_queue(queue, QUEUE_SIZE);

    Game game;
    init_game(&game);

    Board *game_board = init_board(20, 10);

    WINDOW *game_win = init_boardwin(game_board->rows, game_board->cols);
    WINDOW *next_win = init_nextwin(game_win, NUM_BLOCKS);
    WINDOW *hold_win = init_holdwin(game_win, NUM_BLOCKS);
    WINDOW *score_win = init_scorewin(next_win);

    // Init player first piece
    Piece player;
    push_player(&player, &queue[0], game_board->cols);

    Piece hold;
    hold.tetromino.size = 0;
    hold.pos.x = 0;
    hold.pos.y = 0;
    bool can_hold = true;

    // Init next piece
    Piece next;
    push_next(&next, queue, QUEUE_SIZE, &game.pieces);
    draw_piece(&next, next_win, C_BLOCK, false);
    wrefresh(next_win);

    bool dropped = false;

    while (!game.game_over) {
        Piece new_pos;
        new_pos.tetromino = player.tetromino;
        new_pos.pos = player.pos;

        // Falling phase

        bool collide = false;

        // the tetrimino will fall at its normal fall Speed whether or
        // not it is being manipulated by the player.
        int level = get_level(game.lines_cleared);
        int fall_speed = fall_speed_ms(level);
        if (game.ms_since_last_fall >= fall_speed) {
            game.ms_since_last_fall = 0;
            new_pos.pos.y++;
            collide = check_collision(&new_pos, game_board);
            if (!collide) {
                player.pos.y = new_pos.pos.y;
            }
        }

        // We only consider player movement if the fall has not locked the piece
        // on the board. This is because it is way harder to handle collision
        // resolution if we travel more than one unit in a single frame.
        if (!collide) {
            switch(getch()) {
                case KEY_LEFT:
                    new_pos.pos.x--;
                    collide = check_collision(&new_pos, game_board);
                    break;
                case KEY_RIGHT:
                    new_pos.pos.x++;
                    collide = check_collision(&new_pos, game_board);
                    break;
                case KEY_DOWN:
                    game.ms_since_last_fall = 0;
                    new_pos.pos.y++;
                    collide = check_collision(&new_pos, game_board);
                    break;
                case KEY_UP:
                    rotate_right(&new_pos.tetromino);
                    collide = check_collision(&new_pos, game_board);
                    break;
                case ' ':
                    player.pos = drop_position(&player, game_board, game_win);
                    dropped = true;
                    collide = true;
                    break;
                case 'c':
                    if (!can_hold) {
                        break;
                    }
                    if (!hold.tetromino.size) {
                        hold.tetromino = player.tetromino;
                        game.pieces++;

                        push_player(&player, &queue[game.pieces % QUEUE_SIZE], game_board->cols);
                        new_pos.pos = player.pos;
                        new_pos.tetromino = player.tetromino;

                        push_next(&next, queue, QUEUE_SIZE, &game.pieces);
                        draw_piece(&next, next_win, C_BLOCK, false);
                        wrefresh(next_win);
                    } else {
                        Tetromino swap = player.tetromino;

                        push_player(&player, &hold.tetromino, game_board->cols);
                        new_pos.pos = player.pos;
                        new_pos.tetromino = player.tetromino;

                        hold.tetromino = swap;
                    }
                    draw_piece(&hold, hold_win, C_BLOCK, false);
                    wrefresh(hold_win);
                    can_hold = false;
                    break;
                default:
                    break;
            }
        }

        // Cleanup phase

        if (!collide) {
            player.tetromino = new_pos.tetromino;
            player.pos = new_pos.pos;
        } else if (new_pos.pos.y > player.pos.y || dropped == true) {
            lock(&player, game_board);
            push_player(&player, &queue[game.pieces % QUEUE_SIZE], game_board->cols);
            push_next(&next, queue, QUEUE_SIZE, &game.pieces);
            draw_piece(&next, next_win, C_BLOCK, false);
            wrefresh(next_win);

            int cleared = clear_complete_rows(game_board);
            update_score(&game.score, level, cleared);
            game.lines_cleared += cleared;
            draw_score(score_win, &game);
            can_hold = true;
            dropped = false;

            // Game over if new player collide with a block on the board
            if (check_collision(&player, game_board)) {
                game.game_over = true;
            }
        }

        // Render phase

        Position offset = {0, 0};
        draw_board(game_board, game_win, offset, false, C_BLOCK);
        draw_ghost(&player, game_board, game_win);
        draw_piece(&player, game_win, C_BLOCK, true);
        wrefresh(game_win);

        // Level 14 fall speed is ~11ms, lvl 15 is ~7ms.
        int sleep_time = 10;  // 100 fps
        game.ms_since_last_fall += sleep_time;
        sleep_ms(sleep_time);
    }

    mvwprintw(game_win, game_board->rows / 2, 6, "Game Over");
    wrefresh(game_win);
    timeout(-1);
    getch();

    free_board(game_board);
    endwin();  // deallocates memory and end ncurses
}
