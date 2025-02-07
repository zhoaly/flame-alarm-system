idf_component_register(SRCS "HTTP_weather.c" "HTTP_weather.h"
                    INCLUDE_DIRS "."
                    REQUIRES "driver" "esp_wifi" "esp_http_client" "json" "WiFi"
                    )
