// General libraries
#include <driverlib.h>
#include <msp430.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// HAL headers
#include <hal/hal_lcd.h>
#include <hal/hal_uart.h>

// Driver headers
#include <drivers/cli.h>
#include <drivers/crystalfontz.h>

// Local function definitions
void Clocks_init();
void GUI_print_fixed_text();
void GUI_print_status(char* status, int line);
void send_number(uint16_t number);
bool receive_number(uint16_t* number, uint32_t timeout_cycles);

// Global variables
Graphics_Context g_sContext;
uint16_t counter = 0;
bool my_turn = false;  // MSP Player 2 starts by listening

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

  while (1) {
    char display_str[32];

    if (my_turn) {
      // MY TURN: Send the counter

      // Clear any stale RX data
      while (CLI_data_available()) {
        CLI_rx_byte();
      }

      sprintf(display_str, "TX: %u", counter);
      GUI_print_status(display_str, 40);

      send_number(counter);

      // Now switch to listening mode
      my_turn = false;
      GUI_print_status("LISTENING...", 50);

    } else {
      // LISTENING MODE: Wait for response
      uint16_t received_number = 0;
      bool success = receive_number(&received_number, 48000000);

      if (success) {
        sprintf(display_str, "RX: %u", received_number);
        GUI_print_status(display_str, 50);

        counter = received_number + 1;

        // Wait a bit, then it's my turn again
        __delay_cycles(16000000);  // 1 second delay
        my_turn = true;

      } else {
        GUI_print_status("RX: TIMEOUT", 50);

        // Reset and try again
        __delay_cycles(16000000);
        my_turn = true;
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
  Graphics_initContext(&g_sContext, &g_sCRYSTALFONTZ);
  Graphics_setForegroundColor(&g_sContext, GRAPHICS_COLOR_BLUE);
  Graphics_setBackgroundColor(&g_sContext, GRAPHICS_COLOR_WHITE);
  GrContextFontSet(&g_sContext, &g_sFontFixed6x8);
  Graphics_clearDisplay(&g_sContext);
  Graphics_drawStringCentered(&g_sContext, (int8_t*)"RF PING-PONG",
                              AUTO_STRING_LENGTH, 64, 10, OPAQUE_TEXT);
  Graphics_drawStringCentered(&g_sContext, (int8_t*)"MSP430FR5994",
                              AUTO_STRING_LENGTH, 64, 20, OPAQUE_TEXT);
  Graphics_drawStringCentered(&g_sContext, (int8_t*)"PLAYER 2",
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

void send_number(uint16_t number) {
  char buffer[16];
  sprintf(buffer, "%u\r\n", number);

  int i = 0;
  while (buffer[i] != '\0') {
    CLI_tx_byte(buffer[i]);
    i++;
  }
}

bool receive_number(uint16_t* number, uint32_t timeout_cycles) {
  char buffer[16];
  int i = 0;
  char c;
  volatile uint32_t total_timeout = 0;

  memset(buffer, 0, sizeof(buffer));

  // Wait for first byte
  while (!CLI_data_available() && total_timeout < timeout_cycles) {
    total_timeout++;
    if ((total_timeout % 16) == 0) {
      __delay_cycles(1);
    }
  }

  if (total_timeout >= timeout_cycles) {
    return false;
  }

  // Read bytes until newline
  volatile int char_timeout = 0;
  while (i < sizeof(buffer) - 1) {
    if (CLI_data_available()) {
      c = CLI_rx_byte();

      if (c == '\n' || c == '\r') {
        // Consume extra newline if present
        __delay_cycles(800);
        if (CLI_data_available()) {
          CLI_rx_byte();
        }
        break;
      }

      if (c >= '0' && c <= '9') {
        buffer[i++] = c;
      }

      char_timeout = 0;
    } else {
      char_timeout++;
      if (char_timeout > 10000) {
        break;
      }
    }
  }

  buffer[i] = '\0';

  if (i > 0) {
    *number = (uint16_t)atoi(buffer);
    return true;
  }

  return false;
}
