#ifndef _BSP_KEY_H
#define _BSP_KEY_H

#include <stdint.h>

#define  KEY1_Pin  21
#define  KEY2_Pin  26
#define  KEY3_Pin  20

typedef enum{
	SINGLE_SCAN = 0,
	CONTINUOUS_SCAN,
}key_scan_mode_t;

#define KEY1  digitalRead(KEY1_Pin)
#define KEY2  digitalRead(KEY2_Pin)
#define KEY3  digitalRead(KEY3_Pin)

#define KEY1_VALUE 1
#define KEY2_VALUE 2
#define KEY3_VALUE 3

void key_init(void);
uint8_t KeyScan(const key_scan_mode_t key_scan_mode);


#endif
