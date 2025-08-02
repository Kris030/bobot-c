#ifndef RGB_LED_H
#define RGB_LED_H

#include "pwm/pwm.h"

#include "pico/types.h"

typedef struct rgb_led {
    hpwm rpwm, gpwm, bpwm;
} rgb_led;

void rgb_led_init(rgb_led* led, uint r_pin, uint g_pin, uint b_pin);

void rgb_led_set_color(rgb_led* led, uint8_t r, uint8_t g, uint8_t b);

#endif
