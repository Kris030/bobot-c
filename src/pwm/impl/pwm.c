#include "pwm/pwm.h"

#include "hardware/gpio.h"
#include "hardware/platform_defs.h"
#include "hardware/pwm.h"

// CPU clock divider, a higher value allowes much lower
// frequencies to be set, but the precision slightly decreases at
// higher frequencies
// For our use cases, this shouldn't be an issue
// note: you can get the highest possible wrap value with CLOCK_SPEED / CLKDIV
#define CLKDIV 100

static void hpwm_update(hpwm* pwm) {
    uint slice = pwm_gpio_to_slice_num(pwm->pin);
    pwm_set_clkdiv(slice, CLKDIV);

    // wrap value
    // the CPU starts counting, and this will be the last value where it is still going
    // with the frequency, we set how many times a full cycle will be executed every second
    uint wrap = SYS_CLK_HZ / (CLKDIV * pwm->freq);

    // level value
    // this can range from 0 to wrap
    // after the CPU reaches level, it switches from high to low
    uint level = (pwm->duty * wrap) / 0xffff;

    // configure the PWM slice for the desired frequency (wrap value)
    pwm_set_wrap(slice, wrap);

    // set the duty cycle (level value)
    pwm_set_chan_level(slice, pwm_gpio_to_channel(pwm->pin), level);
}

void hpwm_init(hpwm* pwm, uint pin, uint32_t freq, uint16_t duty) {
    pwm->pin = pin;
    pwm->freq = freq;
    pwm->duty = duty;

    gpio_set_function(pin, GPIO_FUNC_PWM);
    pwm_set_enabled(pwm_gpio_to_slice_num(pin), true);

    hpwm_update(pwm);
}

void hpwm_set_freq(hpwm* pwm, uint32_t freq) {
    pwm->freq = freq;
    hpwm_update(pwm);
}
void hpwm_set_duty(hpwm* pwm, uint16_t duty) {
    pwm->duty = duty;
    hpwm_update(pwm);
}
void hpwm_set_freq_duty(hpwm* pwm, uint32_t freq, uint16_t duty) {
    pwm->freq = freq;
    pwm->duty = duty;

    hpwm_update(pwm);
}

void hpwm_deinit(hpwm* pwm) {
    gpio_deinit(pwm->pin);
}
