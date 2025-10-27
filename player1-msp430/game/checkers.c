#include <game/checkers.h>
#include <stdlib.h>
#include <string.h>

// Helper functions
static char col_to_letter(int col) { return 'A' + col; }
static int letter_to_col(char letter) {
  if (letter >= 'a' && letter <= 'h') return letter - 'a';
  if (letter >= 'A' && letter <= 'H') return letter - 'A';
  return -1;
}
static char row_to_digit(int row) { return '8' - row; }
static int digit_to_row(char digit) {
  if (digit >= '1' && digit <= '8') return '8' - digit;
  return -1;
}

void CHECKERS_init(GameState* state) {
  // Clear board
  int i, j, row, col;
  for (i = 0; i < 8; i++) {
    for (j = 0; j < 8; j++) {
      state->board[i][j] = EMPTY;
    }
  }
  // Setup initial piece positions
  // Black pieces (top, rows 0-2)
  for (row = 0; row < 3; row++) {
    for (col = 0; col < 8; col++) {
      if ((row + col) % 2 == 1) {
        state->board[row][col] = BLACK_PIECE;
      }
    }
  }
  // Red pieces (bottom, rows 5-7)
  for (row = 5; row < 8; row++) {
    for (col = 0; col < 8; col++) {
      if ((row + col) % 2 == 1) {
        state->board[row][col] = RED_PIECE;
      }
    }
  }

  state->selection_state = IDLE;
  state->hovered_row = 0;
  state->hovered_col = 0;
  state->selected_row = -1;
  state->selected_col = -1;
  state->last_move_valid = false;
}

void CHECKERS_draw_board(Graphics_Context* pContext, const GameState* state) {
  Graphics_Rectangle rect;
  int row, col;
  for (row = 0; row < BOARD_SIZE; row++) {
    for (col = 0; col < BOARD_SIZE; col++) {
      int x = BOARD_OFFSET_X + col * SQUARE_SIZE;
      int y = BOARD_OFFSET_Y + row * SQUARE_SIZE;
      rect.xMin = x;
      rect.yMin = y;
      rect.xMax = x + SQUARE_SIZE - 1;
      rect.yMax = y + SQUARE_SIZE - 1;

      // Draw square background
      if (state->last_move_valid && ((row == state->last_move.from_row &&
                                      col == state->last_move.from_col) ||
                                     (row == state->last_move.to_row &&
                                      col == state->last_move.to_col))) {
        Graphics_setForegroundColor(pContext, GRAPHICS_COLOR_YELLOW);
      } else if ((row + col) % 2 == 0) {
        Graphics_setForegroundColor(pContext, GRAPHICS_COLOR_WHITE);
      } else {
        Graphics_setForegroundColor(pContext, GRAPHICS_COLOR_LIGHT_GRAY);
      }
      Graphics_fillRectangle(pContext, &rect);

      // Draw piece
      PieceType piece = state->board[row][col];
      if (piece != EMPTY) {
        int center_x = x + SQUARE_SIZE / 2;
        int center_y = y + SQUARE_SIZE / 2;
        int radius = 5;
        // Set piece color
        if (piece == RED_PIECE || piece == RED_KING) {
          Graphics_setForegroundColor(pContext, GRAPHICS_COLOR_RED);
        } else {
          Graphics_setForegroundColor(pContext, GRAPHICS_COLOR_BLACK);
        }
        // Draw piece (filled circle)
        Graphics_fillCircle(pContext, center_x, center_y, radius);
        // Draw king indicator (smaller circle inside)
        if (piece == RED_KING || piece == BLACK_KING) {
          Graphics_setForegroundColor(pContext, GRAPHICS_COLOR_YELLOW);
          Graphics_fillCircle(pContext, center_x, center_y, 2);
        }
      }

      // Draw highlights
      if (state->selection_state == PIECE_SELECTED &&
          row == state->selected_row && col == state->selected_col) {
        Graphics_setForegroundColor(pContext, GRAPHICS_COLOR_BLUE);
        Graphics_drawRectangle(pContext, &rect);
      }
      if (row == state->hovered_row && col == state->hovered_col) {
        Graphics_setForegroundColor(pContext, GRAPHICS_COLOR_GREEN);
        Graphics_drawRectangle(pContext, &rect);
      }
    }
  }
}

void CHECKERS_encode_move(const Move* move, char* move_buffer) {
  move_buffer[0] = col_to_letter(move->from_col);
  move_buffer[1] = row_to_digit(move->from_row);
  move_buffer[2] = col_to_letter(move->to_col);
  move_buffer[3] = row_to_digit(move->to_row);
  move_buffer[4] = '\0';
}

static bool decode_move(const char* move_str, Move* move) {
  if (strlen(move_str) < 4) return false;
  move->from_col = letter_to_col(move_str[0]);
  move->from_row = digit_to_row(move_str[1]);
  move->to_col = letter_to_col(move_str[2]);
  move->to_row = digit_to_row(move_str[3]);
  if (move->from_col == -1 || move->from_row == -1 || move->to_col == -1 ||
      move->to_row == -1)
    return false;
  return true;
}

static bool apply_move(GameState* state, const Move* move) {
  //- TODO: Add check for valid move (e.g. player's turn, piece type, direction)
  int captured_row, captured_col;

  if (move->from_row < 0 || move->from_row >= 8 || move->from_col < 0 ||
      move->from_col >= 8 || move->to_row < 0 || move->to_row >= 8 ||
      move->to_col < 0 || move->to_col >= 8)
    return false;
  if (state->board[move->from_row][move->from_col] == EMPTY) return false;
  if (state->board[move->to_row][move->to_col] != EMPTY) return false;

  state->board[move->to_row][move->to_col] =
      state->board[move->from_row][move->from_col];
  state->board[move->from_row][move->from_col] = EMPTY;

  if (state->board[move->to_row][move->to_col] == RED_PIECE &&
      move->to_row == 0)
    state->board[move->to_row][move->to_col] = RED_KING;
  else if (state->board[move->to_row][move->to_col] == BLACK_PIECE &&
           move->to_row == 7)
    state->board[move->to_row][move->to_col] = BLACK_KING;

  if (abs(move->to_row - move->from_row) == 2) {
    captured_row = (move->from_row + move->to_row) / 2;
    captured_col = (move->from_col + move->to_col) / 2;
    state->board[captured_row][captured_col] = EMPTY;
  }

  state->last_move = *move;
  state->last_move_valid = true;
  return true;
}

bool CHECKERS_apply_move_from_string(const char* move_str, GameState* state) {
  Move move;
  if (decode_move(move_str, &move)) {
    return apply_move(state, &move);
  }
  return false;
}

void CHECKERS_set_hovered(int dir_x, int dir_y, GameState* state) {
  state->hovered_row += dir_y;
  state->hovered_col += dir_x;

  //- TODO: Add boundary checks to wrap around the board
}

void CHECKERS_select_piece(GameState* state) {
  //- TODO: Add check for valid piece (e.g. not empty, belongs to current
  // player)
  state->selection_state = PIECE_SELECTED;
  state->selected_row = state->hovered_row;
  state->selected_col = state->hovered_col;
  state->last_move_valid = false;
}

void CHECKERS_select_destination(GameState* state) {
  state->selection_state = IDLE;
}

Move CHECKERS_get_move(const GameState* state) {
  Move move;
  move.from_row = state->selected_row;
  move.from_col = state->selected_col;
  move.to_row = state->hovered_row;
  move.to_col = state->hovered_col;
  return move;
}
