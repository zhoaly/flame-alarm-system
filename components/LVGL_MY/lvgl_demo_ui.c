#include "lvgl.h"
#include "lvgl_demo_ui.h"
#include "LVGL_init_my.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "time.h"

extern lv_disp_t *disp; // 显示器对象

// 全局变量声明
extern lv_obj_t *scr_lodding;      // 屏幕对象
extern lv_obj_t *scr;      // 屏幕对象
extern lv_obj_t *scr_child_1; // 子界面屏幕1
extern lv_obj_t *scr_child_2; // 子界面屏幕2
extern lv_obj_t *scr_child_3; // 子界面屏幕3
extern lv_obj_t *scr_child_4; // 子界面屏幕4
extern lv_obj_t *scr_child_5; // 子界面屏幕5

extern lv_obj_t *panel;    // 主界面的滚动面板
extern lv_obj_t *panel_child_1;    // 子界面1的面板
extern lv_obj_t *panel_child_2;    // 子界面2的面板
extern lv_obj_t *panel_child_3;    // 子界面3的面板
extern lv_obj_t *panel_child_4;    // 子界面4的面板
extern lv_obj_t *panel_child_5;    // 子界面5的面板

extern lv_obj_t *btn[5];   // 存储 5 个按钮的数组
extern lv_obj_t *label[5]; // 存储 5 个标签的数组

// 子界面中的按钮与标签对象
extern lv_obj_t * new_btn1;   // 子界面1的按钮
extern lv_obj_t * new_btn2;  // 子界面2的按钮
extern lv_obj_t * new_btn3;  // 子界面3的按钮
extern lv_obj_t * new_btn4;  // 子界面4的按钮
extern lv_obj_t * new_btn5;  // 子界面5的按钮

extern lv_obj_t * new_label1_time;   // 子界面1的标签(标题)
extern lv_obj_t * new_label1_mq2;   // 子界面1的标签(标题)

extern lv_obj_t * new_label1_time_value;   // 子界面1的标签(数值)
extern lv_obj_t * new_label1_mq2_value;   // 子界面1的标签(数值)

extern lv_obj_t * new_label2_value;  // 子界面2的标签
extern lv_obj_t * new_label2_head;  // 子界面2的标签

extern lv_obj_t * new_label3;  // 子界面3的标签
extern lv_obj_t * new_label4;  // 子界面4的标签
extern lv_obj_t * new_label5;  // 子界面5的标签

extern int page_index;
extern int child_pag_flag;

// 样式和动画
extern lv_style_t style_panel;               // 面板样式
extern lv_style_t style_button_focu;         // 按钮处于焦点时的样式
extern lv_style_t style_button_default;      // 按钮的默认样式
extern lv_style_t style_text_default;      // 按钮的默认样式
extern lv_style_transition_dsc_t trans_button_focu;    // 按钮获得焦点时的动画
extern lv_style_transition_dsc_t trans_button_default; // 按钮恢复默认状态时的动画


extern QueueHandle_t LVGLQueuehandle;//lvgl队列句柄
extern lvgl_Queue lvgl_receive;

static const char *TAG = "LVGL_UI";

/* 按钮点击事件回调 */
static void btn_event_handler(lv_event_t *e) {
    lv_obj_t *btn_pressed = lv_event_get_target(e); // 获取触发事件的按钮对象
    ESP_LOGI(TAG, "btn pressed");

    // 如果点击子界面中的返回按钮则返回主界面
    if (btn_pressed == new_btn1 || btn_pressed == new_btn2 ||
        btn_pressed == new_btn3 || btn_pressed == new_btn4 ||
        btn_pressed == new_btn5)
    {
        child_pag_flag = 0; // 切换至主界面
        ESP_LOGI(TAG, "返回主界面");
        lvgl_demo_ui();
        return;
    }

    // 根据点击主界面上的按钮进入相应的子界面
    if (btn_pressed == btn[0]) {
        child_pag_flag = 1;
        ESP_LOGI(TAG, "btn0");
        lvgl_demo_ui_child_1();
        return;
    }
    else if (btn_pressed == btn[1]) {
        child_pag_flag = 1;
        ESP_LOGI(TAG, "btn1");
        lvgl_demo_ui_child_2();
        return;
    }
    else if (btn_pressed == btn[2]) {
        child_pag_flag = 1;
        ESP_LOGI(TAG, "btn2");
        lvgl_demo_ui_child_3();
        return;
    }
    else if (btn_pressed == btn[3]) {
        child_pag_flag = 1;
        ESP_LOGI(TAG, "btn3");
        lvgl_demo_ui_child_4();
        return;
    }
    else if (btn_pressed == btn[4]) {
        child_pag_flag = 1;
        ESP_LOGI(TAG, "btn4");
        lvgl_demo_ui_child_5();
        return;
    }
}

