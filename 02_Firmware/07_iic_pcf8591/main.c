#include <wiringPi.h>
#include <stdio.h>

#include "bsp_led.h"
#include "bsp_key.h"
#include "bsp_beep.h"
#include "pcf8591.h"

int main(void)
{
    int key_value  = 0; 
    int dac_led_value = 0;
    uint8_t night_adc     = 0;
    if(wiringPiSetupGpio() < 0) //当使用这个函数初始化树莓派引脚时，程序使用的是wiringPi 引脚编号表。
        return 1;
    
    led_init();
    key_init();
    if(pcf8591Setup()==FALSE)
        printf("pcf初始化失败\r\n");
    else
        printf("pcf初始化成功\r\n");

    while (1)
    {
        key_value = KeyScan(SINGLE_SCAN); //读取引脚电平
        if(key_value == 1)
        {
            dac_led_value +=50;
            if(dac_led_value>250) dac_led_value=250;
            pcfAnalogWrite(dac_led_value);     
        }else if(key_value == 2)
        { 
            dac_led_value -=50;
            if(dac_led_value<0) dac_led_value=0;
            pcfAnalogWrite(dac_led_value); 
        }else if(key_value == 3)  
        {
            night_adc = pcfAnalogRead(0);
            printf("%d\r\n",night_adc);
        }
        delay(5);
    }
}