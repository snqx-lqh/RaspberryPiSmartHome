#include "pcf8591.h"

#include "wiringPi.h"
#include "wiringPiI2C.h"
#include <unistd.h>

#define pcfAddress 0x48

static int pcf8591_fd = 0;

int pcf8591Setup ()
{
    if ((pcf8591_fd = wiringPiI2CSetup (pcfAddress)) < 0)
        return FALSE ;
    return TRUE ;
}

void pcfAnalogWrite (int value)
{
    if(pcf8591_fd <= 0)
        return;
    wiringPiI2CWriteReg8(pcf8591_fd,0x40,value);
}

uint8_t pcfAnalogRead (int pinReg)
{
    uint8_t adcx ;
    adcx = wiringPiI2CReadReg8(pcf8591_fd, 0x40|pinReg);
    return adcx;
}




