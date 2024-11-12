#ifndef _BSP_BEEP_H
#define _BSP_BEEP_H

#include <stdint.h>

#define BEEP 16

void beep_init(void);
void beep_on(void);
void beep_off(void);

#endif
