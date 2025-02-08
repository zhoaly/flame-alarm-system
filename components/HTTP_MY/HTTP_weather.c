#include "HTTP_weather.h"
#include "esp_log.h"
#include "esp_http_client.h"
#include "cJSON.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "WiFi_my.h"
#include "lvgl.h"

extern EventGroupHandle_t wifi_event_group;  // WiFi 事件组

char * location_text={0};
char * Weather_text={0};
char * Temperature_text={0};
char * Update_text={0};

 
// 定义日志标签，便于调试输出
static const char *TAG = "HTTP_weather";
TaskHandle_t http_Handle; // HTTP任务句柄

// 定义最大HTTP输出缓冲区大小
#define MAX_HTTP_OUTPUT_BUFFER 2048

// 目标URL：用于获取天气信息
//static const char *URL = "https://api.seniverse.com/v3/weather/now.json?key=S5gbvt-EgnVL1tJ4B&location=xian&language=zh-Hans&unit=c";
static const char *URL = "https://api.seniverse.com/v3/weather/now.json?key=S5gbvt-EgnVL1tJ4B&location=xian&language=en&unit=c";
/**
 * @brief HTTP事件回调函数
 * @param evt 事件参数
 * @return esp_err_t ESP_OK表示成功
 */
static esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ERROR:
        ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
        break;
    case HTTP_EVENT_HEADER_SENT:
        ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
        break;
    case HTTP_EVENT_ON_HEADER:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER");
        printf("%.*s", evt->data_len, (char *)evt->data);
        break;
    case HTTP_EVENT_ON_DATA:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
        if (!esp_http_client_is_chunked_response(evt->client))
        {
            printf("%.*s", evt->data_len, (char *)evt->data);
        }
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
        break;
    case HTTP_EVENT_REDIRECT:
        ESP_LOGD(TAG, "HTTP_EVENT_REDIRECT");
        break;
    }
    return ESP_OK;
}

/**
 * @brief 解析JSON格式的天气数据
 * @param json_data JSON格式的字符串
 */
void parse_json(const char *json_data)
{
    cJSON *root = cJSON_Parse(json_data);
    if (!root)
    {
        ESP_LOGE(TAG, "Error parsing JSON");
        return;
    }
    
    cJSON *results = cJSON_GetObjectItem(root, "results");
    if (!cJSON_IsArray(results))
    {
        ESP_LOGE(TAG, "Error: results is not an array");
        cJSON_Delete(root);
        return;
    }
    
    cJSON *first_result = cJSON_GetArrayItem(results, 0);
    if (!first_result)
    {
        ESP_LOGE(TAG, "Error: No first result");
        cJSON_Delete(root);
        return;
    }
    
    cJSON *location = cJSON_GetObjectItem(first_result, "location");
    cJSON *now = cJSON_GetObjectItem(first_result, "now");
    cJSON *last_update = cJSON_GetObjectItem(first_result, "last_update");
    
    if (location && now && last_update)
    {
        cJSON *loc_name = cJSON_GetObjectItem(location, "name");
        cJSON *weather_text = cJSON_GetObjectItem(now, "text");
        cJSON *temperature = cJSON_GetObjectItem(now, "temperature");

        
        location_text=loc_name ? loc_name->valuestring: "Unknown";
        Weather_text=weather_text ? weather_text->valuestring : "Unknown";
        Temperature_text=temperature ? temperature->valuestring : "Unknown";
        Update_text=last_update->valuestring;

        ESP_LOGI(TAG, "Location: %s",location_text);
        ESP_LOGI(TAG, "Weather: %s", Weather_text);
        ESP_LOGI(TAG, "Temperature: %s°C",Temperature_text);
        ESP_LOGI(TAG, "Last Update: %s",  Update_text);

        // ESP_LOGI(TAG, "Location: %s", loc_name ? loc_name->valuestring : "Unknown");
        // ESP_LOGI(TAG, "Weather: %s", weather_text ? weather_text->valuestring : "Unknown");
        // ESP_LOGI(TAG, "Temperature: %s°C", temperature ? temperature->valuestring : "Unknown");
        // ESP_LOGI(TAG, "Last Update: %s", last_update->valuestring);
    }
    
    //cJSON_Delete(root);
}

/**
 * @brief HTTP客户端任务
 */
void http_client_task()
{
    //vTaskDelay(5000 / portTICK_PERIOD_MS); // 等待WiFi连接成功
    xEventGroupWaitBits(wifi_event_group,WIFI_CONNECTED_BIT,pdFALSE,pdFALSE,(TickType_t)portMAX_DELAY);\
    ESP_LOGI(TAG, "http_client_task start");

    char local_response_buffer[MAX_HTTP_OUTPUT_BUFFER + 1] = {0};
    while (1){
    
    esp_http_client_config_t config = {
        .url = URL,
        .event_handler = _http_event_handler,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    esp_http_client_set_method(client, HTTP_METHOD_GET);

    esp_err_t err = esp_http_client_open(client, 0);
    if (err == ESP_OK)
    {
        ESP_LOGI(TAG, "HTTP GET Status = %d, content_length = %" PRId64,
                 esp_http_client_get_status_code(client),
                 esp_http_client_get_content_length(client));
        
        int64_t content_length = esp_http_client_fetch_headers(client);
        if (content_length < 0)
        {
            ESP_LOGE(TAG, "HTTP client fetch headers failed");
        }
        else
        {
            int data_read = esp_http_client_read_response(client, local_response_buffer, MAX_HTTP_OUTPUT_BUFFER);
            if (data_read >= 0)
            {
                ESP_LOGI(TAG, "data: %s", local_response_buffer);
                parse_json(local_response_buffer);
            }
        }
    }
    else
    {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }
    
    esp_http_client_cleanup(client);

    xEventGroupSetBits(wifi_event_group, HTTP_WEATHER_SET_BIT);

    vTaskDelay(pdMS_TO_TICKS(1000*60*15));//一分钟阻塞
    //vTaskDelete(NULL);
    }
}

/**
 * @brief 初始化HTTP天气请求任务
 */
void HTTP_weather_init_my(void)
{
    xTaskCreatePinnedToCore(http_client_task, "http_client_task", 1024 * 8, NULL, 5, &http_Handle, 0);
}
