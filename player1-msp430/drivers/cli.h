#ifndef DRIVERS_CLI_H_
#define DRIVERS_CLI_H_

#include <stdbool.h>
#include <stdint.h>

bool CLI_data_available(void);
uint8_t CLI_rx_byte(void);
void CLI_tx_byte(uint8_t txByte);


#endif /* DRIVERS_CLI_H_ */
