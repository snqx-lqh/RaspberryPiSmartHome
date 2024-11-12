#include "bsp_pwm.h"

#include <wiringPi.h>
#include <softPwm.h>

void pwm_init()
{
    pinMode(PWM1,PWM_OUTPUT);
    pwmSetMode (PWM_MODE_MS);
    pwmSetRange(1024);              // pwm脉宽范围 0~1024
    pwmSetClock(75);                // 250Hz，19.2MHz / 75 / 1024 = 250Hz
    pwmWrite(PWM1,512); 
    delay(300);
    // pinMode(PWM2,PWM_OUTPUT);
    // pwmSetMode (PWM_MODE_MS);
    // pwmSetRange(1024);              // pwm脉宽范围 0~1024
    // pwmSetClock(75);                // 250Hz，19.2MHz / 75 / 1024 = 250Hz
    // pwmWrite(PWM2,256); 
    delay(300);
    pinMode(PWM3,PWM_OUTPUT);
    pwmSetMode (PWM_MODE_MS);
    pwmSetRange(1024);              // pwm脉宽范围 0~1024
    pwmSetClock(75);                // 250Hz，19.2MHz / 75 / 1024 = 250Hz
    pwmWrite(PWM3,512); 
    delay(300);
    // pinMode(PWM4,PWM_OUTPUT);
    // pwmSetMode (PWM_MODE_MS);
    // pwmSetRange(1024);              // pwm脉宽范围 0~1024
    // pwmSetClock(75);                // 250Hz，19.2MHz / 75 / 1024 = 250Hz
    // pwmWrite(PWM4,256); 
    // delay(300);
}

void soft_pwm_init(uint8_t soft_pwm_pin,uint16_t max_count)
{
    pinMode(soft_pwm_pin,OUTPUT); //设置引脚为 输出模式
    //PWMfreq = 1 x 10^6 / (100 x range)  需要50hz  这里的200就是range
    softPwmCreate(soft_pwm_pin , 0, max_count);// 设置周期分为200份
}

void soft_pwm_set_count(uint8_t soft_pwm_pin,uint16_t count)
{
    softPwmWrite(soft_pwm_pin , count);
}