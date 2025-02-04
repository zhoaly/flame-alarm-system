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

static const char *TAG = "main";

// 任务和同步句柄
TaskHandle_t MQ2Handle;
TaskHandle_t BEEPHandle;
TaskHandle_t FLAMEHandle;

SemaphoreHandle_t OLEDMutexHandle;
QueueHandle_t OLEDQueuehandle;

// OLED队列的数据结构
typedef struct {
    unsigned int MQ2_value;      // MQ2传感器值
    unsigned int flame_value;   // 火焰传感器值
    uint32_t time;              // 时间戳
} OLED_Queue;

// 蜂鸣器控制标志
bool flag_beep = 0;


// 控制蜂鸣器行为的任务
void beep_task() {
    while (1) {
        if (flag_beep == 1) {
            BEEP_Set_duty(4000);  // 激活蜂鸣器
        } else {
            BEEP_Set_duty(0);    // 关闭蜂鸣器
        }
        vTaskDelay(pdMS_TO_TICKS(300));
        BEEP_Set_duty(0);        // 确保蜂鸣器关闭
        vTaskDelay(pdMS_TO_TICKS(300));
    }
}

// 读取传感器数据并发送到OLED队列的任务
void ADC_task() {
    OLED_Queue adc;

    while (1) {
        adc.MQ2_value = Get_MQ2_Percentage_value();   // 获取MQ2传感器的百分比值
        adc.flame_value = Get_Flame_Percentage_value(); // 获取火焰传感器的百分比值
        adc.time = xTaskGetTickCount();              // 获取当前任务的滴答计数

        // 将传感器数据发送到OLED队列
        xQueueSend(OLEDQueuehandle, &adc, 0);

        // 判断是否需要激活蜂鸣器
        if (adc.MQ2_value > 50 || adc.flame_value < 90) {
            flag_beep = 1;
        } else {
            flag_beep = 0;
        }

        vTaskDelay(10);
    }
}

// 主应用入口
void app_main(void) {
    // 初始化外设
    LCD_Init();
    BEEP_init();
    ledc_init();
    ADC_Init();
    uart_init_my();
    LVGL_Init_my();
    
    lvgl_demo_ui();
    // 创建队列和互斥量
    
    OLEDQueuehandle = xQueueCreate(10, sizeof(OLED_Queue));


    // 创建任务
    xTaskCreatePinnedToCore(ADC_task, "ADC_task", 4096, NULL, 2, &MQ2Handle, 1);
    xTaskCreatePinnedToCore(beep_task, "beep_task", 4096, NULL, 3, &BEEPHandle, 1);

}
