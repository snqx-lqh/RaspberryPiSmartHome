#include "bsp_beep.h"

#include <wiringPi.h>

void beep_init()
{
    pinMode(BEEP,OUTPUT);
}

void beep_on()
{
    digitalWrite(BEEP,0);
}

void beep_off()
{
    digitalWrite(BEEP,1);
}

