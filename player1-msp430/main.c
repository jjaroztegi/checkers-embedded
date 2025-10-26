// General libraries
#include <driverlib.h>
#include <msp430.h>
#include <stdbool.h>
#include <string.h>

// HAL headers
#include <hal/hal_lcd.h>
#include <hal/hal_uart.h>

// Driver headers
#include <drivers/crystalfontz.h>

// Game Headers
#include <comm/protocol.h>
#include <game/checkers.h>

// Local function definitions
void Clocks_init();
void GUI_print_fixed_text();
void GUI_print_status(char* status, int line);

// Global variables
Graphics_Context g_sContext;
uint16_t counter = 0;
bool my_turn = true;  // MSP starts by sending

void main(void) {
  // Stop WDT
  WDT_A_hold(WDT_A_BASE);

  // Initialize clocks
  Clocks_init();

  // Initialize HAL GPIOs
  HAL_LCD_init_gpio();
  HAL_UART_init_gpio();

  // Disable the GPIO power-on default high-impedance mode
  PMM_unlockLPM5();

  // Configure HAL modules
  HAL_LCD_config();
  HAL_UART_config();

  // Enable global interrupts
  __bis_SR_register(GIE);

  // Initialize and configure external devices
  CRYSTALFONTZ_init();

  // Print fixed text on GLCD
  GUI_print_fixed_text();

  // 5 second startup delay - both devices sync
  GUI_print_status("WAITING 5s...", 40);
  __delay_cycles(80000000);  // 5 seconds @ 16MHz

  GUI_print_status("READY!", 40);
  __delay_cycles(16000000);  // 1 more second

  // Initialize checkers board
  init_checkers_board();
  Graphics_clearDisplay(&g_sContext);
  draw_checkers_board(&g_sContext);

  __delay_cycles(16000000 * 2);  // 2 second delay
  // Example: Move from A3 to B4
  apply_move_from_string("A3B4");
  draw_checkers_board(&g_sContext);

  // Example: receive and apply a move
  char received_move[8];
  if (receive_string(received_move, sizeof(received_move), 48000000)) {
    if (apply_move_from_string(received_move)) {
      draw_checkers_board(&g_sContext);  // Redraw after move
    }
  }

  // Main loop (can be expanded for interactive play)
  while (1) {
    // interactive move logic here
    __delay_cycles(16000000);  // 1 second delay
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
  Graphics_drawStringCentered(&g_sContext, (int8_t*)"RF PING-PONG",
                              AUTO_STRING_LENGTH, 64, 10, OPAQUE_TEXT);
  Graphics_drawStringCentered(&g_sContext, (int8_t*)"MSP430FR5994",
                              AUTO_STRING_LENGTH, 64, 20, OPAQUE_TEXT);
  Graphics_drawStringCentered(&g_sContext, (int8_t*)"PLAYER 1",
                              AUTO_STRING_LENGTH, 64, 30, OPAQUE_TEXT);
}

void GUI_print_status(char* status, int line) {
  // Clear the line first
  Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
  Graphics_fillRectangle(&g_sContext,
                         &(Graphics_Rectangle){0, line - 4, 127, line + 8});

  // Print new status
  Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_RED);
  Graphics_drawStringCentered(&g_sContext, (int8_t*)status, strlen(status), 64,
                              line, OPAQUE_TEXT);
}