/* 滚动事件回调（用于判断按钮是否处于屏幕中心） */
static void scroll_event_handler(lv_event_t *e) {
    lv_obj_t *panel = lv_event_get_target(e);  // 获取滚动面板对象
    lv_coord_t panel_x = lv_obj_get_scroll_x(panel); // 获取当前滚动的 X 位置
    lv_coord_t panel_w = lv_obj_get_width(panel);    // 获取面板宽度

    if (child_pag_flag == 0) { // 仅在主界面中处理
        for (uint32_t i = 0; i < 5; i++) {
            lv_coord_t btn_x = lv_obj_get_x(btn[i]);   // 获取按钮 X 位置
            lv_coord_t btn_w = lv_obj_get_width(btn[i]); // 获取按钮宽度

            // 判断按钮是否处于屏幕中心 (误差范围 ±30)
            if (btn_x + btn_w / 2 >= panel_x + panel_w / 2 - 30 &&
                btn_x + btn_w / 2 <= panel_x + panel_w / 2 + 30) {
                lv_obj_add_state(btn[i], LV_STATE_USER_4); // 设为选中状态
            } else {
                lv_obj_clear_state(btn[i], LV_STATE_USER_4);
                lv_obj_add_state(btn[i], LV_STATE_DEFAULT); // 恢复默认状态
            }
        }
    }
}



