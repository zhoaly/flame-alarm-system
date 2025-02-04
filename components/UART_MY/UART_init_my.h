#ifndef UART_INIT_MY
#define UART_INIT_MY

#include "driver/uart.h"

#define uart_num UART_NUM_0
#define uart_buffer_size (1024 * 2)
#define uart_baud_rate 115200
#define uart_max_data_length 2

typedef struct 
{
    TickType_t startime;
    TickType_t currenttime;
    uint8_t reprot;
    uint8_t cmd;
}uart_report_type;



void uart_init_my();
#endif