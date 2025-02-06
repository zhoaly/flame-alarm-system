#include "WiFi_my.h"
#include "nvs_flash.h"

/* FreeRTOS 事件组，用于管理 WiFi 连接状态 */
static EventGroupHandle_t s_wifi_event_group;  // WiFi 事件组

/* 事件组标志位，用于指示 WiFi 连接状态 */
#define WIFI_CONNECTED_BIT BIT0  // 连接成功标志位
#define WIFI_FAIL_BIT      BIT1  // 连接失败标志位

static const char *TAG = "wifi_init";  // 日志标签


static int s_retry_num = 0;  // 连接重试次数

/* WiFi 事件处理函数 */
static void event_handler(void* arg, esp_event_base_t event_base,int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();  // WiFi 启动后尝试连接
    } 
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) 
    {
        if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();  // 断开连接后重试
            s_retry_num++;
            ESP_LOGI(TAG, "重试连接到 AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"连接 AP 失败");
    } 
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) 
    {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "获取到 IP: " IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

/* 初始化 WiFi 站点模式 调用此函数前先初始化NVS 此函数内部已经初始化nvs*/
void wifi_init_sta(void)
{   
    nvs_init();
    s_wifi_event_group = xEventGroupCreate();  // 创建事件组
    ESP_ERROR_CHECK(esp_netif_init());  // 初始化网络接口
    ESP_ERROR_CHECK(esp_event_loop_create_default());  // 创建默认事件循环（以便后面使用事件处理器）
    esp_netif_create_default_wifi_sta();  // 创建默认 WiFi 站点模式

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();  // WiFi 初始化配置
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));  // 初始化 WiFi

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));
    
    wifi_config_t wifi_config = {
        .sta = {
            .ssid = EXAMPLE_ESP_WIFI_SSID,
            .password = EXAMPLE_ESP_WIFI_PASS,
            .threshold.authmode = ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD,
            .sae_pwe_h2e = ESP_WIFI_SAE_MODE,
            .sae_h2e_identifier = EXAMPLE_H2E_IDENTIFIER,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));  // 设置 WiFi 为站点模式
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));  // 配置 WiFi
    ESP_ERROR_CHECK(esp_wifi_start());  // 启动 WiFi

    ESP_LOGI(TAG, "WiFi STA模式初始化完成。");
}


void nvs_init(){

    esp_err_t ret = nvs_flash_init();  // 初始化 NVS（非易失存储器）
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
}