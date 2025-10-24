#include <hal/hal_uart.h>
#include <stdbool.h>
#include <stdint.h>

bool CLI_data_available(void)
{
    return HAL_UART_data_available();
}

uint8_t CLI_rx_byte(void)
{
    return HAL_UART_rx_byte();
}

void CLI_tx_byte(uint8_t txByte)
{
    HAL_UART_tx_byte(txByte);
}
