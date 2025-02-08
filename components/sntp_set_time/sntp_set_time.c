#include "sntp_set_time.h"

#include "freertos/FreeRTOS.h"
#include "esp_sntp.h"//sntp时间校准
#include "esp_netif_sntp.h"//sntp时间校准

#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "WiFi_my.h"

static const char *TAG = "sntp";  // 日志标签


struct tm timeinfo = {0};
char strftime_buf[64];
int time_year,time_mon,time_day,time_hour,time_min,time_sec;

TaskHandle_t sntp_ste_time_task;

extern EventGroupHandle_t wifi_event_group;  // WiFi 事件组
extern bool ip_ready_flag;


void sntp_set_time(){

    esp_sntp_config_t config = ESP_NETIF_SNTP_DEFAULT_CONFIG("cn.pool.ntp.org");
    esp_netif_sntp_init(&config);

    // while (ip_ready_flag==0);//等待wifi获取IP地址
    xEventGroupWaitBits(wifi_event_group,WIFI_CONNECTED_BIT,pdFALSE,pdFALSE,(TickType_t)portMAX_DELAY);
    int return_flag=0;
    while (esp_netif_sntp_sync_wait(pdMS_TO_TICKS(1000)) != ESP_OK)
    {
        ESP_LOGI(TAG,"Failed to update system time within 1s timeout,retry%d",return_flag);
        return_flag++;
        if(return_flag==10) break;
    }

    time_t now = 0;
    time(&now);//获取当前系统时间（UTC 时间戳）

    setenv("TZ", "CST-8", 1);// 将时区设置为中国标准时间
    tzset();//解析 TZ 并应用时区

    localtime_r(&now, &timeinfo);//	将 UTC 时间转换为本地时间
    //strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);


    time_year = timeinfo.tm_year + 1900;  // tm_year 是从 1900 年开始的
    time_mon = timeinfo.tm_mon + 1;     // tm_mon 从 0 开始（0 = 1 月）
    time_day = timeinfo.tm_mday;          // 直接就是日期
    time_hour =timeinfo.tm_hour;
    time_min =timeinfo.tm_min;
    time_sec =timeinfo.tm_sec;

    ESP_LOGI(TAG, "The current date is: %d-%d-%d %d:%d:%d", 
                                time_year,time_mon,time_day,time_hour,time_min,time_sec);
    //ESP_LOGI(TAG, "The current date/time in xian is: %s", strftime_buf);
    xEventGroupSetBits(wifi_event_group, SNTP_SET_BIT);

    vTaskDelete(NULL);//紫砂
}


void sntp_time_init(){

// sntp_set_time

    xTaskCreatePinnedToCore(sntp_set_time, "http_client_task", 1024 * 4, NULL, 5, &sntp_ste_time_task, 0);

}