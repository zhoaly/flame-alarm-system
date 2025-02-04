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

extern esp_lcd_panel_handle_t panel_handle;
extern esp_lcd_panel_io_handle_t io_handle;

uint8_t temp[8][128];

// 任务和同步句柄
TaskHandle_t OLEDHandle;
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

// 更新OLED显示内容的任务
void OLED_task() {
    static uint32_t temp_mq2_value = 0;
    static uint32_t temp_mq2_time = 0;
    static uint32_t temp_flame_value = 0;
    OLED_Queue OLED_receive;
    uint8_t string[] = "FireAlarm System";

    while (1) {
        xSemaphoreTake(OLEDMutexHandle, portMAX_DELAY);

        // 清除OLED显示
        OLED_Clear_GRAM(panel_handle);

        // 显示标题
        OLED_ShowString(0, 0, string, 16);

        // 从队列接收传感器数据
        xQueueReceive(OLEDQueuehandle, &OLED_receive, 0);
        temp_mq2_value = OLED_receive.MQ2_value;
        temp_mq2_time = OLED_receive.time;
        temp_flame_value = OLED_receive.flame_value;

        // 在OLED上显示传感器数据
        OLED_ShowString(0, 28, &"MQ2Value:", 12);
        OLED_ShowString(0, 40, &"FlameValue:", 12);
        OLED_ShowString(0, 52, &"Receivetime:", 12);

        OLED_ShowNum(80, 28, (float)temp_mq2_value, 0, 12);
        OLED_ShowNum(80, 40, 100 - temp_flame_value, 0, 12);
        OLED_ShowNum(80, 52, temp_mq2_time, 0, 12);

        OLED_Refresh(panel_handle);

        vTaskDelay(10);
        xSemaphoreGive(OLEDMutexHandle);
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
    OLEDMutexHandle = xSemaphoreCreateMutex();

    xSemaphoreGive(OLEDMutexHandle);

    // 创建任务
    //xTaskCreatePinnedToCore(OLED_task, "OLED_task", 4096, NULL, 3, &OLEDHandle, 1);
    xTaskCreatePinnedToCore(ADC_task, "ADC_task", 4096, NULL, 2, &MQ2Handle, 1);
    xTaskCreatePinnedToCore(beep_task, "beep_task", 4096, NULL, 3, &BEEPHandle, 1);
    //xTaskCreatePinnedToCore(flame_task, "flame_task", 4096, NULL, 3, &FLAMEHandle, 1);
}
