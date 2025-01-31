idf_component_register(SRCS "bsp_mq2.h""bsp_mq2.c"
                    INCLUDE_DIRS "."
                    REQUIRES "driver" "esp_timer" "esp_adc" "nvs_flash"
                    )
