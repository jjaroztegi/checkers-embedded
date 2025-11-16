//*****************************************************************************
//
// lcd_backlight.c - LCD Backlight Control for MSP430FR5994 + BOOSTXL-EDUMKII
//
// Controls the LCD backlight brightness using PWM on P3.6 via Timer_B0.5
//
// P3.6 is connected to Timer_B0 and can be used for:
//   - TB0.CCI5A (capture/compare input)
//   - TB0.5 (compare output) <- Used for PWM
//
//*****************************************************************************

#include <driverlib.h>
#include <drivers/lcd_backlight.h>

// PWM Configuration
#define PWM_TIMER_PERIOD 100  // Timer period for PWM (determines frequency) - 100 = ~2.5kHz
#define PWM_TIMER_BASE __MSP430_BASEADDRESS_TB0__
#define PWM_CCR_REGISTER TIMER_B_CAPTURECOMPARE_REGISTER_5
#define MIN_LUX_THRESHOLD 2000  // Lux level where backlight reaches max

// Timer_B Up Mode Configuration for PWM
static Timer_B_initUpModeParam upModeParam = {
    TIMER_B_CLOCKSOURCE_SMCLK,            // SMCLK Clock Source (16MHz)
    TIMER_B_CLOCKSOURCE_DIVIDER_64,       // Divide by 64 = 250kHz
    PWM_TIMER_PERIOD,                     // 100 ticks = ~2.5kHz PWM frequency
    TIMER_B_TBIE_INTERRUPT_DISABLE,       // Disable Timer interrupt
    TIMER_B_CCIE_CCR0_INTERRUPT_DISABLE,  // Disable CCR0 interrupt
    TIMER_B_DO_CLEAR,                     // Clear value
    false                                 // Don't start timer yet
};

// Timer_B Compare Mode Configuration for PWM
static Timer_B_initCompareModeParam compareModeParam = {
    PWM_CCR_REGISTER,                          // Use CCR5
    TIMER_B_CAPTURECOMPARE_INTERRUPT_DISABLE,  // Disable CCR interrupt
    TIMER_B_OUTPUTMODE_RESET_SET,  // PWM mode: Reset/Set (active-LOW backlight)
    PWM_TIMER_PERIOD / 2           // 50% initial duty cycle
};

//*****************************************************************************
//
//! Initializes the LCD backlight PWM control
//
//*****************************************************************************
void LCD_BACKLIGHT_init(void) {
  // Configure P3.6 as Timer_B0.5 output (PWM for LCD backlight)
  // P3.6 maps to TB0.5 on MSP430FR5994 with PRIMARY module function
  GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P3, GPIO_PIN6,
                                              GPIO_PRIMARY_MODULE_FUNCTION);

  // Configure Timer_B0 in Up Mode
  Timer_B_initUpMode(PWM_TIMER_BASE, &upModeParam);

  // Initialize Compare Mode for PWM output on CCR5
  // Using RESET_SET mode for active-LOW backlight control
  Timer_B_initCompareMode(PWM_TIMER_BASE, &compareModeParam);

  // Start the timer
  Timer_B_startCounter(PWM_TIMER_BASE, TIMER_B_UP_MODE);
}

//*****************************************************************************
//
//! Sets the LCD backlight brightness
//
//*****************************************************************************
void LCD_BACKLIGHT_set_brightness(uint8_t brightness) {
  uint16_t dutyCycle;

  // Clamp brightness to 0-100 range
  if (brightness > 100) {
    brightness = 100;
  }

  // Calculate duty cycle: (brightness / 100) * PWM_TIMER_PERIOD
  dutyCycle = (uint16_t)((brightness * PWM_TIMER_PERIOD) / 100);

  // Update the compare value
  Timer_B_setCompareValue(PWM_TIMER_BASE, PWM_CCR_REGISTER, dutyCycle);
}

//*****************************************************************************
//
//! Adjusts backlight based on ambient light sensor reading
//
//*****************************************************************************
void LCD_BACKLIGHT_adjust_for_ambient(uint32_t lux) {
  uint16_t dutyCycle;

  // Algorithm:
  // - Below MIN_LUX_THRESHOLD: scale brightness based on ambient light
  // - Above MIN_LUX_THRESHOLD: use maximum brightness
  //
  // Formula: brightness = 10% + (lux / MIN_LUX_THRESHOLD) * 90%
  // This ensures minimum 10% brightness in darkness

  if (lux < MIN_LUX_THRESHOLD) {
    // dutyCycle = ((MIN_LUX_THRESHOLD * 0.1 + lux * 0.9) / MIN_LUX_THRESHOLD) *
    // PWM_TIMER_PERIOD
    dutyCycle = (uint16_t)(((MIN_LUX_THRESHOLD / 10 + (lux * 9) / 10) *
                            PWM_TIMER_PERIOD) /
                           MIN_LUX_THRESHOLD);
  } else {
    // Maximum brightness
    dutyCycle = PWM_TIMER_PERIOD;
  }

  // Update the compare value
  Timer_B_setCompareValue(PWM_TIMER_BASE, PWM_CCR_REGISTER, dutyCycle);
}
