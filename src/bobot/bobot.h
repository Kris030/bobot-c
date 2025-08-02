#ifndef BOBOT_H
#define BOBOT_H

#include "pico/types.h"
#include "util/util.h"

void bobot_init(void);

void bobot_motor(int8_t left, int8_t right);
void bobot_servo_deg(int8_t deg);
void bobot_rgb_led(uint8_t r, uint8_t g, uint8_t b);
void bobot_buzzer(uint16_t freq, uint16_t volume);
void bobot_led(bool value);

int32_t bobot_left_wheel_pos(void);
int32_t bobot_right_wheel_pos(void);
uint16_t bobot_ultra_distance(void);
hsv_color bobot_color_hsv(void);

#endif
