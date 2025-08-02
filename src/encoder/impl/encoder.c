#include "encoder/encoder.h"
#include "debug/debug.h"

#include "hardware/gpio.h"

void encoder_init(encoder* enc, uint pinA, uint pinB) {
    enc->last_irq = 0;
    enc->pinA = pinA;
    enc->pinB = pinB;
    enc->elapsed = 0;
    enc->n = 0;

    gpio_set_irq_enabled(pinA, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
    gpio_set_irq_enabled(pinB, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
}

void encoder_irq_handler(encoder* enc, uint64_t now) {
    uint64_t elapsed = now - enc->last_irq;

    enc->last_irq = now;

    // bool a = gpio_get(enc->pinA);
    bool b = gpio_get(enc->pinB);

    int32_t c = 0;

    // if (a) {
    c = b ? -1 : 1;
    //} else {
    //  c = b ? 1 : -1;
    //}

    //   if (c == 0)
    //     return;

    enc->n += c;

    if (c == -1)
        elapsed = -elapsed;

    enc->elapsed = elapsed;
}

int32_t encoder_get_position(encoder* enc) {
    return enc->n;
}

int32_t encoder_get_speed(encoder* enc) {
    uint32_t elapsed = enc->elapsed;

    if (elapsed == 0)
        return 0;

    // TODO: scale encoder speed
    return 10e6 / elapsed;
}
