#ifndef _BUTTON_MY_H
#define _BUTTON_MY_H


#include "button_gpio.h"
#include "iot_button.h"
#include "esp_log.h"
#include "LVGL_init_my.h"


void button_my_init(button_handle_t *gpio_btn,int32_t gpio_num,int8_t active_level);
void button_my_init_all();
bool button_my_is_press(uint8_t id,
        button_handle_t* gpio_btn1,
        button_handle_t* gpio_btn2,
        button_handle_t* gpio_btn3);//传入三个按键句柄，判断是否按下 

#endif