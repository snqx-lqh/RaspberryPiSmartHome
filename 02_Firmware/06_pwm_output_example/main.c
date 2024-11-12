#include <wiringPi.h>
#include <stdio.h>

#include "bsp_key.h"
#include "bsp_pwm.h"

/*  软件PWM控制PWM输出  这里想控制舵机，使用按键控制 */
int main(void)
{
    int key_value  = 0;
    uint16_t pwm_count = 15;
    uint16_t pwm_count2 = 15;
    if(wiringPiSetupGpio() < 0) //当使用这个函数初始化树莓派引脚时，程序使用的是wiringPi 引脚编号表。
        return 1;
    
    key_init();
    soft_pwm_init(PWM1,200);//PWMfreq = 1 x 10^6 / (100 x range)  这里算出来就是50HZ
    soft_pwm_set_count(PWM1,15);
    soft_pwm_init(PWM2,200);//PWMfreq = 1 x 10^6 / (100 x range)  这里算出来就是50HZ
    soft_pwm_set_count(PWM2,15);
    soft_pwm_init(PWM3,200);//PWMfreq = 1 x 10^6 / (100 x range)  这里算出来就是50HZ
    soft_pwm_set_count(PWM3,15);
    soft_pwm_init(PWM4,200);//PWMfreq = 1 x 10^6 / (100 x range)  这里算出来就是50HZ
    soft_pwm_set_count(PWM4,15);
    while (1)
    {
        key_value = KeyScan(SINGLE_SCAN); //读取引脚电平
        if(key_value == 1)
        {
            pwm_count += 2;
            pwm_count2 += 2;
            if(pwm_count > 24) pwm_count = 24;
            if(pwm_count2 > 24) pwm_count2 = 24;
            soft_pwm_set_count(PWM1,pwm_count); 
            soft_pwm_set_count(PWM3,pwm_count2);       
        }else if(key_value == 2)
        { 
            pwm_count -= 2;
            if(pwm_count < 6) pwm_count = 6;
            soft_pwm_set_count(PWM1,pwm_count);        
        }else if(key_value == 3)  
        {
            pwm_count2 -= 2;    
            if(pwm_count2 < 6) pwm_count2 = 6;
            soft_pwm_set_count(PWM3,pwm_count2); 
        }
        delay(5);
    }
}

/*  硬件PWM控制PWM输出 这种控制方式13和19引脚会同时输出  18和12引脚会同时输出 */
// int main(void)
// {
//     int key_value  = 0;
//     uint16_t pwm_count = 0;
//     uint16_t pwm_count2 = 0;
//     if(wiringPiSetupGpio() < 0) //当使用这个函数初始化树莓派引脚时，程序使用的是wiringPi 引脚编号表。
//         return 1;
    
//     key_init();
//     pwm_init();

//     while (1)
//     {
//         key_value = KeyScan(SINGLE_SCAN); //读取引脚电平
//         if(key_value == 1)
//         {
//             pwm_count += 50;
//             pwm_count2 += 50;
//             if(pwm_count > 1000) pwm_count = 1000;
//             if(pwm_count2 > 1000) pwm_count2 = 1000;
//             pwmWrite(PWM1,pwm_count); 
//             pwmWrite(PWM3,pwm_count2);       
//         }else if(key_value == 2)
//         { 
//             pwm_count -=50;
//             if(pwm_count < 100) pwm_count = 100;
//             pwmWrite(PWM1,pwm_count);        
//         }else if(key_value == 3)  
//         {
//             pwm_count2 -= 50;    
//             if(pwm_count2 < 100) pwm_count2 = 100;
//             pwmWrite(PWM3,pwm_count2); 
//         }
//         delay(5);
//     }
// }