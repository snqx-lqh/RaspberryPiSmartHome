#include <wiringPi.h>
#include <stdio.h>

#include "bsp_led.h"
#include "bsp_key.h"

int main(void)
{
    int key_value = 0;

    if(wiringPiSetupGpio() < 0) //当使用这个函数初始化树莓派引脚时，程序使用的是BCM引脚编号表。
        return 1;
    
    led_init();
    key_init();
    
    while (1)
    {
        key_value = KeyScan(SINGLE_SCAN); //读取引脚电平
        if(key_value == 1)
        {
            led_all_on(); 
                  
        }else if(key_value == 2 || key_value == 3)
        { 
            led_all_off();;       
        }  
        delay(5);
    }
}