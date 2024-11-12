#ifndef _THREAD_AD_H
#define _THREAD_AD_H

#include <stdint.h>

void *thread_ad(void *parameter);

void mqtt_set_da_num(uint8_t da_flag,uint8_t dac_num);

#endif
