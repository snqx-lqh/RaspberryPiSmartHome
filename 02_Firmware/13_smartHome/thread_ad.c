#include "thread_ad.h"

#include <wiringPi.h>
#include "pcf8591.h" 
#include <stdio.h>
#include "thread_ui.h"

static uint8_t mqtt_set_flag = 0;
static uint8_t mqtt_dac_num = 0;

void *thread_ad(void *parameter)
{
    uint8_t night_adc     = 0;
    uint16_t get_adc_count = 0;
    if(pcf8591_init()==FALSE)
        printf("pcf初始化失败\r\n");
    else
        printf("pcf初始化成功\r\n");
    while (1)
    {
        get_adc_count ++;
        if(get_adc_count%10 == 0)
        {
            night_adc = pcf8591_get_adc(0);
            set_ui_data_light_adc(&night_adc);
            printf("%16snight_adc:%u\r\n","[thread_ad]:",night_adc);
        }
        if(mqtt_set_flag == 1)
        {
            pcf8591_set_dac(mqtt_dac_num); 
            printf("%16sset_mqtt_num\r\n","[thread_ad]:");
            mqtt_set_flag = 0;
        }
        delay(1000);
    }
}

void mqtt_set_da_num(uint8_t da_flag,uint8_t dac_num)
{
    mqtt_set_flag = da_flag;
    mqtt_dac_num  = dac_num;
}