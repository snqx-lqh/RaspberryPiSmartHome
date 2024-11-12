#ifndef _PCF8591_H
#define _PCF8591_H

#include <stdint.h>

int pcf8591Setup ();
void pcfAnalogWrite (int value);
uint8_t pcfAnalogRead (int pinReg);

#endif
