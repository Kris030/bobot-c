#ifndef RGB_SENSOR_H
#define RGB_SENSOR_H

#include "hardware/i2c.h"
#include "pico/types.h"

#include <stdbool.h>

typedef struct rgb_sensor {
    i2c_inst_t* i2c;
} rgb_sensor;

typedef struct rgb_sensor_color_raw {
    uint16_t c, r, g, b;
} rgb_sensor_color_raw;

void rgb_sensor_init(rgb_sensor* s, uint sda, uint scl, uint i2c_index);

rgb_sensor_color_raw rgb_sensor_measure(rgb_sensor* s);

void set_integration_time(rgb_sensor* s, uint8_t int_time);
void set_gain(rgb_sensor* s, uint8_t gain);

#endif
