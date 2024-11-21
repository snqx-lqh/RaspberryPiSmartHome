#ifndef _DHT11_H
#define _DHT11_H

#include <stdint.h>

void dht11_init(void);

int get_dht11_data(uint8_t *RH_DATA,uint8_t *TMP_DATA);

#endif
