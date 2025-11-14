#ifndef LCD_BACKLIGHT_H_
#define LCD_BACKLIGHT_H_

#include <stdint.h>

//*****************************************************************************
//
// LCD Backlight Control using PWM
// Backlight is connected to P3.6 on MSP430FR5994 + BOOSTXL-EDUMKII
// Controlled via Timer_B0, channel 5 (TB0.5)
//
// P3.6 Pin Functions:
//   - TB0.CCI5A (capture/compare input)
//   - TB0.5 (compare output) <- Used for PWM generation
//
// Note: Backlight is active-LOW, so inverted PWM (SET_RESET) is used
//
//*****************************************************************************

//*****************************************************************************
//
//! \brief Initializes the LCD backlight PWM control
//!
//! This function configures Timer_B0 to generate a PWM signal on P3.6 (TB0.5)
//! to control the LCD backlight brightness. Uses inverted PWM (SET_RESET mode)
//! because the backlight circuit is active-LOW.
//!
//! \return None
//
//*****************************************************************************
extern void LCD_BACKLIGHT_init(void);

//*****************************************************************************
//
//! \brief Sets the LCD backlight brightness
//!
//! \param brightness A value from 0 (off) to 100 (full brightness)
//!
//! Sets the PWM duty cycle to control backlight brightness.
//! 0 = completely off, 100 = maximum brightness
//!
//! \return None
//
//*****************************************************************************
extern void LCD_BACKLIGHT_set_brightness(uint8_t brightness);

//*****************************************************************************
//
//! \brief Adjusts backlight based on ambient light sensor reading
//!
//! \param lux Ambient light level in lux from OPT3001 sensor
//!
//! Automatically adjusts the LCD backlight brightness based on ambient
//! light conditions. In low light, reduces brightness. In bright light,
//! increases brightness to maintain visibility.
//!
//! \return None
//
//*****************************************************************************
extern void LCD_BACKLIGHT_adjust_for_ambient(uint32_t lux);

#endif /* LCD_BACKLIGHT_H_ */
