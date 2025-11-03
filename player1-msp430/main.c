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
#include <input/input.h>

// Constants
#define POLLING_RATE 60
#define RENDER_INTERVAL 3                            // 60/3 = 20fps
#define POLL_DELAY_CYCLES (16000000 / POLLING_RATE)  // 60Hz

// Turn state machine
typedef enum { TURN_PLAYING, TURN_SENDING, TURN_WAITING } TurnState;

// Local function definitions
void Clocks_init();
void GUI_print_fixed_text();
void GUI_print_status(char* status, int line);
void handle_input(GameState* game, InputState* input, TurnState* turn_state);

// Global variables
Graphics_Context g_graphicsContext;
TurnState turn_state = TURN_PLAYING;
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
  INPUT_init();

  GUI_print_fixed_text();
  GUI_print_status("READY!", 40);

  // Game state initialization
  GameState game;
  CHECKERS_init(&game, PLAYER_RED);  // Player 1 is RED

  // Initial draw
  Graphics_clearDisplay(&g_graphicsContext);
  CHECKERS_draw_board(&g_graphicsContext, &game);

  // Main loop
  int frame_counter = 0;
  while (1) {
    switch (turn_state) {
      case TURN_PLAYING: {
        // Accept input and handle moves
        __delay_cycles(POLL_DELAY_CYCLES);
        HAL_ADC_trigger_single_conversion();
        InputState input = INPUT_poll();
        handle_input(&game, &input, &turn_state);

        frame_counter++;
        if (frame_counter >= RENDER_INTERVAL) {
          CHECKERS_draw_board(&g_graphicsContext, &game);
          frame_counter = 0;
        }
        break;
      }
      case TURN_SENDING:
        // Draw final board state
        CHECKERS_draw_board(&g_graphicsContext, &game);
        __delay_cycles(8000000);  // 0.5 second delay to see the move

        // Send move to opponent
        char move_buffer[8];
        CHECKERS_encode_move(&pending_move, move_buffer);
        send_string(move_buffer);

        // Switch to waiting for opponent
        turn_state = TURN_WAITING;
        frame_counter = 0;
        break;

      case TURN_WAITING: {
        // Wait for opponent's move
        char receive_buffer[8];
        bool ok =
            receive_string(receive_buffer, sizeof(receive_buffer), 48000000);

        if (ok) {
          CHECKERS_apply_move_from_string(receive_buffer, &game);
          CHECKERS_draw_board(&g_graphicsContext, &game);
          __delay_cycles(8000000);  // 0.5 second delay
          turn_state = TURN_PLAYING;
        } else {
          __delay_cycles(8000000);
        }
        break;
      }
    }
  }
}

void handle_input(GameState* game, InputState* input, TurnState* turn_state) {
  if (*turn_state != TURN_PLAYING) {
    return;
  }

  // Handle cursor movement
  if (input->dir_x != 0 || input->dir_y != 0) {
    CHECKERS_move_cursor(input->dir_x, input->dir_y, game);
  }

  // Handle button presses
  if (game->selection_state == IDLE) {
    if (input->select_pressed) {
      CHECKERS_select_piece(game);
    }
  } else {  // PIECE_SELECTED
    if (input->confirm_pressed) {
      CHECKERS_confirm_move(game);
      pending_move = CHECKERS_get_move(game);
      // Only send if the move is valid
      if (CHECKERS_apply_move(game, &pending_move)) {
        *turn_state = TURN_SENDING;
      }
    }
  }
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
  Graphics_drawStringCentered(&g_graphicsContext, (int8_t*)"Player 1",
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
