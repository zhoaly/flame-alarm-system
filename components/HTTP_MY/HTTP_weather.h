#ifndef _HTTP_MY
#define _HTTP_MY

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

#include "esp_http_client.h"
#include "cJSON.h"



void HTTP_weather_init_my(void);
void parse_json(const char *json_data) ;

#endif