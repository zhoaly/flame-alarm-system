idf_component_register(SRCS "gb2312_16.c" "LVGL_init_my.h""LVGL_init_my.c" "lvgl_demo_ui.c""lvgl_demo_ui.h" "lv_port_indev.c" "lv_port_indev.h"
#include "lvgl.h""
                    INCLUDE_DIRS "."
                    REQUIRES "esp_lcd" "lvgl" "esp_lvgl_port" "LCD""UART_MY" "WiFi" "button_MY"
                    )
