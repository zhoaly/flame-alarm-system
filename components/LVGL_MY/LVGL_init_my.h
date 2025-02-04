#ifndef LVGL_INIT_MY_H
#define LVGL_INIT_MY_H

#include"lvgl.h"
#include "esp_lvgl_port.h"

typedef enum {
    INDEX=1,
    LEFT,
    RIGHT,
    CONFIRM
}cmd_enum;
lv_disp_t* LVGL_Init_my();
void lvgl_style_init();
void lvgl_scr_init();
#endif