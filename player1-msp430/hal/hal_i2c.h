#ifndef HAL_I2C_H_
#define HAL_I2C_H_

#include <stdint.h>

void HAL_I2C_init_gpio(void);
void HAL_I2C_config(void);
uint16_t HAL_I2C_read16(unsigned char pointer);
void HAL_I2C_write16(unsigned char pointer, unsigned int writeByte);
void HAL_I2C_setslave(unsigned int slaveAdr);


#endif /* HAL_I2C_H_ */
