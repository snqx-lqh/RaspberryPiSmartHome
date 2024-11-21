#include "pcf8591.h"

#include "wiringPi.h"
#include "wiringPiI2C.h"
#include <unistd.h>

#include <pthread.h>

#define pcfAddress 0x48

static int pcf8591_fd = 0;

extern pthread_mutex_t pcfmutex;

int pcf8591_init ()
{
    if ((pcf8591_fd = wiringPiI2CSetup (pcfAddress)) < 0)
        return FALSE ;
    return TRUE ;
}

void pcf8591_set_dac (int value)
{

    if(pcf8591_fd <= 0)
        return;
    pthread_mutex_lock  (&pcfmutex);
    wiringPiI2CWriteReg8(pcf8591_fd,0x40,value);
    pthread_mutex_unlock(&pcfmutex);
}

uint8_t pcf8591_get_adc (int pinReg)
{
    uint8_t adcx ; 

    pthread_mutex_lock(&pcfmutex);
    adcx = wiringPiI2CReadReg8(pcf8591_fd, 0x40|pinReg);
    pthread_mutex_unlock(&pcfmutex);

    return adcx ;
}




