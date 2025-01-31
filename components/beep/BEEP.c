#include "BEEP.h"

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



    /* 初始化渐变模式 */

}

void BEEP_Set_duty(uint32_t duty){


    ESP_ERROR_CHECK(ledc_set_duty(BEEP_MODE, BEEP_CHANNEL, duty));
    ESP_ERROR_CHECK(ledc_update_duty(BEEP_MODE, BEEP_CHANNEL));

}

