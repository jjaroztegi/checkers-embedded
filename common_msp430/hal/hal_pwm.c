#include <driverlib.h>
#include <msp430.h>
#include <hal/hal_pwm.h>

void HAL_PWM_init_gpio()
{
    // P3.7 mapped to TB0.6 for buzzer output
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P3, GPIO_PIN7, GPIO_PRIMARY_MODULE_FUNCTION);
}

void HAL_PWM_config()
{
    // Configure CCR6 for buzzer output on Timer_B0
    // Timer_B0 is already initialized and running by LCD backlight
    Timer_B_initCompareModeParam compareModeParam = {
        TIMER_B_CAPTURECOMPARE_REGISTER_6,
        TIMER_B_CAPTURECOMPARE_INTERRUPT_DISABLE,
        TIMER_B_OUTPUTMODE_RESET_SET,  // PWM mode
        0  // Start with buzzer off (0% duty cycle)
    };
    Timer_B_initCompareMode(TIMER_B0_BASE, &compareModeParam);
}

void HAL_PWM_buzzer_on()
{
    // Set buzzer to 50% duty cycle for audible tone
    uint16_t period = Timer_B_getCaptureCompareCount(TIMER_B0_BASE, TIMER_B_CAPTURECOMPARE_REGISTER_0);
    Timer_B_setCompareValue(TIMER_B0_BASE, TIMER_B_CAPTURECOMPARE_REGISTER_6, period / 2);
}

void HAL_PWM_buzzer_off()
{
    // Turn off buzzer (0% duty cycle)
    Timer_B_setCompareValue(TIMER_B0_BASE, TIMER_B_CAPTURECOMPARE_REGISTER_6, 0);
}
