#ifndef ULTRA_H
#define ULTRA_H

#include "pico/time.h"
#include "pico/types.h"

typedef struct ultra {
    uint trig, echo;

    struct repeating_timer trig_up, trig_down;

    volatile uint64_t rise;

    uint32_t last_dist;
} ultra;

void ultra_init(ultra* u, uint trig, uint echo);

void ultra_echo_rise_irq(ultra* u, uint64_t now);
void ultra_echo_fall_irq(ultra* u, uint64_t now);

uint16_t ultra_distance(ultra* u);

#endif
