#include "ultra/ultra.h"
#include "debug/debug.h"
#include "util/util.h"

#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "pico/time.h"

#define ULTRA_MIN_DIST 2
#define ULTRA_MAX_DIST 400

// TODO: asd
#define CM_PER_US 3

static bool _ultra_trig_irq(struct repeating_timer* t) {
    uint trig = ((ultra*) t->user_data)->trig;

    // toggle
    gpio_put(trig, !gpio_get(trig));

    return true;
}

void ultra_init(ultra* u, uint trig, uint echo) {
    u->trig = trig;
    u->echo = echo;
    u->rise = 0;
    u->last_dist = 0;

    gpio_set_irq_enabled(echo, GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);

    add_repeating_timer_ms(-60, _ultra_trig_irq, u, &u->trig_up);
    sleep_us(15);
    add_repeating_timer_ms(-60, _ultra_trig_irq, u, &u->trig_down);
}

void ultra_echo_rise_irq(ultra* u, uint64_t now) {
    u->rise = now;
}

void ultra_echo_fall_irq(ultra* u, uint64_t fall) {
    uint16_t dist = (fall - u->rise) * CM_PER_US;
    dist = CLAMP(dist, ULTRA_MIN_DIST, ULTRA_MAX_DIST);

    u->last_dist = dist;
}

uint16_t ultra_distance(ultra* u) {
    return u->last_dist;
}
