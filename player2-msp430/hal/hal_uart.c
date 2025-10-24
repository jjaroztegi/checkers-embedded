#include <msp430.h>
#include <driverlib.h>

bool UART_RX_flag = false;
uint8_t UART_RX_data;

void HAL_UART_init_gpio()
{
    // Select UART function for TX (P6.0) - Output
    GPIO_setAsPeripheralModuleFunctionOutputPin(
            GPIO_PORT_P6,
            GPIO_PIN0,
            GPIO_PRIMARY_MODULE_FUNCTION);

    // Select UART function for RX (P6.1) - Input
    GPIO_setAsPeripheralModuleFunctionInputPin(
            GPIO_PORT_P6,
            GPIO_PIN1,
            GPIO_PRIMARY_MODULE_FUNCTION);
}


void HAL_UART_config(void)
{
    EUSCI_A_UART_initParam uartConfig = {0};
    uartConfig.selectClockSource = EUSCI_A_UART_CLOCKSOURCE_SMCLK;

    // Baud rate calculation for 115200 @ 16MHz SMCLK
    // N = 16,000,000 / 115,200 = 138.888
    // See MSP430FR59xx Family User Guide (SLAU367P) Section 22.3.10

    // UCBRx = INT(N / 16) = INT(138.888 / 16) = INT(8.68) = 8
    uartConfig.clockPrescalar = 8;

    // UCBRFx = round(((N/16) - INT(N/16)) * 16) = round(0.68 * 16) = 11
    uartConfig.firstModReg = 11;

    // UCBRSx = Lookup table value for fractional part of N (0.888) = 0xD6
    uartConfig.secondModReg = 0xD6; // 214 in decimal

    uartConfig.parity = EUSCI_A_UART_NO_PARITY;
    uartConfig.msborLsbFirst = EUSCI_A_UART_LSB_FIRST;
    uartConfig.numberofStopBits = EUSCI_A_UART_ONE_STOP_BIT;
    uartConfig.uartMode = EUSCI_A_UART_MODE;
    uartConfig.overSampling = EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION;

    // Initialize USCI_A0 as UART
    EUSCI_A_UART_init(EUSCI_A3_BASE, &uartConfig);

    // Enable UART module
    EUSCI_A_UART_enable(EUSCI_A3_BASE);

    // Enable RX interrupt
    EUSCI_A_UART_enableInterrupt(EUSCI_A3_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
}

bool HAL_UART_data_available(void)
{
    return UART_RX_flag;
}

uint8_t HAL_UART_rx_byte(void)
{
    UART_RX_flag = false;
    return UART_RX_data;
}

void HAL_UART_tx_byte(uint8_t txByte)
{
    EUSCI_A_UART_transmitData(EUSCI_A3_BASE, txByte);
}

#pragma vector=EUSCI_A3_VECTOR
__interrupt void USCI_A3_ISR(void)
{
    uint8_t status = EUSCI_A_UART_getInterruptStatus(EUSCI_A3_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG);

    if (status == EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG)
    {
        UART_RX_data = EUSCI_A_UART_receiveData(EUSCI_A3_BASE);
        UART_RX_flag = true;
        EUSCI_A_UART_clearInterrupt(EUSCI_A3_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT_FLAG);
    }
}

