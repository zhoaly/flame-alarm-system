#include "LED.h"

int cnt;
static bool cb_ledc_fade_end_event(const ledc_cb_param_t *param, void *user_arg)//回调函数
{
    BaseType_t taskAwoken = pdFALSE;
 
    if (param->event == LEDC_FADE_END_EVT) {

        ledc_set_fade_with_step(LEDC_MODE, LEDC_CHANNEL, cnt == 0 ? 0 : 8000, 100,100);
        ledc_fade_start(LEDC_MODE, LEDC_CHANNEL, LEDC_FADE_NO_WAIT);
        cnt = (cnt + 1) % 2;

    }
    return (taskAwoken == pdTRUE);
}



void ledc_init(void)
{
    // Prepare and then apply the LEDC PWM timer configuration
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .duty_resolution  = LEDC_DUTY_RES,
        .timer_num        = LEDC_TIMER,
        .freq_hz          = LEDC_FREQUENCY,  // Set output frequency at 4 kHz
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // Prepare and then apply the LEDC PWM channel configuration
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = LEDC_OUTPUT_IO,
        .duty           = 0, // Set duty to 0%
        .hpoint         = 0,
        .flags.output_invert=0
    };

    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));



    /* 初始化渐变模式 */

    ledc_fade_func_install(0);

    ledc_cbs_t callbacks = {
        .fade_cb = cb_ledc_fade_end_event
    };
    ledc_cb_register(LEDC_MODE, LEDC_CHANNEL, &callbacks, NULL);


    ledc_set_fade_with_step(LEDC_MODE,LEDC_CHANNEL,8000,100,100);
    ledc_fade_start(LEDC_MODE, LEDC_CHANNEL,LEDC_FADE_NO_WAIT);

}



