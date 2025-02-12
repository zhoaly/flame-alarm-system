#include "button_my.h"


// button_handle_t gpio_btn = NULL;
button_handle_t sw1_btn = NULL;
button_handle_t sw2_btn = NULL;
button_handle_t sw3_btn = NULL;

static const char *TAG = "button_my";


// 按键单击回调函数
static void button_single_click_cb(void *arg,void *usr_data)
{
    button_handle_t *source_btn =  usr_data;//接收指针
    if (source_btn==&sw1_btn)
    {
        ESP_LOGI(TAG, "sw1_right");
        lvgl_scroll_to_right_my();
    }
    else if (source_btn==&sw2_btn)
    {
        ESP_LOGI(TAG, "sw2_ok");
        lvgl_key_enter_my();
    }
    else if (source_btn==&sw3_btn)
    {
        ESP_LOGI(TAG, "sw3_left");
        lvgl_scroll_to_left_my();
    }
    
    
    //iot_button_get_event();
    ESP_LOGI(TAG, "BUTTON_SINGLE_CLICK");
}

// 按键初始化
void button_my_init(button_handle_t* gpio_btn,int32_t gpio_num,int8_t active_level){

    const button_config_t btn_cfg = {0};
    const button_gpio_config_t btn_gpio_cfg = {
        .gpio_num = gpio_num,//按键引脚
        .active_level = active_level,//按键电平
    };
    esp_err_t ret = iot_button_new_gpio_device(&btn_cfg, &btn_gpio_cfg, gpio_btn);
    if(NULL == *gpio_btn) {
        ESP_LOGE(TAG, "Button create failed");
    }

    // 注册按键单击事件回调函数
    iot_button_register_cb(*gpio_btn, BUTTON_SINGLE_CLICK, NULL, button_single_click_cb,gpio_btn);

}


bool button_my_is_press(uint8_t id,
                        button_handle_t* gpio_btn1,
                        button_handle_t* gpio_btn2,
                        button_handle_t* gpio_btn3)
{
    switch (id)
    {
    case 1:
        if (iot_button_get_key_level(*gpio_btn1))return true;
        else return false;
    case 2:
        if (iot_button_get_key_level(*gpio_btn2))return true;
        else return false;
    case 3:
        if (iot_button_get_key_level(*gpio_btn3))return true;
        else return false;
    default:
        return false;
    }
    
}

void button_my_init_all(){

    button_my_init(&sw1_btn,48,1);
    button_my_init(&sw2_btn,47,1);
    button_my_init(&sw3_btn,38,1);

}



