
#ifndef HAL_ADC_H_
#define HAL_ADC_H_

#include <stdint.h>

extern uint16_t joystick_x_raw;
extern uint16_t joystick_y_raw;

void HAL_ADC_init_gpio();
void HAL_ADC_config();
void HAL_ADC_trigger_single_conversion();
void HAL_ADC_trigger_continuous_conversion();

#endif /* HAL_ADC_H_ */
