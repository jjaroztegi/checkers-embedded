#ifndef __HAL_MSP_EXP430FR5994_CRYSTALFONTZLCD_H_
#define __HAL_MSP_EXP430FR5994_CRYSTALFONTZLCD_H_


#include <stdint.h>
#include "driverlib.h"
//*****************************************************************************
//
// User Configuration for the LCD Driver
//
//*****************************************************************************

// System clock speed (in Hz)
#define LCD_SYSTEM_CLOCK_SPEED                 16000000
// SPI clock speed (in Hz)
#define LCD_SPI_CLOCK_SPEED                    16000000

// Ports from MSP430 connected to LCD
#define LCD_SCK_PORT          GPIO_PORT_P5
#define LCD_SCK_PIN_FUNCTION  GPIO_PRIMARY_MODULE_FUNCTION
#define LCD_MOSI_PORT         GPIO_PORT_P5
#define LCD_MOSI_PIN_FUNCTION GPIO_PRIMARY_MODULE_FUNCTION
#define LCD_RST_PORT          GPIO_PORT_P5
#define LCD_CS_PORT           GPIO_PORT_P8
#define LCD_DC_PORT           GPIO_PORT_P4

// Pins from MSP432 connected to LCD
#define LCD_SCK_PIN           GPIO_PIN2
#define LCD_MOSI_PIN          GPIO_PIN0
#define LCD_RST_PIN           GPIO_PIN3
#define LCD_CS_PIN            GPIO_PIN3
#define LCD_DC_PIN            GPIO_PIN1

// Definition of USCI base address to be used for SPI communication
#define LCD_EUSCI_BASE        EUSCI_B1_BASE

//*****************************************************************************
//
// Prototypes for the globals exported by this driver.
//
//*****************************************************************************
extern void HAL_LCD_writeCommand(uint8_t command);
extern void HAL_LCD_writeData(uint8_t data);
extern void HAL_LCD_init_gpio(void);
extern void HAL_LCD_config(void);


#define HAL_LCD_delay(x)      __delay_cycles(x * 16)

#endif /* HAL_MSP_EXP430FR5994_CRYSTALFONTZ128X128_ST7735_H_ */
