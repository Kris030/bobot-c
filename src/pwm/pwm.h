#ifndef PWM_H
#define PWM_H

#include "pico/types.h"

typedef struct hpwm {
    uint pin;

    uint16_t duty;
    uint32_t freq;
} hpwm;

void hpwm_init(hpwm* pwm, uint pin, uint32_t freq, uint16_t duty);

void hpwm_set_freq(hpwm* pwm, uint32_t freq);
void hpwm_set_duty(hpwm* pwm, uint16_t duty);
void hpwm_set_freq_duty(hpwm* pwm, uint32_t freq, uint16_t duty);

void hpwm_deinit(hpwm* pwm);

#endif
