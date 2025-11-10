#include <msp430.h>
#include <driverlib.h>


// I2C Master Configuration Parameter
EUSCI_B_I2C_initMasterParam i2cConfig =
{
        EUSCI_B_I2C_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        16000000,                               // SMCLK = 16MHz
        EUSCI_B_I2C_SET_DATA_RATE_100KBPS,      // Desired I2C Clock of 400khz
        0,                                      // No byte counter threshold
        EUSCI_B_I2C_NO_AUTO_STOP                // No Autostop
};

void HAL_I2C_init_gpio()
{
    // Select I2C function for I2C_SCL(P7.1)
    GPIO_setAsPeripheralModuleFunctionOutputPin(
            GPIO_PORT_P7,
            GPIO_PIN1,
            GPIO_PRIMARY_MODULE_FUNCTION);

    // Select I2C function for I2C_SDA(P7.0 )
    GPIO_setAsPeripheralModuleFunctionOutputPin(
            GPIO_PORT_P7,
            GPIO_PIN0,
            GPIO_PRIMARY_MODULE_FUNCTION);
}


void HAL_I2C_config(void)
{
    // Initialize USCI_B2 and I2C Master to communicate with slave devices
    EUSCI_B_I2C_initMaster(EUSCI_B2_BASE, &i2cConfig);

    // Disable I2C module to make changes
    EUSCI_B_I2C_disable(EUSCI_B2_BASE);

    // Enable I2C Module to start operations
    EUSCI_B_I2C_enable(EUSCI_B2_BASE);
}


uint16_t HAL_I2C_read16(unsigned char pointer)
{
    uint8_t content_MSB = 0;
    uint8_t content_LSB = 0;
    uint16_t content = 0;

    // Set master to transmit mode
    EUSCI_B_I2C_setMode(EUSCI_B2_BASE,
        EUSCI_B_I2C_TRANSMIT_MODE);

    // Clear any existing interrupt flag
    EUSCI_B_I2C_clearInterrupt(EUSCI_B2_BASE,
        EUSCI_B_I2C_TRANSMIT_INTERRUPT0);

    // Wait until ready to write
    while (EUSCI_B_I2C_isBusBusy(EUSCI_B2_BASE));

    // Initiate start and send first character
    EUSCI_B_I2C_masterSendMultiByteStart(EUSCI_B2_BASE, pointer);

    // Wait for TX to finish
    while(!(EUSCI_B_I2C_getInterruptStatus(EUSCI_B2_BASE,
        EUSCI_B_I2C_TRANSMIT_INTERRUPT0)));

    // Initiate stop only
    EUSCI_B_I2C_masterSendMultiByteStop(EUSCI_B2_BASE);

    // Wait for Stop to finish
    while(!EUSCI_B_I2C_getInterruptStatus(EUSCI_B2_BASE,
        EUSCI_B_I2C_STOP_INTERRUPT));


    // Generate Start condition and set it to receive mode.
    // This sends out the slave address and continues to read
    // until you issue a STOP
    EUSCI_B_I2C_masterReceiveStart(EUSCI_B2_BASE);

    // Wait for RX buffer to fill
    while(!(EUSCI_B_I2C_getInterruptStatus(EUSCI_B2_BASE,
        EUSCI_B_I2C_RECEIVE_INTERRUPT0)));

    // Read first byte from I2C RX register
    content_MSB = EUSCI_B_I2C_masterReceiveMultiByteNext(EUSCI_B2_BASE);

    // Receive second byte then send STOP condition
    content_LSB = EUSCI_B_I2C_masterReceiveMultiByteFinish(EUSCI_B2_BASE);

    // Mount full content word
    content = (content_MSB << 8) | content_LSB;

    // Return content
    return content;
}


void HAL_I2C_write16 (unsigned char pointer, unsigned int writeByte)
{
    // Set master to transmit mode
    EUSCI_B_I2C_setMode(EUSCI_B2_BASE,
        EUSCI_B_I2C_TRANSMIT_MODE);

    // Clear any existing interrupt flag
    EUSCI_B_I2C_clearInterrupt(EUSCI_B2_BASE,
        EUSCI_B_I2C_TRANSMIT_INTERRUPT0);

    // Wait until ready to write
    while (EUSCI_B_I2C_isBusBusy(EUSCI_B2_BASE));

    // Initiate start and send first character
    EUSCI_B_I2C_masterSendMultiByteStart(EUSCI_B2_BASE,
        pointer);

    // Send the MSB to slave
    EUSCI_B_I2C_masterSendMultiByteNext(EUSCI_B2_BASE,
        (unsigned char)(writeByte>>8));

    // Send the LSB to slave
    EUSCI_B_I2C_masterSendMultiByteFinish(EUSCI_B2_BASE,
        (unsigned char)(writeByte&0xFF));

}


void HAL_I2C_setslave(unsigned int slaveAdr)
{
    // Specify I2C slave address
    EUSCI_B_I2C_setSlaveAddress(EUSCI_B2_BASE,
        slaveAdr);

    // Enable and clear the interrupt flag
    EUSCI_B_I2C_clearInterrupt(EUSCI_B2_BASE,
        EUSCI_B_I2C_TRANSMIT_INTERRUPT0 + EUSCI_B_I2C_RECEIVE_INTERRUPT0);
}
