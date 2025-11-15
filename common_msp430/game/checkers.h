#ifndef GAME_CHECKERS_H_
#define GAME_CHECKERS_H_

#include <drivers/crystalfontz.h>
#include <hal/hal_lcd.h>
#include <stdbool.h>

// Checkers board constants
#define BOARD_SIZE 8
#define SQUARE_SIZE 14
#define BOARD_OFFSET_X 8
#define BOARD_OFFSET_Y 8

// Piece types
typedef enum {
  EMPTY = 0,
  RED_PIECE = 1,
  RED_KING = 2,
  BLACK_PIECE = 3,
  BLACK_KING = 4
} PieceType;

typedef enum { IDLE, PIECE_SELECTED } SelectionState;

typedef enum { PLAYER_NONE = 0, PLAYER_RED = 1, PLAYER_BLACK = 2 } Player;

typedef struct {
  int from_row;
  int from_col;
  int to_row;
  int to_col;
} Move;

typedef struct {
  PieceType board[8][8];
  SelectionState selection_state;
  int hovered_row;
  int hovered_col;
  int selected_row;
  int selected_col;
  Move last_move;
  bool last_move_valid;
  Player current_player;
} GameState;

void CHECKERS_init(GameState* state, Player player);
void CHECKERS_draw_board(Graphics_Context* pContext, const GameState* state);
void CHECKERS_encode_move(const Move* move, char* move_buffer);
bool CHECKERS_apply_move(GameState* state, const Move* move);
bool CHECKERS_apply_move_from_string(const char* move_str, GameState* state);
void CHECKERS_move_cursor(int dir_x, int dir_y, GameState* state);
void CHECKERS_select_piece(GameState* state);
void CHECKERS_confirm_move(GameState* state);
Move CHECKERS_get_move(const GameState* state);
Player CHECKERS_game_ended(GameState* state);
bool CHECKERS_find_valid_move(GameState* state, Move* move_to_fill);

#endif /* GAME_CHECKERS_H_ */
