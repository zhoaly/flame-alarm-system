#include "LVGL_init_my.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_lvgl_port.h"
#include "UART_init_my.h"
#include "lvgl_demo_ui.h"

#include "lvgl.h"
#include "LCD_init.h"

#include "gb2312_16.c"

#include "time.h"
extern esp_lcd_panel_handle_t panel_handle;
extern esp_lcd_panel_io_handle_t io_handle;

extern QueueHandle_t uart_report_cmd_quere;

QueueHandle_t LVGLQueuehandle;//lvgl队列句柄


lv_obj_t *scr_lodding;      // 屏幕对象
lv_obj_t *scr;      // 显示器对象
lv_obj_t *scr_child_1;
lv_obj_t *scr_child_2;
lv_obj_t *scr_child_3;
lv_obj_t *scr_child_4;
lv_obj_t *scr_child_5;
lv_obj_t *panel;    // 主界面滚动面板
lv_obj_t *panel_child_1;    // 子界面的面板
lv_obj_t *panel_child_2;    // 子界面的面板
lv_obj_t *panel_child_3;    // 子界面的面板
lv_obj_t *panel_child_4;    // 子界面的面板
lv_obj_t *panel_child_5;    // 子界面的面板

lv_obj_t *btn[5];   // 存储 5 个按钮的数组
lv_obj_t *label[5]; // 存储 5 个标签的数组
lv_obj_t * new_btn1;//子界面的按钮
lv_obj_t * new_btn2;//子界面的按钮
lv_obj_t * new_btn3;//子界面的按钮
lv_obj_t * new_btn4;//子界面的按钮
lv_obj_t * new_btn5;//子界面的按钮



lv_obj_t * new_label1_time;   // 子界面1的标签
lv_obj_t * new_label1_mq2;   // 子界面1的标签
lv_obj_t * new_label1_time_value;   // 子界面1的标签(数值)
lv_obj_t * new_label1_mq2_value;   // 子界面1的标签(数值)

lv_obj_t * new_label2_value1;  // 子界面2的标签
lv_obj_t * new_label2_value2;  // 子界面2的标签
lv_obj_t * new_label2_head;  // 子界面2的标签

lv_obj_t * new_label3_Location;//子界面的lable
lv_obj_t * new_label3_Weather;//子界面的lable
lv_obj_t * new_label3_Temperature;//子界面的lable
lv_obj_t * new_label3_Update;//子界面的lable

lv_obj_t * new_label4;//子界面的lable
lv_obj_t * new_label5;//子界面的lable

lv_style_t style_panel;               // 面板样式
lv_style_t style_button_focu;         // 按钮处于焦点时的样式
lv_style_t style_button_default;      // 按钮的默认样式
lv_style_t style_text_default;      // 字体的默认样式
lv_style_t style_text_mini;      // 字体的mini样式
lv_style_t style_text_chinese;      // 字体的中文样式
lv_style_transition_dsc_t trans_button_focu;    // 按钮获得焦点时的动画
lv_style_transition_dsc_t trans_button_default; // 按钮恢复默认状态时的动画

lv_group_t * g; // 默认组

// 定义动画属性
lv_style_prop_t props[] = {LV_STYLE_TRANSFORM_WIDTH, LV_STYLE_TRANSFORM_HEIGHT, 0};

int  page_index=0;
int  child_pag_flag=0;

lv_disp_t *disp = NULL;//屏幕对象

TaskHandle_t lvgl_task_Handle;

lvgl_Queue lvgl_receive;//lvgl队列

lv_timer_t *timer;  // 定时器对象

static const char *TAG = "LVGL_init_my";


// 定时器回调函数 用来给page2刷新时间
void my_time_cb(lv_timer_t *parm) {
    uint32_t *user_data = timer->user_data;

    if (child_pag_flag == 1&&child_pag_flag==1)//判断是否在子页面
    {
        time_t now = 0;
        time(&now);//获取当前系统时间（UTC 时间戳）
        extern struct tm timeinfo;
        localtime_r(&now, &timeinfo);//	将 UTC 时间转换为本地时间

        int time_year,time_mon,time_day,time_hour,time_min,time_sec;
        time_year = timeinfo.tm_year + 1900;  // tm_year 是从 1900 年开始的
        time_mon = timeinfo.tm_mon + 1;     // tm_mon 从 0 开始（0 = 1 月）
        time_day = timeinfo.tm_mday;          // 直接就是日期
        time_hour =timeinfo.tm_hour;
        time_min =timeinfo.tm_min;
        time_sec =timeinfo.tm_sec;

        if (new_label2_value1!=NULL)
        lv_label_set_text_fmt(new_label2_value1, "%d-%d-%d",
        time_year,time_mon,time_day);
        if (new_label2_value2!=NULL)
        lv_label_set_text_fmt(new_label2_value2, "%d:%d:%d",
        time_hour,time_min,time_sec);

    }
    

}


            


