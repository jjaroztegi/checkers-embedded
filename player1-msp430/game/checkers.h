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

// Function prototypes
void init_checkers_board();
void draw_checkers_board(Graphics_Context *pContext);
void encode_move(int from_col, int from_row, int to_col, int to_row,
                 char* buffer);
bool apply_move_from_string(const char* move_str);

#endif /* GAME_CHECKERS_H_ */
