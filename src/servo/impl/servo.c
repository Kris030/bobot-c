#include "servo/servo.h"
#include "pwm/pwm.h"
#include "util/util.h"

#include <stdint.h>

#define SERVO_FREQ (1000 / 20)

void servo_init(servo* s, uint pin, uint16_t min, uint16_t mid, uint16_t max) {
    hpwm_init(&s->pwm, pin, SERVO_FREQ, mid);
    s->min = min;
    s->mid = mid;
    s->max = max;
}

void servo_duty(servo* s, int8_t v) {
    uint16_t d;
    if (0 <= v)
        d = s->min + ((s->mid - s->min) * v) / INT8_MIN;
    else
        d = s->mid + ((s->max - s->mid) * v) / INT8_MAX;

    hpwm_set_duty(&s->pwm, d);
}

void servo_deg(servo* s, int8_t d) {
    d = CLAMP(d, -90, 90);
    servo_duty(s, ((int32_t) d * 0x7f) / 90);
}
