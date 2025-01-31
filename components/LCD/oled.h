#ifndef __OLED_H
#define __OLED_H 

#include "LCD_init.h"
#include <stdio.h>
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/i2c_master.h"

#include "stdlib.h"	

#ifndef u8
#define u8 uint8_t
#endif

#ifndef u16
#define u16 uint16_t
#endif

#ifndef u32
#define u32 uint32_t
#endif


//-----------------LCD端口移植---------------- 

#define LCD_SCL_PIN     1
#define LCD_MOSI_PIN    2
#define LCD_RES_PIN     3
#define LCD_DC_PIN      4
#define LCD_CS_PIN      5

//-----------------OLED端口定义---------------- 



#define OLED_CMD  0	//写命令
#define OLED_DATA 1	//写数据


void OLED_ClearPoint(u8 x,u8 y);
void OLED_ColorTurn(esp_lcd_panel_handle_t panel);
void OLED_DisplayTurn(esp_lcd_panel_handle_t panel);
void OLED_DisPlay_On(esp_lcd_panel_handle_t panel);
void OLED_DisPlay_Off(esp_lcd_panel_handle_t panel);
void OLED_Refresh(esp_lcd_panel_handle_t panel);
void OLED_Clear_GRAM();
void OLED_Clear(esp_lcd_panel_handle_t panel);

void OLED_DrawPoint_To_GRAM(u8 x,u8 y);
void OLED_CleanPoint_To_GRAM(u8 x,u8 y);

void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2);
void OLED_DrawCircle(u8 x,u8 y,u8 r);

void OLED_ShowChar(u8 x,u8 y,u8 chr,u8 size1);
void OLED_ShowChar6x8(u8 x,u8 y,u8 chr);
void OLED_ShowString(u8 x,u8 y,u8 *chr,u8 size1);
void OLED_ShowNum(uint8_t x,uint8_t y,float num,uint8_t float_len,uint16_t font_size);




#endif

