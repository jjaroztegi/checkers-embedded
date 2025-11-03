// General libraries
#include <driverlib.h>
#include <msp430.h>
#include <stdbool.h>
#include <string.h>

// HAL headers
#include <hal/hal_adc.h>
#include <hal/hal_digital_input.h>
#include <hal/hal_lcd.h>
#include <hal/hal_uart.h>

// Driver headers
#include <drivers/crystalfontz.h>
#include <drivers/joystick.h>
#include <drivers/switch.h>

// Game Headers
#include <comm/protocol.h>
#include <game/checkers.h>

// Constants
#define POLLING_RATE 60
#define RENDER_INTERVAL 3                            // 60/3 = 20fps
#define POLL_DELAY_CYCLES (16000000 / POLLING_RATE)  // 60Hz
#define JOYSTICK_THRESHOLD 85

// Local function definitions
void Clocks_init();
void GUI_print_fixed_text();
void GUI_print_status(char* status, int line);
void handle_input(GameState* game);

// Global variables
Graphics_Context g_graphicsContext;
bool my_turn = false;  // Player 2 starts listening
bool move_ready_to_send = false;
Move pending_move;

void main(void) {
  // Stop WDT
  WDT_A_hold(WDT_A_BASE);

  // Initializations
  Clocks_init();
  HAL_LCD_init_gpio();
  HAL_UART_init_gpio();
  HAL_ADC_init_gpio();

  PMM_unlockLPM5();

  HAL_LCD_config();
  HAL_UART_config();
  HAL_ADC_config();
  HAL_DIGIN_init_gpio();

  // Enable global interrupts
  __bis_SR_register(GIE);

  // External devices
  CRYSTALFONTZ_init();
  HAL_DIGIN_config();

  GUI_print_fixed_text();
  GUI_print_status("READY!", 40);

  // Game state initialization
  GameState game;
  CHECKERS_init(&game, PLAYER_BLACK);  // Player 2 is BLACK

  // Initial draw
  Graphics_clearDisplay(&g_graphicsContext);
  CHECKERS_draw_board(&g_graphicsContext, &game);

  // Main loop
  int frame_counter = 0;
  while (1) {
    if (my_turn) {
      // MY TURN: Handle input and wait for move
      if (!move_ready_to_send) {
        __delay_cycles(POLL_DELAY_CYCLES);
        HAL_ADC_trigger_continuous_conversion();
        handle_input(&game);

        frame_counter++;
        if (frame_counter >= RENDER_INTERVAL) {
          CHECKERS_draw_board(&g_graphicsContext, &game);
          frame_counter = 0;
        }
      } else {
        // Move is ready to send - draw board one final time
        CHECKERS_draw_board(&g_graphicsContext, &game);
        __delay_cycles(8000000);  // 0.5 second delay to see the move

        // Send move to opponent
        char move_buffer[8];
        CHECKERS_encode_move(&pending_move, move_buffer);
        send_string(move_buffer);

        // Switch to opponent's turn
        move_ready_to_send = false;
        my_turn = false;
      }
    } else {
      // LISTENING MODE: Wait for opponent's move
      char receive_buffer[8];
      bool ok =
          receive_string(receive_buffer, sizeof(receive_buffer), 48000000);

      if (ok) {
        CHECKERS_apply_move_from_string(receive_buffer, &game);
        CHECKERS_draw_board(&g_graphicsContext, &game);
        __delay_cycles(8000000);  // 0.5 second delay
        my_turn = true;
      } else {
        __delay_cycles(8000000);
      }
    }
  }
}

void handle_input(GameState* game) {
  // Joystick movement
  static int prev_dir_x = 0;
  static int prev_dir_y = 0;
  static bool first_read = true;

  float joystick_x = JOYSTICK_get_x();
  float joystick_y = JOYSTICK_get_y();
  int dir_x = 0;
  int dir_y = 0;

  if (joystick_y > JOYSTICK_THRESHOLD)
    dir_y = -1;
  else if (joystick_y < -JOYSTICK_THRESHOLD)
    dir_y = 1;
  else if (joystick_x > JOYSTICK_THRESHOLD)
    dir_x = 1;
  else if (joystick_x < -JOYSTICK_THRESHOLD)
    dir_x = -1;

  // Initialize prev values on first read to avoid initial movement
  if (first_read) {
    prev_dir_x = dir_x;
    prev_dir_y = dir_y;
    first_read = false;
  }

  // Move only on a new direction
  if ((dir_x != 0 || dir_y != 0) &&
      (dir_x != prev_dir_x || dir_y != prev_dir_y)) {
    CHECKERS_move_cursor(dir_x, dir_y, game);
  }

  prev_dir_x = dir_x;
  prev_dir_y = dir_y;

  // Button presses
  static bool prev_s1_state = false;
  static bool prev_s2_state = false;

  bool s1_state = SWITCH_get_edumkii_S1();
  bool s2_state = SWITCH_get_edumkii_S2();

  if (game->selection_state == IDLE) {
    if (s1_state && !prev_s1_state) {  // S1 pressed
      CHECKERS_select_piece(game);
    }
  } else {                             // PIECE_SELECTED
    if (s2_state && !prev_s2_state) {  // S2 pressed
      CHECKERS_confirm_move(game);
      pending_move = CHECKERS_get_move(game);
      // Only send if the move is valid
      if (CHECKERS_apply_move(game, &pending_move)) {
        move_ready_to_send = true;
      }
    }
  }

  prev_s1_state = s1_state;
  prev_s2_state = s2_state;
}

void Clocks_init() {
  FRAMCtl_A_configureWaitStateControl(FRAMCTL_A_ACCESS_TIME_CYCLES_0);
  CS_setDCOFreq(CS_DCORSEL_1, CS_DCOFSEL_4);
  CS_initClockSignal(CS_MCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
  CS_initClockSignal(CS_SMCLK, CS_DCOCLK_SELECT, CS_CLOCK_DIVIDER_1);
  CS_initClockSignal(CS_ACLK, CS_VLOCLK_SELECT, CS_CLOCK_DIVIDER_1);
}

void GUI_print_fixed_text() {
  CRYSTALFONTZ_set_orientation(LCD_ORIENTATION_UP);
  Graphics_initContext(&g_graphicsContext, &g_sCRYSTALFONTZ);
  Graphics_setForegroundColor(&g_graphicsContext, GRAPHICS_COLOR_BLUE);
  Graphics_setBackgroundColor(&g_graphicsContext, GRAPHICS_COLOR_WHITE);
  GrContextFontSet(&g_graphicsContext, &g_sFontFixed6x8);
  Graphics_clearDisplay(&g_graphicsContext);
  Graphics_drawStringCentered(&g_graphicsContext, (int8_t*)"Checkers",
                              AUTO_STRING_LENGTH, 64, 10, OPAQUE_TEXT);
  Graphics_drawStringCentered(&g_graphicsContext, (int8_t*)"Player 2",
                              AUTO_STRING_LENGTH, 64, 20, OPAQUE_TEXT);
}

void GUI_print_status(char* status, int line) {
  Graphics_setForegroundColor(&g_graphicsContext, GRAPHICS_COLOR_WHITE);
  Graphics_fillRectangle(&g_graphicsContext,
                         &(Graphics_Rectangle){0, line - 4, 127, line + 8});
  Graphics_setForegroundColor(&g_graphicsContext, GRAPHICS_COLOR_RED);
  Graphics_drawStringCentered(&g_graphicsContext, (int8_t*)status,
                              strlen(status), 64, line, OPAQUE_TEXT);
}
