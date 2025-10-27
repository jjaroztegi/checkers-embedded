#include <msp430.h>
#include <driverlib.h>

uint16_t joystick_x_raw;
uint16_t joystick_y_raw;

bool adc_working = false;

void HAL_ADC_init_gpio()
{
    // Joystick
    // X
    GPIO_setAsPeripheralModuleFunctionOutputPin(
            GPIO_PORT_P1,
            GPIO_PIN2,
            GPIO_TERNARY_MODULE_FUNCTION);
    // Y
    GPIO_setAsPeripheralModuleFunctionOutputPin(
            GPIO_PORT_P3,
            GPIO_PIN3,
            GPIO_TERNARY_MODULE_FUNCTION);
}

void HAL_ADC_config()
{
    ADC12_B_initParam adcConfig =
    {
        ADC12_B_SAMPLEHOLDSOURCE_SC,
        ADC12_B_CLOCKSOURCE_ADC12OSC,
        ADC12_B_CLOCKDIVIDER_8,
        ADC12_B_CLOCKPREDIVIDER__64,
        ADC12_B_NOINTCH
    };

    ADC12_B_init(ADC12_B_BASE, &adcConfig);
    ADC12_B_enable(ADC12_B_BASE);
    ADC12_B_setResolution(ADC12_B_BASE, ADC12_B_RESOLUTION_12BIT);
    ADC12_B_setupSamplingTimer(ADC12_B_BASE, ADC12_B_CYCLEHOLD_4_CYCLES,
                               ADC12_B_CYCLEHOLD_4_CYCLES, ADC12_B_MULTIPLESAMPLESENABLE);

    ADC12_B_enableInterrupt(ADC12_B_BASE, ADC12_B_IE1, 0, 0);

    ADC12_B_configureMemoryParam joystick_x_config =
    {
     ADC12_B_MEMORY_0,
     ADC12_B_INPUT_A2,
     ADC12_B_VREFPOS_AVCC_VREFNEG_VSS,
     ADC12_B_NOTENDOFSEQUENCE,
     ADC12_B_WINDOW_COMPARATOR_DISABLE,
     ADC12_B_DIFFERENTIAL_MODE_DISABLE
    };
    ADC12_B_configureMemory(ADC12_B_BASE, &joystick_x_config);

    ADC12_B_configureMemoryParam joystick_y_config =
    {
     ADC12_B_MEMORY_1,
     ADC12_B_INPUT_A15,
     ADC12_B_VREFPOS_AVCC_VREFNEG_VSS,
     ADC12_B_ENDOFSEQUENCE,
     ADC12_B_WINDOW_COMPARATOR_DISABLE,
     ADC12_B_DIFFERENTIAL_MODE_DISABLE
    };
    ADC12_B_configureMemory(ADC12_B_BASE, &joystick_y_config);
}

void HAL_ADC_trigger_single_conversion()
{
    adc_working = true;
    ADC12_B_startConversion(ADC12_B_BASE, ADC12_B_START_AT_ADC12MEM0, ADC12_B_SEQOFCHANNELS);
    while (adc_working);
}

void HAL_ADC_trigger_continuous_conversion()
{
    ADC12_B_startConversion(ADC12_B_BASE, ADC12_B_START_AT_ADC12MEM0, ADC12_B_REPEATED_SEQOFCHANNELS);
}


#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
{
    uint16_t status;

    status = ADC12_B_getInterruptStatus(ADC12_B_BASE, 0, ADC12_B_IFG1);
    /* ADC12_B_MEMORY_2 conversion completed */
    if(status)
    {
        joystick_x_raw = ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_0);
        joystick_y_raw = ADC12_B_getResults(ADC12_B_BASE, ADC12_B_MEMORY_1);

        adc_working = false;
        ADC12_B_clearInterrupt(ADC12_B_BASE, 0, ADC12_B_IFG1);
    }

}
