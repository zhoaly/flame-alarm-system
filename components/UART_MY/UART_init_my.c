
#include "UART_init_my.h"
#include "esp_log.h"
#include "lvgl.h"

static const char *TAG = "uart_init_my";



uart_report_type uart_input_cmd={   .startime=0x00, 
                                    .reprot=0x00,
                                    .currenttime=0x00//初始化
                    };

QueueHandle_t uart_report_cmd_quere;//uart输入队列句柄


uint8_t data[uart_max_data_length]={0};
TaskHandle_t uart_Handle;
QueueHandle_t uart_queue;//uart 事件队列句柄
//const uart_port_t uart_num = UART_NUM_0;


int  DRAM_ATTR length=0;
uart_event_t DRAM_ATTR event;
    
void uart_task(){

    while (1)
    {
        if (xQueueReceive(uart_queue, (void *)&event, (TickType_t)portMAX_DELAY)) {
            switch (event.type){
                case UART_DATA:{
                    ESP_ERROR_CHECK(uart_get_buffered_data_len(uart_num, (size_t*)&length));//读取缓冲区内长度
                    while (length)
                    {
                        uint16_t temp_len=length;
                        if(temp_len>=uart_max_data_length) temp_len=uart_max_data_length;
                        uart_read_bytes(uart_num, data, temp_len, 100);
                        //uart_write_bytes(uart_num, (const char*)data, temp_len); 
                        ESP_LOGI(TAG,"esp_send lenth:%d",temp_len);
                        length-=temp_len;
                    }
                    uart_input_cmd.cmd=data[0];
                    uart_input_cmd.reprot=data[1];
                    uart_input_cmd.currenttime=xTaskGetTickCount();
                    xQueueSend(uart_report_cmd_quere,(void *)&uart_input_cmd,(TickType_t)0);
                    break;
                }
                case UART_BUFFER_FULL:{
                    ESP_LOGE(TAG,"errorUART_BUFFER_FULL");
                    break;
                }
                case UART_FIFO_OVF:{
                    ESP_LOGE(TAG,"errorUART_FIFO_OVF");
                    break;
                }
                case UART_BREAK:{
                    ESP_LOGE(TAG,"errorUART_BREAK");
                    break;
                }
                default:{
                    ESP_LOGI(TAG, "uart event type: %d", event.type);
                    break;
                }

            }
        }
    }
}


void uart_init_my(){

    uart_config_t uart_config = {
    .baud_rate = uart_baud_rate,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    };
    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(uart_num, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(uart_num, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE,\
                                 UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));


    // Install UART driver using an event queue here
    ESP_ERROR_CHECK(uart_driver_install(uart_num, uart_buffer_size, \
                                        uart_buffer_size, 10, &uart_queue, 0));
    //创建输入队列
    xTaskCreatePinnedToCore(uart_task,"uart_task",4096,NULL,3,&uart_Handle,0);

    uart_report_cmd_quere=xQueueCreate(10,sizeof(uart_report_type));

    //队列元素初始化
    uart_input_cmd.startime=xTaskGetTickCount();
    uart_input_cmd.currenttime=xTaskGetTickCount();
    
}