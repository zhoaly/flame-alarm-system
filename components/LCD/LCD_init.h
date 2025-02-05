#ifndef LCD_INIT_H
#define LCD_INIT_H

#define EXAMPLE_LCD_PIXEL_CLOCK_HZ    (200 * 1000)
#define EXAMPLE_PIN_NUM_SDA           11
#define EXAMPLE_PIN_NUM_SCL           12
#define EXAMPLE_PIN_NUM_RST           -1
#define EXAMPLE_I2C_HW_ADDR           0x3C

// The pixel number in horizontal and vertical
#define EXAMPLE_LCD_H_RES              128
#define EXAMPLE_LCD_V_RES              64

// Bit number used to represent command and parameter
#define EXAMPLE_LCD_CMD_BITS           8
#define EXAMPLE_LCD_PARAM_BITS         8


#define I2C_BUS_PORT  0



void LCD_Init();
#endif