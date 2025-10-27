#ifndef HAL_DIGITAL_INPUT_H_
#define HAL_DIGITAL_INPUT_H_

#include <stdbool.h>

extern bool flag_edumkii_S1;
extern bool flag_edumkii_S2;

void HAL_DIGIN_init_gpio();
void HAL_DIGIN_config();


#endif /* HAL_DIGITAL_INPUT_H_ */
