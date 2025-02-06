#include <stdio.h>
#include "LCD_init.h"
#include "oled.h"
#include "LED.h"
#include "bsp_mq2.h"
#include "BEEP.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_err.h"
#include "esp_log.h"

#include "driver/i2c_master.h"
#include "driver/gpio.h"
#include "esp_lcd_panel_vendor.h"

#include "UART_init_my.h"
#include "LVGL_init_my.h"
#include "lvgl_demo_ui.h"
#include "WiFi_my.h"
#include "sntp_set_time.h"

static const char *TAG = "main";

// 主应用入口
void app_main(void) {


    // 初始化外设
    LCD_Init();
    BEEP_init();
    ledc_init();
    ADC_Init();
    uart_init_my();
    LVGL_Init_my();

    lvgl_lodding();//lodding界面等待初始化完成

    wifi_init_sta();
    sntp_set_time();

    
    lvgl_demo_ui();
    // 创建队列和互斥量

    //OLEDQueuehandle = xQueueCreate(10, sizeof(OLED_Queue));



}
