#ifndef _BSP_PWM_H
#define _BSP_PWM_H

#include <stdint.h>

#define PWM1 18 
#define PWM2 12
#define PWM3 13
#define PWM4 19

void pwm_init(void);
void soft_pwm_init(uint8_t soft_pwm_pin,uint16_t max_count);
void soft_pwm_set_count(uint8_t soft_pwm_pin,uint16_t count);

#endif
