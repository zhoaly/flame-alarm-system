idf_component_register(SRCS "sntp_set_time.c" "sntp_set_time.h"
                    INCLUDE_DIRS "."
                    REQUIRES "driver" "esp_wifi" "lwip"
                    )
