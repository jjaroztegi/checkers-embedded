#include <hal/hal_adc.h>
#include <stdbool.h>


float JOYSTICK_get_x()
{
    float joystick_x_percentage = joystick_x_raw/4095.0 * 200 - 100;
    return joystick_x_percentage;
}

float JOYSTICK_get_y()
{
    float joystick_y_percentage = joystick_y_raw/4095.0 * 200 - 100;
    return joystick_y_percentage;
}
