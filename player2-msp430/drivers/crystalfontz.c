
#include <hal/hal_lcd.h>
#include <driverlib.h>
#include <grlib.h>
#include <drivers/crystalfontz.h>
#include <stdint.h>

uint8_t Lcd_Orientation;
uint16_t Lcd_ScreenWidth, Lcd_ScreenHeigth;
uint8_t Lcd_PenSolid, Lcd_FontSolid, Lcd_FlagRead;
uint16_t Lcd_TouchTrim;

void CRYSTALFONTZ_init(void)
{
    GPIO_setOutputLowOnPin(LCD_RST_PORT, LCD_RST_PIN);
    HAL_LCD_delay(50);
    GPIO_setOutputHighOnPin(LCD_RST_PORT, LCD_RST_PIN);
    HAL_LCD_delay(120);

    HAL_LCD_writeCommand(CM_SLPOUT);
    HAL_LCD_delay(200);

    HAL_LCD_writeCommand(CM_GAMSET);
    HAL_LCD_writeData(0x04);

    HAL_LCD_writeCommand(CM_SETPWCTR);
    HAL_LCD_writeData(0x0A);
    HAL_LCD_writeData(0x14);

    HAL_LCD_writeCommand(CM_SETSTBA);
    HAL_LCD_writeData(0x0A);
    HAL_LCD_writeData(0x00);

    HAL_LCD_writeCommand(CM_COLMOD);
    HAL_LCD_writeData(0x05);
    HAL_LCD_delay(10);

    HAL_LCD_writeCommand(CM_MADCTL);
    HAL_LCD_writeData(CM_MADCTL_BGR);

    HAL_LCD_writeCommand(CM_NORON);

    Lcd_ScreenWidth  = LCD_VERTICAL_MAX;
    Lcd_ScreenHeigth = LCD_HORIZONTAL_MAX;
    Lcd_PenSolid  = 0;
    Lcd_FontSolid = 1;
    Lcd_FlagRead  = 0;
    Lcd_TouchTrim = 0;

    CRYSTALFONTZ_set_draw_frame(0, 0, 127, 127);
    HAL_LCD_writeCommand(CM_RAMWR);
    int i;
    for (i = 0; i < 16384; i++)
    {
        HAL_LCD_writeData(0xFF);
        HAL_LCD_writeData(0xFF);
    }

    HAL_LCD_delay(10);
    HAL_LCD_writeCommand(CM_DISPON);
}


void CRYSTALFONTZ_set_draw_frame(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    switch (Lcd_Orientation) {
        case 0:
            x0 += 2;
            y0 += 3;
            x1 += 2;
            y1 += 3;
            break;
        case 1:
            x0 += 3;
            y0 += 2;
            x1 += 3;
            y1 += 2;
            break;
        case 2:
            x0 += 2;
            y0 += 1;
            x1 += 2;
            y1 += 1;
            break;
        case 3:
            x0 += 1;
            y0 += 2;
            x1 += 1;
            y1 += 2;
            break;
        default:
            break;
    }

    HAL_LCD_writeCommand(CM_CASET);
    HAL_LCD_writeData((uint8_t)(x0 >> 8));
    HAL_LCD_writeData((uint8_t)(x0));
    HAL_LCD_writeData((uint8_t)(x1 >> 8));
    HAL_LCD_writeData((uint8_t)(x1));

    HAL_LCD_writeCommand(CM_RASET);
    HAL_LCD_writeData((uint8_t)(y0 >> 8));
    HAL_LCD_writeData((uint8_t)(y0));
    HAL_LCD_writeData((uint8_t)(y1 >> 8));
    HAL_LCD_writeData((uint8_t)(y1));
}

void CRYSTALFONTZ_set_orientation(uint8_t orientation)
{
    Lcd_Orientation = orientation;
    HAL_LCD_writeCommand(CM_MADCTL);
    switch (Lcd_Orientation) {
        case LCD_ORIENTATION_UP:
            HAL_LCD_writeData(CM_MADCTL_MX | CM_MADCTL_MY | CM_MADCTL_BGR);
            break;
        case LCD_ORIENTATION_LEFT:
            HAL_LCD_writeData(CM_MADCTL_MY | CM_MADCTL_MV | CM_MADCTL_BGR);
            break;
        case LCD_ORIENTATION_DOWN:
            HAL_LCD_writeData(CM_MADCTL_BGR);
            break;
        case LCD_ORIENTATION_RIGHT:
            HAL_LCD_writeData(CM_MADCTL_MX | CM_MADCTL_MV | CM_MADCTL_BGR);
            break;
    }
}


