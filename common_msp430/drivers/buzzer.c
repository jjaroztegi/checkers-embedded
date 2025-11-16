#include <hal/hal_pwm.h>

void BUZ_sound_on()
{
    HAL_PWM_buzzer_on();
}

void BUZ_sound_off()
{
    HAL_PWM_buzzer_off();
}
