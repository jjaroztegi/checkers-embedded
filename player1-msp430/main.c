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

// Local function definitions
void Clocks_init();
void GUI_print_fixed_text();
void GUI_print_status(char* status, int line);
void handle_input(GameState* game);

// Global variables
Graphics_Context g_sContext;

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
  CHECKERS_init(&game);

  // Initial draw
  Graphics_clearDisplay(&g_sContext);
  CHECKERS_draw_board(&g_sContext, &game);

  // Main loop
  while (1) {
    __delay_cycles(1066666);  // 1,066,666 / 16,000,000 ≈ 67ms
    HAL_ADC_trigger_continuous_conversion();
    handle_input(&game);
    CHECKERS_draw_board(&g_sContext, &game);
  }
}

void handle_input(GameState* game) {
  // Joystick movement
  float joystick_x = JOYSTICK_get_x();
  float joystick_y = JOYSTICK_get_y();
  int dir_x = 0;
  int dir_y = 0;

  if (joystick_y > 90)
    dir_y = -1;
  else if (joystick_y < -90)
    dir_y = 1;
  else if (joystick_x > 90)
    dir_x = 1;
  else if (joystick_x < -90)
    dir_x = -1;

  if (dir_x != 0 || dir_y != 0) {
    CHECKERS_set_hovered(dir_x, dir_y, game);
  }

  // Button presses
  if (game->selection_state == IDLE) {
    if (SWITCH_get_edumkii_S1()) {
      CHECKERS_select_piece(game);
    }
  } else {
    if (SWITCH_get_edumkii_S2()) {
      CHECKERS_select_destination(game);
      Move move = CHECKERS_get_move(game);
      char move_buffer[8];
      CHECKERS_encode_move(&move, move_buffer);
      CHECKERS_apply_move_from_string(move_buffer, game);
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
  Graphics_initContext(&g_sContext, &g_sCRYSTALFONTZ);
  Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLUE);
  Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
  GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
  Graphics_clearDisplay(&g_sContext);
  Graphics_drawStringCentered(&g_sContext, (int8_t*)"Checkers",
                              AUTO_STRING_LENGTH, 64, 10, OPAQUE_TEXT);
  Graphics_drawStringCentered(&g_sContext, (int8_t*)"Player 1",
                              AUTO_STRING_LENGTH, 64, 20, OPAQUE_TEXT);
}

void GUI_print_status(char* status, int line) {
  Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
  Graphics_fillRectangle(&g_sContext,
                         &(Graphics_Rectangle){0, line - 4, 127, line + 8});
  Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
  Graphics_drawStringCentered(&g_sContext, (int8_t*)status, strlen(status), 64,
                              line, OPAQUE_TEXT);
}
