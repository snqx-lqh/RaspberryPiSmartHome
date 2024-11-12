#include "bsp_led.h"

#include <wiringPi.h>

void led_init()
{
    pinMode(LED2,OUTPUT);
    pinMode(LED3,OUTPUT);
    pinMode(LED4,OUTPUT);
    pinMode(LED5,OUTPUT);
}

void led_on(uint8_t led_pin)
{
    if(led_pin == LED2 || led_pin == LED3 || led_pin == LED4 || led_pin == LED5)
    {
        digitalWrite(led_pin,0);
    }else
    {
        return;
    }
}

void led_off(uint8_t led_pin)
{
    if(led_pin == LED2 || led_pin == LED3 || led_pin == LED4 || led_pin == LED5)
    {
        digitalWrite(led_pin,1);
    }else
    {
        return;
    }
}
