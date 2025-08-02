#ifndef UTIL_H
#define UTIL_H

#include "rgb_sensor/rgb_sensor.h"

#include <stdint.h>

typedef struct hsv_color {
    float h, s, v;
} hsv_color;

hsv_color util_rgb_to_hsv(uint16_t r, uint16_t g, uint16_t b);

void util_rgb_rel(rgb_sensor_color_raw* raw);

#define CLAMP(v, min, max) ((v) > (max) ? (max) : ((v) < (min) ? (min) : (v)))

#define _UTIL_QUOTE(str) #str
#define _UTIL_EXPAND_AND_QUOTE(str) _UTIL_QUOTE(str)

#endif
