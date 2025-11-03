#include <msp430.h>
#include <driverlib.h>


bool flag_edumkii_S1;
bool flag_edumkii_S2;

void HAL_DIGIN_init_gpio()
{
    // EDUMKII S1
    GPIO_setAsInputPin(GPIO_PORT_P4, GPIO_PIN3);
    // EDUMKII S2
    GPIO_setAsInputPin(GPIO_PORT_P4, GPIO_PIN2);
}

void HAL_DIGIN_config()
{
    // EDUMKII S1
    GPIO_selectInterruptEdge(GPIO_PORT_P4, GPIO_PIN3, GPIO_HIGH_TO_LOW_TRANSITION);
    GPIO_clearInterrupt(GPIO_PORT_P4, GPIO_PIN3);
    GPIO_enableInterrupt(GPIO_PORT_P4, GPIO_PIN3);
    // EDUMKII S2
    GPIO_selectInterruptEdge(GPIO_PORT_P4, GPIO_PIN2, GPIO_HIGH_TO_LOW_TRANSITION);
    GPIO_clearInterrupt(GPIO_PORT_P4, GPIO_PIN2);
    GPIO_enableInterrupt(GPIO_PORT_P4, GPIO_PIN2);

    flag_edumkii_S1 = false;
    flag_edumkii_S2 = false;
}


#pragma vector=PORT4_VECTOR
__interrupt void port4_isr_handler(void)
{
    uint16_t status;

    status = GPIO_getInterruptStatus(GPIO_PORT_P4, GPIO_PIN3 | GPIO_PIN2);

    // Check BT1
    if(status & GPIO_PIN3)
    {
        flag_edumkii_S1 = true;
        GPIO_clearInterrupt(GPIO_PORT_P4, GPIO_PIN3);
    }

    // Check BT2
    if(status & GPIO_PIN2)
    {
        flag_edumkii_S2 = true;
        GPIO_clearInterrupt(GPIO_PORT_P4, GPIO_PIN2);
    }
}
