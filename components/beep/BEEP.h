#ifndef _BEEP_H
#define _BEEP_H

#include "driver/ledc.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


#define BEEP_TIMER              LEDC_TIMER_0
#define BEEP_MODE               LEDC_LOW_SPEED_MODE
#define BEEP_OUTPUT_IO          (21) // Define the output GPIO
#define BEEP_CHANNEL            LEDC_CHANNEL_1
#define BEEP_DUTY_RES           LEDC_TIMER_13_BIT // Set duty resolution to 13 bits
#define BEEP_DUTY               (8000) // Set duty to 50%. (2 ** 13) * 50% = 4096
#define BEEP_FREQUENCY          (4000) // Frequency in Hertz. Set frequency at 4 kHz


void BEEP_init(void);
void BEEP_Set_duty(uint32_t duty);

#endif