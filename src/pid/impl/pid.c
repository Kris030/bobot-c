#include "pid/pid.h"
#include "debug/debug.h"

#include "pico/time.h"

void pid_init(pid* pid) {
    pid->integral = 0;
    pid->prev_err = 0;
    pid->last_t = time_us_64();
}
void pid_set_consts(pid* pid, float kp, float ki, float kd, float min_integral, float max_integral) {
    // writes belong together and are more than 32 bits
    DEBUG_DISABLE_IRQ {
        pid->kp = kp;
        pid->ki = ki;
        pid->kd = kd;
        pid->min_integral = min_integral;
        pid->max_integral = max_integral;
    }
}

void pid_set_sp(pid* pid, float sp) {
    pid->setpoint = sp;
}

float pid_update(pid* pid, float pv) {
    uint64_t t = time_us_64();

    uint64_t last_t;

    // writes belong together and are more than 32 bits
    DEBUG_DISABLE_IRQ {
        last_t = pid->last_t;
        pid->last_t = t;
    }

    uint64_t dt = t - last_t;

    float err = pid->setpoint - pv;

    float p = pid->kp * err;

    pid->integral += pid->ki * err * dt;
    if (pid->integral > pid->max_integral)
        pid->integral = pid->max_integral;
    else if (pid->integral < pid->min_integral)
        pid->integral = pid->min_integral;

    float i = pid->integral;

    float d = pid->kd * (err - pid->prev_err) * dt;

    pid->prev_err = err;

    return p + i + d;
}
