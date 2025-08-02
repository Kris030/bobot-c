#ifndef ENCODER_H
#define ENCODER_H

#include "pico/types.h"

typedef struct encoder {
    uint pinA, pinB;

    volatile int32_t n;
    volatile uint64_t elapsed;

    uint64_t last_irq;
} encoder;

void encoder_init(encoder* enc, uint pinA, uint pinB);

void encoder_irq_handler(encoder* enc, uint64_t now);

int32_t encoder_get_position(encoder* enc);
int32_t encoder_get_speed(encoder* enc);

#endif
