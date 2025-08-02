#ifndef SERVO_H
#define SERVO_H

#include "pwm/pwm.h"

#include "pico/types.h"

typedef struct servo {
    hpwm pwm;
    uint16_t min, mid, max;
} servo;

void servo_init(servo* s, uint pin, uint16_t min, uint16_t mid, uint16_t max);

void servo_duty(servo* s, int8_t d);
void servo_deg(servo* s, int8_t d);

#endif
