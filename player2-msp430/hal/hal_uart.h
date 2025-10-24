
#ifndef HAL_HAL_UART_H_
#define HAL_HAL_UART_H_

#include <stdint.h>
#include <stdbool.h>

void HAL_UART_init_gpio(void);
void HAL_UART_config(void);

bool HAL_UART_data_available(void);
uint8_t HAL_UART_rx_byte(void);
void HAL_UART_tx_byte(uint8_t txByte);



#endif /* HAL_HAL_UART_H_ */
