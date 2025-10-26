#include <game/checkers.h>
#include <stdlib.h>
#include <string.h>

// Game state
static PieceType board[8][8];
static bool selected = false;
static int selected_row = -1;
static int selected_col = -1;

// Function to initialize the checkers board
void init_checkers_board() {
  // Clear board
  int i, j, row, col;
  for (i = 0; i < 8; i++) {
    for (j = 0; j < 8; j++) {
      board[i][j] = EMPTY;
    }
  }
  // Setup initial piece positions
  // Black pieces (top, rows 0-2)
  for (row = 0; row < 3; row++) {
    for (col = 0; col < 8; col++) {
      if ((row + col) % 2 == 1) {
        board[row][col] = BLACK_PIECE;
      }
    }
  }
  // Red pieces (bottom, rows 5-7)
  for (row = 5; row < 8; row++) {
    for (col = 0; col < 8; col++) {
      if ((row + col) % 2 == 1) {
        board[row][col] = RED_PIECE;
      }
    }
  }
}

// Function to draw the checkers board
void draw_checkers_board(Graphics_Context* pContext) {
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
      // Draw square background (alternating colors)
      if ((row + col) % 2 == 0) {
        Graphics_setForegroundColor(pContext, GRAPHICS_COLOR_WHITE);
      } else {
        Graphics_setForegroundColor(pContext, GRAPHICS_COLOR_LIGHT_GRAY);
      }
      Graphics_fillRectangle(pContext, &rect);
      // Draw piece if present
      PieceType piece = board[row][col];
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
      // Draw selection highlight
      if (selected && row == selected_row && col == selected_col) {
        Graphics_setForegroundColor(pContext, GRAPHICS_COLOR_BLUE);
        Graphics_drawRectangle(pContext, &rect);
        rect.xMin++;
        rect.yMin++;
        rect.xMax--;
        rect.yMax--;
        Graphics_drawRectangle(pContext, &rect);
      }
    }
  }
}

// (col=0, row=7) -> A1 (col=7, row=0) -> H8
char col_to_letter(int col) { return 'A' + col; }

int letter_to_col(char letter) {
  if (letter >= 'a' && letter <= 'h') return letter - 'a';
  if (letter >= 'A' && letter <= 'H') return letter - 'A';
  return -1;
}

char row_to_digit(int row) { return '8' - row; }

int digit_to_row(char digit) {
  if (digit >= '1' && digit <= '8') return '8' - digit;
  return -1;
}

void encode_move(int from_col, int from_row, int to_col, int to_row,
                 char* buffer) {
  buffer[0] = col_to_letter(from_col);
  buffer[1] = row_to_digit(from_row);
  buffer[2] = col_to_letter(to_col);
  buffer[3] = row_to_digit(to_row);
  buffer[4] = '\0';
}

static bool decode_move(const char* move_str, int* from_col, int* from_row,
                        int* to_col, int* to_row) {
  if (strlen(move_str) < 4) return false;
  *from_col = letter_to_col(move_str[0]);
  *from_row = digit_to_row(move_str[1]);
  *to_col = letter_to_col(move_str[2]);
  *to_row = digit_to_row(move_str[3]);
  if (*from_col == -1 || *from_row == -1 || *to_col == -1 || *to_row == -1)
    return false;
  return true;
}

static bool apply_move(int from_col, int from_row, int to_col, int to_row) {
  int captured_row;
  int captured_col;

  if (from_row < 0 || from_row >= 8 || from_col < 0 || from_col >= 8 ||
      to_row < 0 || to_row >= 8 || to_col < 0 || to_col >= 8)
    return false;
  if (board[from_row][from_col] == EMPTY) return false;
  if (board[to_row][to_col] != EMPTY) return false;
  board[to_row][to_col] = board[from_row][from_col];
  board[from_row][from_col] = EMPTY;
  if (board[to_row][to_col] == RED_PIECE && to_row == 0)
    board[to_row][to_col] = RED_KING;
  else if (board[to_row][to_col] == BLACK_PIECE && to_row == 7)
    board[to_row][to_col] = BLACK_KING;
  if (abs(to_row - from_row) == 2) {
    captured_row = (from_row + to_row) / 2;
    captured_col = (from_col + to_col) / 2;
    board[captured_row][captured_col] = EMPTY;
  }
  return true;
}

bool apply_move_from_string(const char* move_str) {
  int from_col, from_row, to_col, to_row;
  if (decode_move(move_str, &from_col, &from_row, &to_col, &to_row)) {
    return apply_move(from_col, from_row, to_col, to_row);
  }
  return false;
}
