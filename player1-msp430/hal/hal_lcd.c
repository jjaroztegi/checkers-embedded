
//*****************************************************************************
//
// HAL_MSP_EXP430FR5994_Crystalfontz128x128_ST7735.c -
//           Hardware abstraction layer for using the Educational Boosterpack's
//           Crystalfontz128x128 LCD with MSP-EXP432P401R LaunchPad
//
//*****************************************************************************

#include <msp430.h>
#include <driverlib.h>
#include <hal/hal_lcd.h>

void HAL_LCD_init_gpio(void)
{
    // LCD_SCK
    GPIO_setAsPeripheralModuleFunctionOutputPin(LCD_SCK_PORT, LCD_SCK_PIN, GPIO_PRIMARY_MODULE_FUNCTION);
    // LCD_MOSI
    GPIO_setAsPeripheralModuleFunctionOutputPin(LCD_MOSI_PORT, LCD_MOSI_PIN, GPIO_PRIMARY_MODULE_FUNCTION);
    // LCD_RST
    GPIO_setAsOutputPin(LCD_RST_PORT, LCD_RST_PIN);
    GPIO_setOutputLowOnPin(LCD_RST_PORT, LCD_RST_PIN);
    // LCD_RS
    GPIO_setAsOutputPin(LCD_DC_PORT, LCD_DC_PIN);
    GPIO_setOutputHighOnPin(LCD_DC_PORT, LCD_DC_PIN);
    // LCD_CS
    GPIO_setAsOutputPin(LCD_CS_PORT, LCD_CS_PIN);
    GPIO_setOutputLowOnPin(LCD_CS_PORT, LCD_CS_PIN);

}

void HAL_LCD_config(void)
{
    EUSCI_B_SPI_initMasterParam config =
        {
            EUSCI_B_SPI_CLOCKSOURCE_SMCLK,
            LCD_SYSTEM_CLOCK_SPEED,
            LCD_SPI_CLOCK_SPEED,
            EUSCI_B_SPI_MSB_FIRST,
            EUSCI_B_SPI_PHASE_DATA_CAPTURED_ONFIRST_CHANGED_ON_NEXT,
            EUSCI_B_SPI_CLOCKPOLARITY_INACTIVITY_LOW,
            EUSCI_B_SPI_3PIN
        };
    EUSCI_B_SPI_initMaster(LCD_EUSCI_BASE, &config);
    EUSCI_B_SPI_enable(LCD_EUSCI_BASE);
}


//*****************************************************************************
//
// Writes a command to the CFAF128128B-0145T.  This function implements the basic SPI
// interface to the LCD display.
//
//*****************************************************************************
void HAL_LCD_writeCommand(uint8_t command)
{
    // Set to command mode
    GPIO_setOutputLowOnPin(LCD_DC_PORT, LCD_DC_PIN);

    // USCI_B1 Busy?
    while (UCB1STATW & UCBUSY);

    // Transmit data
    UCB1TXBUF = command;

    // USCI_B1 Busy?
    while (UCB1STATW & UCBUSY);

    // Set back to data mode
    GPIO_setOutputHighOnPin(LCD_DC_PORT, LCD_DC_PIN);
}


//*****************************************************************************
//
// Writes a data to the CFAF128128B-0145T.  This function implements the basic SPI
// interface to the LCD display.
//
//*****************************************************************************
void HAL_LCD_writeData(uint8_t data)
{
    // USCI_B1 Busy?
    while (UCB1STATW & UCBUSY);

    // Transmit data
    UCB1TXBUF = data;

    // USCI_B1 Busy?
    while (UCB1STATW & UCBUSY);
}

