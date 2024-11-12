#ifndef _PCF8591_H
#define _PCF8591_H

#include <stdint.h>

int pcf8591_init ();
void pcf8591_set_dac (int value);
uint8_t pcf8591_get_adc (int pinReg);

#endif