static void CRYSTALFONTZ_pixel_draw(void *pvDisplayData,
                                 int16_t lX,
                                 int16_t lY,
                                 uint16_t ulValue)
{

    CRYSTALFONTZ_set_draw_frame(lX,lY,lX,lY);

    //
    // Write the pixel value.
    //
    HAL_LCD_writeCommand(CM_RAMWR);
    HAL_LCD_writeData(ulValue>>8);
    HAL_LCD_writeData(ulValue);
}

static void CRYSTALFONTZ_pixel_draw_multiple(void *pvDisplayData,
                                    int16_t lX,
                                    int16_t lY,
                                    int16_t lX0,
                                    int16_t lCount,
                                    int16_t lBPP,
                                    const uint8_t *pucData,
                                    const uint32_t *pucPalette)
{
    uint16_t Data;

    //
    // Set the cursor increment to left to right, followed by top to bottom.
    //
    CRYSTALFONTZ_set_draw_frame(lX,lY,lX+lCount,127);
    HAL_LCD_writeCommand(CM_RAMWR);

    //
    // Determine how to interpret the pixel data based on the number of bits
    // per pixel.
    //
    switch(lBPP)
    {
        // The pixel data is in 1 bit per pixel format
        case 1:
        {
            // Loop while there are more pixels to draw
            while(lCount > 0)
            {
                // Get the next byte of image data
                Data = *pucData++;

                // Loop through the pixels in this byte of image data
                for(; (lX0 < 8) && lCount; lX0++, lCount--)
                {
                    // Draw this pixel in the appropriate color
                    HAL_LCD_writeData((((uint32_t *)pucPalette)[(Data >>
                                                             (7 - lX0)) & 1])>>8);
                    HAL_LCD_writeData(((uint32_t *)pucPalette)[(Data >>
                                                             (7 - lX0)) & 1]);
                }

                // Start at the beginning of the next byte of image data
                lX0 = 0;
            }
            // The image data has been drawn

            break;
        }

        // The pixel data is in 4 bit per pixel format
        case 4:
        {
            // Loop while there are more pixels to draw.  "Duff's device" is
            // used to jump into the middle of the loop if the first nibble of
            // the pixel data should not be used.  Duff's device makes use of
            // the fact that a case statement is legal anywhere within a
            // sub-block of a switch statement.  See
            // http://en.wikipedia.org/wiki/Duff's_device for detailed
            // information about Duff's device.
            switch(lX0 & 1)
            {
                case 0:

                    while(lCount)
                    {
                        // Get the upper nibble of the next byte of pixel data
                        // and extract the corresponding entry from the palette
                        Data = (*pucData >> 4);
                        Data = (*(uint16_t *)(pucPalette + Data));
                        // Write to LCD screen
                        HAL_LCD_writeData(Data>>8);
                        HAL_LCD_writeData(Data);

                        // Decrement the count of pixels to draw
                        lCount--;

                        // See if there is another pixel to draw
                        if(lCount)
                        {
                case 1:
                            // Get the lower nibble of the next byte of pixel
                            // data and extract the corresponding entry from
                            // the palette
                            Data = (*pucData++ & 15);
                            Data = (*(uint16_t *)(pucPalette + Data));
                            // Write to LCD screen
                            HAL_LCD_writeData(Data>>8);
                            HAL_LCD_writeData(Data);

                            // Decrement the count of pixels to draw
                            lCount--;
                        }
                    }
            }
            // The image data has been drawn.

            break;
        }

        // The pixel data is in 8 bit per pixel format
        case 8:
        {
            // Loop while there are more pixels to draw
            while(lCount--)
            {
                // Get the next byte of pixel data and extract the
                // corresponding entry from the palette
                Data = *pucData++;
                Data = (*(uint16_t *)(pucPalette + Data));
                // Write to LCD screen
                HAL_LCD_writeData(Data>>8);
                HAL_LCD_writeData(Data);
            }
            // The image data has been drawn
            break;
        }

        //
        // We are being passed data in the display's native format.  Merely
        // write it directly to the display.  This is a special case which is
        // not used by the graphics library but which is helpful to
        // applications which may want to handle, for example, JPEG images.
        //
        case 16:
        {
            uint16_t usData;

            // Loop while there are more pixels to draw.

            while(lCount--)
            {
                // Get the next byte of pixel data and extract the
                // corresponding entry from the palette
                usData = *((uint16_t *)pucData);
                pucData += 2;

                // Translate this palette entry and write it to the screen
                HAL_LCD_writeData(usData>>8);
                HAL_LCD_writeData(usData);
            }
        }
    }
}