//读取LVGLQueuehandle任务 接收硬件数据
static void lvgl_task_queue(){
    
    while (1)
    {
        xQueueReceive(LVGLQueuehandle,&lvgl_receive,(TickType_t)portMAX_DELAY);
        
        if (scr_child_1!=NULL&&child_pag_flag == 1)
        {
            //ESP_LOGI(TAG, "lvgl_receive:%ld,mq2 :%d",lvgl_receive.time,lvgl_receive.MQ2_value);

            lv_label_set_text_fmt(new_label1_time_value, ":%ld", pdTICKS_TO_MS(lvgl_receive.time)/1000);
            lv_label_set_text_fmt(new_label1_mq2_value, ":%d", lvgl_receive.MQ2_value);
        }
    }
}


//串口队列读取操作任务
static void lvgl_task_uart(){
    uart_report_type uart_output;

    while (1)
    {
        xQueueReceive(uart_report_cmd_quere,(void *)&uart_output,(TickType_t)portMAX_DELAY);
        TickType_t reort_time=uart_output.currenttime-uart_output.startime;

        // static int i= 0;
        // i = (int)uart_output.reprot-'0';//char转化为int
        static int i= 0;
        i = (int)uart_output.reprot;
        ESP_LOGI(TAG,"cmd is :%c \n,reprot is :%c\n,time is: %ld \n",uart_output.cmd,uart_output.reprot,reort_time);
        lvgl_port_unlock();
        switch (uart_output.cmd)
        {
        case INDEX:
            ESP_LOGI(TAG,"cmd is INDEX");
            if(uart_output.reprot!=0x00&&i<=4) page_index=i;
            ESP_LOGI(TAG,"page index:%d",page_index);
            lv_obj_scroll_to_view(btn[page_index],LV_ANIM_ON);
            break;
        case LEFT:
            ESP_LOGI(TAG,"cmd is LEFT");
            lvgl_scroll_to_left_my();
            // page_index+=1;
            // if (page_index>4)page_index=0;
            // ESP_LOGI(TAG,"page index:%d",page_index);
            // lv_obj_scroll_to_view(btn[page_index],LV_ANIM_ON);
            break;
        case RIGHT:
            ESP_LOGI(TAG,"cmd is RIGHT");
            lvgl_scroll_to_right_my();
            // page_index-=1;
            // if (page_index<0)page_index=4;
            // ESP_LOGI(TAG,"page index:%d",page_index);
            // lv_obj_scroll_to_view(btn[page_index],LV_ANIM_ON);
            break;
        case CONFIRM:
            ESP_LOGI(TAG,"cmd is CONFIRM");
            //lv_obj_add_state(btn[page_index], LV_STATE_CHECKED);
            lvgl_key_enter_my();
            break;
        default:
            ESP_LOGI(TAG,"cmd is unknow");
            break;
        }
        
        //lv_obj_scroll_to_view(btn[i],LV_ANIM_ON);
        // static int i=0;
        // lv_obj_scroll_to_view(btn[i],LV_ANIM_ON);
        // i++;
        // if (i>=5) i=0;

    }
}


lv_disp_t* LVGL_Init_my(){

    ESP_LOGI(TAG, "Initialize LVGL");
    const lvgl_port_cfg_t lvgl_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    lvgl_port_init(&lvgl_cfg);
    
    const lvgl_port_display_cfg_t disp_cfg = {
        .io_handle = io_handle,
        .panel_handle = panel_handle,
        .buffer_size = EXAMPLE_LCD_H_RES * EXAMPLE_LCD_V_RES,
        .double_buffer = true,
        .hres = EXAMPLE_LCD_H_RES,
        .vres = EXAMPLE_LCD_V_RES,
        .monochrome = true,
        .rotation = {
            .swap_xy = false,
            .mirror_x = false,
            .mirror_y = false,
        }
    };

    disp = lvgl_port_add_disp(&disp_cfg);

    /* Rotation of the screen */
    lv_disp_set_rotation(disp, LV_DISP_ROT_180);

    // 创建 1 秒触发的定时器
    static uint32_t user_data = 0;
    lv_timer_enable(true);
    timer = lv_timer_create(my_time_cb, 1000, &user_data); 

    lvgl_style_init();//初始化样式
    lvgl_scr_init();
    // g = lv_group_create();//创建默认组
    // lv_group_set_default(g);

    LV_FONT_DECLARE(gb2312_16); //声明中文字体

    LVGLQueuehandle = xQueueCreate(10, sizeof(lvgl_Queue));//创建队列

    xTaskCreatePinnedToCore(lvgl_task_uart,"lvgl_task_uart",4096,NULL,2,&lvgl_task_Handle,0);
    xTaskCreatePinnedToCore(lvgl_task_queue,"lvgl_task_queue",4096,NULL,2,&lvgl_task_Handle,0);
    return disp;
}

