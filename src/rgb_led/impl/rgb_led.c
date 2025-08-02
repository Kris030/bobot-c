#include "rgb_led/rgb_led.h"

#define RGB_LED_FREQ 2000

void rgb_led_init(rgb_led* led, uint r_pin, uint g_pin, uint b_pin) {
    hpwm_init(&led->rpwm, r_pin, RGB_LED_FREQ, 0);
    hpwm_init(&led->gpwm, g_pin, RGB_LED_FREQ, 0);
    hpwm_init(&led->bpwm, b_pin, RGB_LED_FREQ, 0);
}

void rgb_led_set_color(rgb_led* led, uint8_t r, uint8_t g, uint8_t b) {
    hpwm_set_duty(&led->rpwm, r * 0xff);
    hpwm_set_duty(&led->gpwm, g * 0xff);
    hpwm_set_duty(&led->bpwm, b * 0xff);
}
