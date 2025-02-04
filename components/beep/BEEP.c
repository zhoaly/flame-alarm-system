#include "BEEP.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

bool flag_beep = 0;
TaskHandle_t BEEPHandle;

void BEEP_init(void)
{
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = BEEP_MODE,
        .duty_resolution  = BEEP_DUTY_RES,
        .timer_num        = BEEP_TIMER,
        .freq_hz          = BEEP_FREQUENCY,  // Set output frequency at 4 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = BEEP_MODE,
        .channel        = BEEP_CHANNEL,
        .timer_sel      = BEEP_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = BEEP_OUTPUT_IO,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0,
        .flags.output_invert=0
    };

    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));
    xTaskCreatePinnedToCore(beep_task, "beep_task", 4096, NULL, 3, &BEEPHandle, 1);

}

void BEEP_Set_duty(uint32_t duty){


    ESP_ERROR_CHECK(ledc_set_duty(BEEP_MODE, BEEP_CHANNEL, duty));
    ESP_ERROR_CHECK(ledc_update_duty(BEEP_MODE, BEEP_CHANNEL));

}


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