void lvgl_lodding(){
    ESP_LOGI(TAG, "lodding");
    lv_obj_t * panel_lodding = lv_obj_create(scr_lodding);
    lv_obj_align(panel_lodding, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_style(panel_lodding, &style_panel, 0); // 设置面板样式

    lv_obj_t * label_lodding = lv_label_create(panel_lodding);
    lv_label_set_text_fmt(label_lodding, "lodding...");//显示项目标题 宽度35
    lv_obj_align(label_lodding, LV_ALIGN_CENTER, 0, 0);

}
/* UI函数（主界面） */
void lvgl_demo_ui() {
    ESP_LOGI(TAG, "lvgl_demo_ui");

    // 清除上个界面，并加载主界面
    lv_obj_clean(scr_child_1);
    lv_obj_clean(scr_child_2);
    lv_obj_clean(scr_child_3);
    lv_obj_clean(scr_child_4);
    lv_obj_clean(scr_child_5);//清除所有子界面
    lv_obj_clean(scr_lodding);//清除所有子界面

    lv_scr_load_anim(scr, LV_SCR_LOAD_ANIM_OVER_RIGHT, 100, 0, true);

    /* 创建容器面板（用于放置按钮） */
    panel = lv_obj_create(scr);
    lv_obj_align(panel, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_event_cb(panel, scroll_event_handler, LV_EVENT_SCROLL, NULL); // 注册滚动事件
    lv_obj_add_style(panel, &style_panel, 0); // 设置面板样式
    lv_obj_clear_flag(panel, LV_OBJ_FLAG_HIDDEN);

    /* 设置面板的滚动属性与布局 */
    lv_obj_set_scroll_snap_x(panel, LV_SCROLL_SNAP_CENTER);
    lv_obj_set_flex_flow(panel, LV_FLEX_FLOW_ROW);
    lv_obj_set_scrollbar_mode(panel, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_flex_align(panel, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    /* 在面板中添加 5 个按钮 */
    for (uint32_t i = 0; i < 5; i++) {
        btn[i] = lv_btn_create(panel);
        lv_obj_add_style(btn[i], &style_button_default, LV_STATE_DEFAULT);
        lv_obj_add_style(btn[i], &style_button_focu, LV_STATE_USER_4);
        lv_obj_add_event_cb(btn[i], btn_event_handler, LV_EVENT_PRESSED, (void *)i); // 绑定点击事件

        label[i] = lv_label_create(btn[i]);
        lv_label_set_text_fmt(label[i], "Page %ld", i + 1);
        lv_obj_center(label[i]);
    }
    
    lv_obj_scroll_to_view(btn[page_index], LV_ANIM_ON);


}

/* 子界面1的 UI函数 */
void lvgl_demo_ui_child_1(){
    ESP_LOGI(TAG, "lvgl_demo_ui_child_1");

    lv_obj_clean(scr); // 清除上个界面
    lv_scr_load_anim(scr_child_1, LV_SCR_LOAD_ANIM_OVER_TOP, 100, 0, true);


    panel_child_1 = lv_obj_create(scr_child_1);
    lv_obj_align(panel_child_1, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_style(panel_child_1, &style_panel, 0); // 设置面板样式

    new_btn1 = lv_btn_create(panel_child_1);
    lv_obj_add_style(new_btn1, &style_button_default, 0);
    lv_obj_center(new_btn1);
    lv_obj_add_event_cb(new_btn1, btn_event_handler, LV_EVENT_PRESSED, NULL); // 注册回调（返回主界面）

    new_label1_time = lv_label_create(new_btn1);
    lv_obj_add_style(new_label1_time,&style_text_default,0);
    lv_label_set_long_mode(new_label1_time, LV_LABEL_LONG_SCROLL_CIRCULAR);//长文本滚动模式
    lv_obj_set_width(new_label1_time, 35);//显示项目标题 宽度35
    lv_obj_set_pos(new_label1_time,10,0);
    lv_label_set_text_fmt(new_label1_time, "time");//显示项目标题 宽度35
    // lv_label_set_text_fmt(new_label1_time, "time:%ld", lvgl_receive.time);

    new_label1_mq2 = lv_label_create(new_btn1);
    lv_obj_add_style(new_label1_mq2,&style_text_default,0);
    lv_label_set_long_mode(new_label1_mq2, LV_LABEL_LONG_SCROLL_CIRCULAR);//长文本滚动模式
    lv_obj_set_width(new_label1_mq2, 35);//显示项目标题 宽度35
    lv_obj_set_pos(new_label1_mq2,10,16);
    lv_label_set_text_fmt(new_label1_mq2, "mq2_vlaue");//显示项目标题 宽度35
    //lv_label_set_text_fmt(new_label1_mq2, "mq2_vlaue:%d", lvgl_receive.MQ2_value);

    new_label1_time_value = lv_label_create(new_btn1);
    lv_obj_add_style(new_label1_time_value,&style_text_default,0);
    //lv_label_set_long_mode(new_label1_time_value, LV_LABEL_LONG_SCROLL_CIRCULAR);//长文本滚动模式
    lv_obj_set_width(new_label1_time_value, 45);//显示项目数值 宽度45
    lv_obj_set_pos(new_label1_time_value,45,0);
    lv_label_set_text_fmt(new_label1_time_value, ":%ld", pdTICKS_TO_MS(lvgl_receive.time)/1000);//显示项目数值

    new_label1_mq2_value = lv_label_create(new_btn1);
    lv_obj_add_style(new_label1_mq2_value,&style_text_default,0);
    //lv_label_set_long_mode(new_label1_mq2_value, LV_LABEL_LONG_SCROLL_CIRCULAR);//长文本滚动模式
    lv_obj_set_width(new_label1_mq2_value, 45);//显示项目数值 宽度45
    lv_obj_set_pos(new_label1_mq2_value,45,16);
    lv_label_set_text_fmt(new_label1_mq2_value, ":%d", lvgl_receive.MQ2_value);//显示项目数值
}

/* 子界面2的 UI函数 */
void lvgl_demo_ui_child_2(){
    ESP_LOGI(TAG, "lvgl_demo_ui_child_2");

    lv_obj_clean(scr); // 清除上个界面
    lv_scr_load_anim(scr_child_2, LV_SCR_LOAD_ANIM_OVER_TOP, 100, 0, true);

    panel_child_2 = lv_obj_create(scr_child_2);
    lv_obj_align(panel_child_2, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_style(panel_child_2, &style_panel, 0); // 设置面板样式

    new_btn2 = lv_btn_create(panel_child_2);
    lv_obj_add_style(new_btn2, &style_button_default, 0);
    lv_obj_center(new_btn2);
    lv_obj_add_event_cb(new_btn2, btn_event_handler, LV_EVENT_PRESSED, NULL); // 注册回调（返回主界面）


    new_label2_head =lv_label_create(new_btn2);
    lv_obj_add_style(new_label2_value,&style_text_default,0);
    lv_obj_align(new_label2_value, LV_ALIGN_TOP_MID, 0, 0);

    new_label2_value = lv_label_create(new_btn2);
    lv_obj_add_style(new_label2_value,&style_text_default,0);
    lv_label_set_long_mode(new_label2_value, LV_LABEL_LONG_SCROLL_CIRCULAR);//长文本滚动模式
    lv_obj_set_width(new_label2_value, 100);//显示项目数值 宽度100
    lv_label_set_text_fmt(new_label2_value, "%d-%d-%d %d:%d:%d ",0,0,0,0,0,0);

}

/* 子界面3的 UI函数 */
void lvgl_demo_ui_child_3(){
    ESP_LOGI(TAG, "lvgl_demo_ui_child_3");

    lv_obj_clean(scr); // 清除上个界面
    lv_scr_load_anim(scr_child_3, LV_SCR_LOAD_ANIM_OVER_TOP, 500, 0, true);

    panel_child_3 = lv_obj_create(scr_child_3);
    lv_obj_align(panel_child_3, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_style(panel_child_3, &style_panel, 0); // 设置面板样式

    new_btn3 = lv_btn_create(panel_child_3);
    lv_obj_add_style(new_btn3, &style_button_default, 0);
    lv_obj_center(new_btn3);
    lv_obj_add_event_cb(new_btn3, btn_event_handler, LV_EVENT_PRESSED, NULL); // 注册回调（返回主界面）

    new_label3 = lv_label_create(new_btn3);
    lv_label_set_text_fmt(new_label3, "btn %d", 3);
}

/* 子界面4的 UI函数 */
void lvgl_demo_ui_child_4(){
    ESP_LOGI(TAG, "lvgl_demo_ui_child_4");

    lv_obj_clean(scr); // 清除上个界面
    lv_scr_load_anim(scr_child_4, LV_SCR_LOAD_ANIM_OVER_TOP, 500, 0, true);

    panel_child_4 = lv_obj_create(scr_child_4);
    lv_obj_align(panel_child_4, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_style(panel_child_4, &style_panel, 0); // 设置面板样式

    new_btn4 = lv_btn_create(panel_child_4);
    lv_obj_add_style(new_btn4, &style_button_default, 0);
    lv_obj_center(new_btn4);
    lv_obj_add_event_cb(new_btn4, btn_event_handler, LV_EVENT_PRESSED, NULL); // 注册回调（返回主界面）

    new_label4 = lv_label_create(new_btn4);
    lv_label_set_text_fmt(new_label4, "btn %d", 4);
}

/* 子界面5的 UI函数 */
void lvgl_demo_ui_child_5(){
    ESP_LOGI(TAG, "lvgl_demo_ui_child_5");

    lv_obj_clean(scr); // 清除上个界面
    lv_scr_load_anim(scr_child_5, LV_SCR_LOAD_ANIM_OVER_TOP, 500, 0, true);

    panel_child_5 = lv_obj_create(scr_child_5);
    lv_obj_align(panel_child_5, LV_ALIGN_CENTER, 0, 0);
    lv_obj_add_style(panel_child_5, &style_panel, 0); // 设置面板样式

    new_btn5 = lv_btn_create(panel_child_5);
    lv_obj_add_style(new_btn5, &style_button_default, 0);
    lv_obj_center(new_btn5);
    lv_obj_add_event_cb(new_btn5, btn_event_handler, LV_EVENT_PRESSED, NULL); // 注册回调（返回主界面）

    new_label5 = lv_label_create(new_btn5);
    lv_label_set_text_fmt(new_label5, "btn %d", 5);
}
