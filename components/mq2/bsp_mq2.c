#include "bsp_mq2.h"
#include "esp_log.h"


static const char *TAG = "adc";
esp_adc_cal_characteristics_t *adc_chars;

void delay_ms(unsigned int ms)
{
    vTaskDelay(ms / portTICK_PERIOD_MS);
}
void delay_us(unsigned int us)
{
    ets_delay_us(us);
}


/******************************************************************
 * 函 数 名 称：ADC_DMA_Init
 * 函 数 说 明：初始化ADC+DMA功能
 * 函 数 形 参：无
 * 函 数 返 回：无
 * 作       者：LC
 * 备       注：无
******************************************************************/
void ADC_Init(void)
{
    adc1_config_width(width);// 12位分辨率
    
    //ADC_ATTEN_DB_0:表示参考电压为1.1V
    //ADC_ATTEN_DB_2_5:表示参考电压为1.5V
    //ADC_ATTEN_DB_6:表示参考电压为2.2V
    //ADC_ATTEN_DB_11:表示参考电压为3.3V
    //adc1_config_channel_atten( channel,atten);// 设置通道0和3.3V参考电压

    // 分配内存
    adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
    // 对 ADC 特性进行初始化，使其能够正确地计算转换结果和补偿因素
    esp_adc_cal_characterize(unit, atten, width, DEFAULT_VREF, adc_chars);
}
 

/******************************************************************
 * 函 数 名 称：Get_Adc_Dma_Value
 * 函 数 说 明：对DMA保存的数据进行平均值计算后输出
 * 函 数 形 参：CHx 第几个扫描的数据
 * 函 数 返 回：对应扫描的ADC值
 * 作       者：LC
 * 备       注：无
******************************************************************/
unsigned int Get_Adc_Value(adc1_channel_t channel)
{
    unsigned char i = 0;
    unsigned int AdcValue = 0;

    /* 因为采集 SAMPLES 次，故循环 SAMPLES 次 */
    for(i=0; i < SAMPLES; i++)
    {
    /*    累加    */
            AdcValue += adc1_get_raw(channel);
            
    }
    /* 求平均值 */
    AdcValue = AdcValue / SAMPLES;
    ESP_LOGI(TAG,"adc avenage value is %d",AdcValue);
    return AdcValue;
}




/******************************************************************
 * 函 数 名 称：Get_MQ2_Percentage_value
 * 函 数 说 明：读取MQ2值，并且返回百分比
 * 函 数 形 参：无
 * 函 数 返 回：返回百分比
 * 作       者：LC
 * 备       注：无
******************************************************************/
unsigned int Get_MQ2_Percentage_value(void)
{
    int adc_max = 4095;
    int adc_new = 0;
    int Percentage_value = 0;
    
    adc_new = Get_Adc_Value(ADC1_CHANNEL_1);
    
    Percentage_value = ((float)adc_new/adc_max) * 100;
    return Percentage_value;
}



unsigned int Get_Flame_Percentage_value(void)
{
    int adc_max = 4095;
    int adc_new = 0;
    int Percentage_value = 0;
    
    adc_new = Get_Adc_Value(ADC1_CHANNEL_3);
    
    Percentage_value = ((float)adc_new/adc_max) * 100;
    
    return Percentage_value;
}