void lvgl_style_init(){
    /* 设置按钮样式的动画过渡 */
    lv_style_transition_dsc_init(&trans_button_focu, props, lv_anim_path_linear, 300, 0, NULL);
    lv_style_transition_dsc_init(&trans_button_default, props, lv_anim_path_linear, 200, 0, NULL);

    /* 初始化面板样式 */
    lv_style_init(&style_panel);
    lv_style_set_bg_color(&style_panel, lv_color_white());
    lv_style_set_text_color(&style_panel, lv_color_black());
    lv_style_set_border_width(&style_panel, 1);
    lv_style_set_border_color(&style_panel, lv_color_black());
    lv_style_set_height(&style_panel, 64);
    lv_style_set_width(&style_panel, 128);
    lv_style_set_radius(&style_panel, 3);
    lv_style_set_pad_column(&style_panel, 5);
    
    /* 初始化按钮默认样式 */
    lv_style_init(&style_button_default);
    lv_style_set_bg_color(&style_button_default, lv_color_white());
    lv_style_set_text_color(&style_button_default, lv_color_black());
    lv_style_set_border_width(&style_button_default, 1);
    lv_style_set_border_color(&style_button_default, lv_color_black());
    lv_style_set_radius(&style_button_default, 3);
    lv_style_set_pad_column(&style_button_default, 5);
    lv_style_set_height(&style_button_default, 50);
    lv_style_set_width(&style_button_default, 100);
    lv_style_set_transition(&style_button_default, &trans_button_default);

    /* 初始化按钮聚焦（选中）样式 */
    lv_style_init(&style_button_focu);
    lv_style_set_transform_width(&style_button_focu, 5);
    lv_style_set_transform_height(&style_button_focu, 5);
    lv_style_set_transition(&style_button_focu, &trans_button_focu);

    /* 初始化字体样式 */
    lv_style_init(&style_text_default);
    lv_style_set_text_font(&style_text_default,&lv_font_montserrat_16);//16号字体


    lv_style_init(&style_text_mini);
    lv_style_set_text_font(&style_text_mini,&lv_font_montserrat_12);//12号字体

    lv_style_init(&style_text_chinese);
    lv_style_set_text_font(&style_text_chinese,&gb2312_16);//16号中文字体


}

void lvgl_scr_init(){

    scr = lv_disp_get_scr_act(disp); // 获取当前屏幕
    scr_child_1 = lv_disp_get_scr_act(disp); // 获取当前屏幕
    scr_child_2 = lv_disp_get_scr_act(disp);
    scr_child_3 = lv_disp_get_scr_act(disp);
    scr_child_4 = lv_disp_get_scr_act(disp);
    scr_child_5 = lv_disp_get_scr_act(disp);
    scr_lodding = lv_disp_get_scr_act(disp);      // 屏幕对象
}


void lvgl_key_enter_my(){
    //lv_event_send(btn[page_index],LV_EVENT_PRESSED,NULL);

    if (child_pag_flag==1)//判断是否在子页面
    {
        switch (page_index)
        {
        case 0:
            if (new_btn1!=NULL)
            lv_event_send(new_btn1,LV_EVENT_PRESSED,NULL);
            ESP_LOGI(TAG,"back to main, index is %d",page_index);
            break;
        case 1:
            if (new_btn2!=NULL)
            lv_event_send(new_btn2,LV_EVENT_PRESSED,NULL);
            ESP_LOGI(TAG,"back to main, index is %d",page_index);
            break;
        case 2:
            if (new_btn3!=NULL)
            lv_event_send(new_btn3,LV_EVENT_PRESSED,NULL);
            ESP_LOGI(TAG,"back to main, index is %d",page_index);
            break;
        case 3:
            if (new_btn4!=NULL)
            lv_event_send(new_btn4,LV_EVENT_PRESSED,NULL);
            ESP_LOGI(TAG,"back to main, index is %d",page_index);
            break;
        case 4:
            if (new_btn5!=NULL)
            lv_event_send(new_btn5,LV_EVENT_PRESSED,NULL);
            ESP_LOGI(TAG,"back to main, index is %d",page_index);
            break;
        default:
            break;
        }
    }else if(child_pag_flag==0)//判断是否在主页面
    {
        lv_event_send(btn[page_index],LV_EVENT_PRESSED,NULL);
        ESP_LOGI(TAG,"go to child");
        //lv_event_send(btn[page_index],LV_EVENT_RELEASED,NULL);
    }

}


void lvgl_scroll_to_left_my(){
    page_index+=1;
    if (page_index>4)page_index=0;
    ESP_LOGI(TAG,"page index:%d",page_index);
    lv_obj_scroll_to_view(btn[page_index],LV_ANIM_ON);
}
void lvgl_scroll_to_right_my(){
    page_index-=1;
    if (page_index<0)page_index=4;
    ESP_LOGI(TAG,"page index:%d",page_index);
    lv_obj_scroll_to_view(btn[page_index],LV_ANIM_ON);
}
