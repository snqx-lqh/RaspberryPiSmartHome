#include "thread_key.h"

#include <stdio.h>

#include "bsp_key.h"
#include "bsp_led.h"
#include "bsp_pwm.h"
#include "pcf8591.h"

void *thread_key(void *parameter)
{
    int key_value  = 0; 
    int pwm_state  = 0;
    led_init();
    key_init();
    soft_pwm_init(PWM1,200);//PWMfreq = 1 x 10^6 / (100 x range)  这里算出来就是50HZ
    soft_pwm_set_count(PWM1,15);
    while (1)
    {
        key_value = KeyScan(SINGLE_SCAN); //读取引脚电平
        if(key_value == 1)
        {
            led_all_on();
        }else if(key_value == 2)
        { 
            led_all_off();
        }else if(key_value == 3)  
        {
            if(pwm_state == 0) {
                soft_pwm_set_count(PWM1,5);
                pwm_state = 1;
            }
		    else if(pwm_state == 1) {
                soft_pwm_set_count(PWM1,25);
                pwm_state = 0;
            } 
        }
        delay(5);
    }
}