#include "rgb_sensor/rgb_sensor.h"
#include <hardware/i2c.h>

#include "debug/debug.h"
#include "hardware/gpio.h"

#define DEFAULT_ADDRESS 0x29
#define CMD_BIT 0x80

#define REG_ENABLE 0x00
#define REG_CDATAL 0x14
#define REG_CONTROL 0x0F
#define REG_ATIME 0x01

#define PON 0x01
#define AEN 0x02

static void _rgb_sens_write8(rgb_sensor* s, uint8_t reg, uint8_t val) {
    // i2c_write_blocking(s->i2c, DEFAULT_ADDRESS, &reg, sizeof(reg), true);
    // i2c_write_blocking(s->i2c, DEFAULT_ADDRESS, &val, sizeof(val), false);

    uint8_t buf[] = { reg, val };
    int r = i2c_write_burst_blocking(s->i2c, DEFAULT_ADDRESS, buf, sizeof(buf));

    if (r != sizeof(buf))
        debug_printf("[ERROR] couldn't write rgb sensor value\n");
}

static uint8_t _rgb_sens_read8(rgb_sensor* s, uint8_t reg) {
    int r = i2c_write_blocking(s->i2c, DEFAULT_ADDRESS, &reg, sizeof(reg), true);
    if (r != sizeof(reg)) {
        debug_printf("[ERROR] couldn't write rgb sensor value\n");
        return 0;
    }

    uint8_t val;
    r = i2c_read_blocking(s->i2c, DEFAULT_ADDRESS, &reg, sizeof(val), false);
    if (r != sizeof(val)) {
        debug_printf("[ERROR] couldn't write rgb sensor value\n");
        return 0;
    }

    return val;
}

static void _rgb_sens_write_bits(rgb_sensor* s, uint8_t reg, uint8_t value, uint8_t mask) {
    uint8_t old = _rgb_sens_read8(s, reg);

    // zero out bits not to be written
    value &= mask;
    // zero out bits to be written
    old &= ~mask;

    // combine the old and the new values
    uint8_t new = old | value;
    _rgb_sens_write8(s, reg, new);
}

void rgb_sensor_init(rgb_sensor* s, uint sda, uint scl, uint i2c_index) {
    s->i2c = i2c_get_instance(i2c_index);

    i2c_init(s->i2c, 100 * 1000);
    gpio_set_function(sda, GPIO_FUNC_I2C);
    gpio_set_function(scl, GPIO_FUNC_I2C);
    gpio_pull_up(sda);
    gpio_pull_up(scl);

    _rgb_sens_write_bits(s, REG_ENABLE, PON, PON);

    // datasheet recommends at least 10ms
    sleep_ms(15);

    _rgb_sens_write_bits(s, REG_ENABLE, AEN, AEN);
}

rgb_sensor_color_raw rgb_sensor_measure(rgb_sensor* s) {
    rgb_sensor_color_raw c;

    // read c, r, g, b at once
    // !!STRUCT MEMBER ORDER MATTERS!!
    int r = i2c_read_burst_blocking(s->i2c, REG_CDATAL, (uint8_t*) &c, sizeof(c));
    if (r != sizeof(c)) {
        debug_printf("[ERROR] couldn't read rgb sensor colors\n");
        return (rgb_sensor_color_raw) { .c = 0, .r = 0, .g = 0, .b = 0 };
    }

    return c;
}

void set_integration_time(rgb_sensor* s, uint8_t int_time) {
    _rgb_sens_write8(s, REG_ATIME, 0xff - int_time);
}
void set_gain(rgb_sensor* s, uint8_t gain) {
    _rgb_sens_write_bits(s, REG_CONTROL, gain, 0b11);
}
