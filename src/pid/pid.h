#ifndef PID_H
#define PID_H

#include "pico/types.h"

typedef struct pid {
    // volatile because we set it from the main loop and read in in irq
    volatile float kp, ki, kd, min_integral, max_integral;
    volatile float setpoint;

    // non-volatile because we only access it from only irq / the main loop
    uint64_t last_t;
    float prev_err, integral;
} pid;

void pid_init(pid* pid);
void pid_set_consts(pid* pid, float kp, float ki, float kd, float min_integral, float max_integral);

void pid_set_sp(pid* pid, float sp);

float pid_update(pid* pid, float pv);

#endif
