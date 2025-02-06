idf_component_register(SRCS "WiFi_my.c" "WiFi_my.h"
                    INCLUDE_DIRS "."
                    REQUIRES "driver" "esp_wifi" "lwip" "nvs_flash"
                    )
