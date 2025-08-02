#ifndef H_BRIDGE_H
#define H_BRIDGE_H

#include "pwm/pwm.h"

#include "pico/types.h"

typedef struct h_bridge {
    hpwm l1, l2, r1, r2;
    uint sleep, fault;
} h_bridge;

void h_bridge_init(h_bridge* hb, uint l1, uint l2, uint r1, uint r2, uint sleep, uint fault);

void h_bridge_set_power(h_bridge* hb, int8_t left, int8_t right);

bool h_bridge_has_fault(h_bridge* hb);

#endif