static void CRYSTALFONTZ_line_draw_h(void *pvDisplayData,
                                 int16_t lX1,
                                 int16_t lX2,
                                 int16_t lY,
                                 uint16_t ulValue)
{


    CRYSTALFONTZ_set_draw_frame(lX1, lY, lX2, lY);

    //
    // Write the pixel value.
    //
    int16_t i;
    HAL_LCD_writeCommand(CM_RAMWR);
    for (i = lX1; i <= lX2; i++)
    {
        HAL_LCD_writeData(ulValue>>8);
        HAL_LCD_writeData(ulValue);
    }
}

static void CRYSTALFONTZ_line_draw_v(void *pvDisplayData,
                                 int16_t lX,
                                 int16_t lY1,
                                 int16_t lY2,
                                 uint16_t ulValue)
{
    CRYSTALFONTZ_set_draw_frame(lX, lY1, lX, lY2);

    //
    // Write the pixel value.
    //
    int16_t i;
    HAL_LCD_writeCommand(CM_RAMWR);
    for (i = lY1; i <= lY2; i++)
    {
        HAL_LCD_writeData(ulValue>>8);
        HAL_LCD_writeData(ulValue);
    }
}

static void CRYSTALFONTZ_rect_fill(void *pvDisplayData,
                                const Graphics_Rectangle *pRect,
                                uint16_t ulValue)
{
    int16_t x0 = pRect->sXMin;
    int16_t x1 = pRect->sXMax;
    int16_t y0 = pRect->sYMin;
    int16_t y1 = pRect->sYMax;

    CRYSTALFONTZ_set_draw_frame(x0, y0, x1, y1);

    //
    // Write the pixel value.
    //
    int16_t i;
    int16_t pixels = (x1 - x0 + 1) * (y1 - y0 + 1);
    HAL_LCD_writeCommand(CM_RAMWR);
    for (i = 0; i <= pixels; i++)
    {
        HAL_LCD_writeData(ulValue>>8);
        HAL_LCD_writeData(ulValue);
    }
}

static uint32_t CRYSTALFONTZ_color_translate(void *pvDisplayData,
                                          uint32_t ulValue)
{
    //
    // Translate from a 24-bit RGB color to a 5-6-5 RGB color.
    //
    return(((((ulValue) & 0x00f80000) >> 8) |
            (((ulValue) & 0x0000fc00) >> 5) |
            (((ulValue) & 0x000000f8) >> 3)));
}


static void CRYSTALFONTZ_flush(void *pvDisplayData)
{
    //
    // There is nothing to be done.
    //
}


static void CRYSTALFONTZ_clear_screen(void *pvDisplayData,
                                   uint16_t ulValue)
{
    Graphics_Rectangle rect = { 0, 0, LCD_VERTICAL_MAX-1, LCD_VERTICAL_MAX-1};
    CRYSTALFONTZ_rect_fill(pvDisplayData, &rect, ulValue);
}


Graphics_Display g_sCRYSTALFONTZ =
{
    sizeof(Graphics_Display),
    0,
    LCD_VERTICAL_MAX,
    LCD_HORIZONTAL_MAX,
    CRYSTALFONTZ_pixel_draw,
    CRYSTALFONTZ_pixel_draw_multiple,
    CRYSTALFONTZ_line_draw_h,
    CRYSTALFONTZ_line_draw_v,
    CRYSTALFONTZ_rect_fill,
    CRYSTALFONTZ_color_translate,
    CRYSTALFONTZ_flush,
    CRYSTALFONTZ_clear_screen
};
