#ifndef _THREAD_UI_H
#define _THREAD_UI_H

#include <stdint.h>

typedef struct 
{
    uint8_t tmp;
    uint8_t rh;
    uint8_t light_adc;
}ui_data_t;


void *thread_ui(void *parameter);

void set_ui_data_dht11(uint8_t *RH_DATA,uint8_t *TMP_DATA);
void set_ui_data_light_adc(uint8_t *light_adc);
ui_data_t get_ui_data(void);

#endif
