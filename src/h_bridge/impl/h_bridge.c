#include "h_bridge/h_bridge.h"

#include "hardware/gpio.h"
#include "pwm/pwm.h"

#define H_BRIDGE_FREQ 2000

void h_bridge_init(h_bridge* hb, uint l1, uint l2, uint r1, uint r2, uint sleep, uint fault) {
    hpwm_init(&hb->l1, l1, H_BRIDGE_FREQ, 0);
    hpwm_init(&hb->l2, l2, H_BRIDGE_FREQ, 0);
    hpwm_init(&hb->r1, r1, H_BRIDGE_FREQ, 0);
    hpwm_init(&hb->r2, r2, H_BRIDGE_FREQ, 0);

    gpio_init(sleep);
    gpio_set_dir(sleep, GPIO_OUT);
    gpio_put(sleep, 1);

    gpio_init(fault);
    gpio_set_dir(fault, GPIO_IN);

    hb->sleep = sleep;
    hb->fault = fault;
}

void h_bridge_set_power(h_bridge* hb, int8_t left, int8_t right) {
    uint16_t l1, l2, r1, r2;

    if (left >= 0) {
        l1 = ((uint16_t) left) * 2 * 0xff;
        l2 = 0;
    } else {
        l1 = 0;
        l2 = ((uint16_t) -left) * 2 * 0xff;
    }

    if (right >= 0) {
        r1 = ((uint16_t) right) * 2 * 0xff;
        r2 = 0;
    } else {
        r1 = 0;
        r2 = ((uint16_t) -right) * 2 * 0xff;
    }

    hpwm_set_duty(&hb->l1, l1);
    hpwm_set_duty(&hb->l2, l2);
    hpwm_set_duty(&hb->r1, r1);
    hpwm_set_duty(&hb->r2, r2);
}

bool h_bridge_has_fault(h_bridge* hb) {
    return gpio_get(hb->fault);
}
