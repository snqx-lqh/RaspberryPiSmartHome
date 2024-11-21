#include <wiringPi.h>

#include "bsp_led.h"

int main(void)
{
    if(wiringPiSetupGpio() < 0) 
        return 1;

    led_init();

    while (1)
    {
        led_on(LED2);
        led_on(LED3);
        led_off(LED4);
        led_off(LED5);
        delay(1000);//延时1000ms
        led_off(LED2);
        led_off(LED3);
        led_on(LED4);
        led_on(LED5);
        delay(1000);
    }
}