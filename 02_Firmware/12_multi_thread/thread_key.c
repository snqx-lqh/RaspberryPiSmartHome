#include "thread_key.h"

#include "bsp_key.h"
#include "bsp_led.h"

void *thread_key(void *parameter)
{
    int key_value = 0;
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