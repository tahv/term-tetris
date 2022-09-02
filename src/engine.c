#include "engine.h"

void init_ncurses(void)
{
    // setlocale(LC_ALL, "");
    initscr();             // initialize ncurses
    refresh();
    cbreak();              // Allow exit program with CTRL+C
    noecho();              // Dont print pressed characters to screen
    keypad(stdscr, true);  // Allow arrow keys
    curs_set(0);           // Hide cursor
    timeout(0);            // Non blocking getch

    init_pair(PAIR_BLACK_WHITE, COLOR_WHITE, COLOR_BLACK);
    // init_pair(PAIR_WHITE_BLACK, COLOR_BLACK, COLOR_WHITE);
}

WINDOW* init_boardwin(int rows, int cols)
{
    const int BORDERS = 2;

    Position term;
    getmaxyx(stdscr, term.y, term.x);

    WINDOW *boardwin = newwin(
        rows + BORDERS,
        cols * WIN_SCALE + BORDERS,
        term.y / 2 - rows / 2,
        term.x / 2 - cols * WIN_SCALE / 2
    );
    box(boardwin, 0, 0);
    wrefresh(boardwin);

    return boardwin;
}

WINDOW* init_nextwin(WINDOW *boardwin, int size)
{
    const int BORDERS = 2;
    const int SPACING_BOARDWIN = 1;

    Position boardwin_max;
    getmaxyx(boardwin, boardwin_max.y, boardwin_max.x);

    Position boardwin_beg;
    getbegyx(boardwin, boardwin_beg.y, boardwin_beg.x);

    Position boardwin_end = {
        boardwin_max.y + boardwin_beg.y,
        boardwin_max.x + boardwin_beg.x
    };

    WINDOW *nextwin = newwin(
        size + BORDERS,
        size * WIN_SCALE + BORDERS,
        boardwin_beg.y,
        boardwin_end.x + SPACING_BOARDWIN
    );
    box(nextwin, 0, 0);
    mvwprintw(nextwin, 0, 3, "Next");

    wrefresh(nextwin);

    return nextwin;
}

WINDOW* init_holdwin(WINDOW *boardwin, int size)
{
    const int BORDERS = 2;
    const int SPACING_BOARDWIN = 1;

    Position boardwin_beg;
    getbegyx(boardwin, boardwin_beg.y, boardwin_beg.x);

    Position holdwin_max = {
        size + BORDERS,
        size * WIN_SCALE + BORDERS
    };

    WINDOW *holdwin = newwin(
        holdwin_max.y,
        holdwin_max.x,
        boardwin_beg.y,
        boardwin_beg.x - holdwin_max.x - SPACING_BOARDWIN
    );
    box(holdwin, 0, 0);
    mvwprintw(holdwin, 0, 3, "Hold");

    wrefresh(holdwin);

    return holdwin;
}

void sleep_ms(int milliseconds)
{
    #ifdef WIN32
        Sleep(milliseconds);
    #elif _POSIX_C_SOURCE >= 199309L
        struct timespec ts;
        ts.tv_sec = milliseconds / 1000;
        ts.tv_nsec = (milliseconds % 1000) * 1000000;
        nanosleep(&ts, NULL);
    #else
        usleep(milliseconds * 1000);
    #endif
}

// Draw a block in window at given position
void draw_block(Position pos, WINDOW *window, chtype block)
{
    wmove(window, pos.y, pos.x);
    for (int i=0; i< WIN_SCALE; i++){
        waddch(window, block);
    }
}

Board* init_board(int rows, int cols)
{
    Board *board = malloc(sizeof(Board));
    board->rows = rows;
    board->cols = cols;
    board->matrix = malloc(rows * sizeof(bool*));
    for (int y = 0; y <  rows; y++) {
        board->matrix[y] = malloc(cols * sizeof(bool));
        memset(board->matrix[y], false, cols * sizeof(bool));
    }
    return board;
}

void free_board(Board *board)
{
    for (int y = 0; y < board->rows; y++) {
        free(board->matrix[y]);
    }
    free(board->matrix);
    free(board);
}

// Draw the content of board to window
void draw_board(Board *board, WINDOW *window, Position start, bool additive, chtype block)
{
    // TODO make sure we draw only inside win boundaries.
    const int BORDER = 1;  // Offset from border
    Position win_pos;

    for (int y = 0; y < board->rows; y++) {
		for (int x = 0; x < board->cols; x++) {

            win_pos.y = y + start.y + BORDER;
            win_pos.x = (x + start.x) * WIN_SCALE + BORDER;

            if (board->matrix[y][x]) {
                draw_block(win_pos, window, block);
            } else if (!additive) {
                draw_block(win_pos, window, ' ');
            }
        }
    }
}
