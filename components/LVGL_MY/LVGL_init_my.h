#ifndef LVGL_INIT_MY_H
#define LVGL_INIT_MY_H

#include"lvgl.h"
#include "esp_lvgl_port.h"

typedef struct {
    unsigned int MQ2_value;      // MQ2传感器值
    unsigned int flame_value;   // 火焰传感器值
    uint32_t time;              // 时间戳
} lvgl_Queue;


typedef enum {
    INDEX=1,
    LEFT,
    RIGHT,
    CONFIRM
}cmd_enum;
lv_disp_t* LVGL_Init_my();
void lvgl_style_init();
void lvgl_scr_init();
void lvgl_key_enter_my();

void lvgl_scroll_to_left_my();
void lvgl_scroll_to_right_my();

#endif