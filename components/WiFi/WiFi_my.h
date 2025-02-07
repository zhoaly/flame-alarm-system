#ifndef _WIFI_MY
#define _WIFI_MY

#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sys.h"


#define CONFIG_ESP_WIFI_SSID "Xiaomi_7F5C"
#define CONFIG_ESP_WIFI_PASSWORD "08080808"
#define CONFIG_ESP_MAXIMUM_RETRY 5
#define CONFIG_ESP_WIFI_PW_ID ""

#define CONFIG_ESP_WPA3_SAE_PWE_BOTH 1
#define CONFIG_ESP_WIFI_AUTH_WPA2_PSK 1


/* WiFi 配置信息，可在菜单配置工具中进行设置，或者直接在这里修改 */
#define EXAMPLE_ESP_WIFI_SSID      CONFIG_ESP_WIFI_SSID  // WiFi SSID
#define EXAMPLE_ESP_WIFI_PASS      CONFIG_ESP_WIFI_PASSWORD  // WiFi 密码
#define EXAMPLE_ESP_MAXIMUM_RETRY  CONFIG_ESP_MAXIMUM_RETRY  // 最大重试次数

/* 配置 WPA3 认证模式，根据用户配置选择合适的模式 */
#if CONFIG_ESP_WPA3_SAE_PWE_HUNT_AND_PECK
#define ESP_WIFI_SAE_MODE WPA3_SAE_PWE_HUNT_AND_PECK  // WPA3 认证模式（Hunt and Peck）
#define EXAMPLE_H2E_IDENTIFIER ""
#elif CONFIG_ESP_WPA3_SAE_PWE_HASH_TO_ELEMENT
#define ESP_WIFI_SAE_MODE WPA3_SAE_PWE_HASH_TO_ELEMENT  // WPA3 认证模式（Hash to Element）
#define EXAMPLE_H2E_IDENTIFIER CONFIG_ESP_WIFI_PW_ID
#elif CONFIG_ESP_WPA3_SAE_PWE_BOTH
#define ESP_WIFI_SAE_MODE WPA3_SAE_PWE_BOTH  // WPA3 认证模式（支持两种方法）
#define EXAMPLE_H2E_IDENTIFIER CONFIG_ESP_WIFI_PW_ID
#endif

/* 设定 WiFi 认证模式门槛 */
#if CONFIG_ESP_WIFI_AUTH_OPEN
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_OPEN  // 允许开放网络
#elif CONFIG_ESP_WIFI_AUTH_WEP
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WEP  // 允许 WEP 加密
#elif CONFIG_ESP_WIFI_AUTH_WPA_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_PSK  // 允许 WPA-PSK 加密
#elif CONFIG_ESP_WIFI_AUTH_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK  // 允许 WPA2-PSK 加密
#elif CONFIG_ESP_WIFI_AUTH_WPA_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_WPA2_PSK  // 允许 WPA/WPA2 混合加密
#elif CONFIG_ESP_WIFI_AUTH_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA3_PSK  // 允许 WPA3-PSK 加密
#elif CONFIG_ESP_WIFI_AUTH_WPA2_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_WPA3_PSK  // 允许 WPA2/WPA3 混合加密
#elif CONFIG_ESP_WIFI_AUTH_WAPI_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WAPI_PSK  // 允许 WAPI-PSK 加密
#endif



/* 事件组标志位，用于指示 WiFi 连接状态 */
#define WIFI_CONNECTED_BIT BIT0  // 连接成功标志位
#define WIFI_FAIL_BIT      BIT1  // 连接失败标志位
void wifi_init_sta(void);
void nvs_init();

#endif