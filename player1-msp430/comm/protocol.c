#include <comm/protocol.h>
#include <drivers/cli.h>
#include <msp430.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Helper function to send strings instead of numbers
void send_string(const char* str) {
  int i = 0;
  while (str[i] != '\0') {
    CLI_tx_byte(str[i]);
    i++;
  }
  CLI_tx_byte('\r');
  CLI_tx_byte('\n');
}

// Helper function to receive strings
bool receive_string(char* buffer, int max_len, uint32_t timeout_cycles) {
  int i = 0;
  char c;
  volatile uint32_t total_timeout = 0;
  memset(buffer, 0, max_len);
  while (!CLI_data_available() && total_timeout < timeout_cycles) {
    total_timeout++;
    if ((total_timeout % 16) == 0) {
      __delay_cycles(1);
    }
  }
  if (total_timeout >= timeout_cycles) return false;
  volatile int char_timeout = 0;
  while (i < max_len - 1) {
    if (CLI_data_available()) {
      c = CLI_rx_byte();
      if (c == '\n' || c == '\r') {
        __delay_cycles(800);
        if (CLI_data_available()) {
          CLI_rx_byte();
        }
        break;
      }
      if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
          (c >= '0' && c <= '9')) {
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
  return (i > 0);
}
