#ifndef _BSP_LED_H
#define _BSP_LED_H

#include <stdint.h>

#define LED2  17
#define LED3  27
#define LED4  22
#define LED5  23

void led_init(void);
void led_on(uint8_t led_pin);
void led_off(uint8_t led_pin);

#endif