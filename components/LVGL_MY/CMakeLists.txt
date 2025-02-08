idf_component_register(SRCS "LVGL_init_my.h""LVGL_init_my.c" "lvgl_demo_ui.c""lvgl_demo_ui.h"
                    INCLUDE_DIRS "."
                    REQUIRES "esp_lcd" "lvgl" "esp_lvgl_port" "LCD""UART_MY" "WiFi"
                    )
