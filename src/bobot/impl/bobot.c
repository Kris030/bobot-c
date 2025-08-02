#include "bobot/bobot.h"
#include "encoder/encoder.h"
#include "h_bridge/h_bridge.h"
#include "pid/pid.h"
#include "pwm/pwm.h"
#include "rgb_led/rgb_led.h"
#include "rgb_sensor/rgb_sensor.h"
#include "servo/servo.h"
#include "ultra/ultra.h"

#include "config/config.h"

#include "boards/pico_w.h"
#include "hardware/irq.h"
#include "hardware/regs/io_bank0.h"
#include "pico/cyw43_arch.h"

#include <pico/time.h>
#include <stdio.h>

static volatile bool _bobot_should_leave = false;

static h_bridge _h_bridge;
static encoder _encoder_left, _encoder_right;
static pid _pid_left, _pid_right;

static servo _servo;

static ultra _ultra;

static rgb_sensor _rgb_sensor;
static rgb_led _rgb_led;

static hpwm _buzzer;

static repeating_timer_t _bobot_speed_control_timer;
static bool _bobot_speed_control_irq(__unused repeating_timer_t* _timer) {
    h_bridge_set_power(
        &_h_bridge, //
        pid_update(&_pid_left, encoder_get_position(&_encoder_left)),
        pid_update(&_pid_right, encoder_get_position(&_encoder_right))
    );

    return true;
}

#define IRQ_RISE(pin) (io_bank0_hw->proc0_irq_ctrl.ints[pin / 8] & (GPIO_IRQ_EDGE_RISE << (4 * (pin % 8))))
#define IRQ_FALL(pin) (io_bank0_hw->proc0_irq_ctrl.ints[pin / 8] & (GPIO_IRQ_EDGE_FALL << (4 * (pin % 8))))

static void __not_in_flash_func(_bobot_gpio_irq)(void) {
    // static void _bobot_gpio_irq(void) {
    uint64_t now = time_us_64();

    if (IRQ_RISE(CONF_ULTRA_ECHO))
        ultra_echo_rise_irq(&_ultra, now);

    if (IRQ_FALL(CONF_ULTRA_ECHO))
        ultra_echo_fall_irq(&_ultra, now);

    if (IRQ_RISE(CONF_ENC_L_A))
        encoder_irq_handler(&_encoder_left, now);

    if (IRQ_RISE(CONF_ENC_R_A))
        encoder_irq_handler(&_encoder_right, now);

    if (IRQ_RISE(CONF_PROX_PIN)) {
        static uint64_t _bobot_last_proxy = 0;

        if (now - _bobot_last_proxy >= CONF_PROX_DEBOUNCE) {
            _bobot_last_proxy = now;
            //  toggle();
        }
    }

    if (IRQ_RISE(CONF_BUTTON_PIN) || IRQ_FALL(CONF_BUTTON_PIN)) {
        static uint64_t _bobot_last_button = 0;

        if (now - _bobot_last_button >= CONF_BUTTON_DEBOUNCE) {
            _bobot_last_button = now;
            //  toggle();
        }
    }

    for (uint i = 0; i < sizeof(io_bank0_hw->intr) / sizeof(io_bank0_hw->intr[0]); i++)
        io_bank0_hw->intr[i] = io_bank0_hw->proc0_irq_ctrl.ints[i];
}

void bobot_init(void) {
    // TODO: tight_loop_contents();

    puts("Initializing bobot...");

    // enable irqs
    irq_set_exclusive_handler(IO_IRQ_BANK0, _bobot_gpio_irq);
    irq_set_enabled(IO_IRQ_BANK0, true);

    h_bridge_init(&_h_bridge, CONF_HB_L1, CONF_HB_L2, CONF_HB_R1, CONF_HB_R2, CONF_HB_SLEEP, CONF_HB_FAULT);
    h_bridge_set_power(&_h_bridge, 0, 0);

    encoder_init(&_encoder_left, CONF_ENC_L_A, CONF_ENC_L_B);
    encoder_init(&_encoder_right, CONF_ENC_R_A, CONF_ENC_R_B);

    pid_init(&_pid_left);
    pid_set_sp(&_pid_left, 0);
    pid_set_consts(
        &_pid_left,                                              //
        CONF_MOT_PID_L_KP, CONF_MOT_PID_L_KI, CONF_MOT_PID_L_KD, //
        CONF_MOT_PID_L_INT_MIN, CONF_MOT_PID_L_INT_MAX
    );

    pid_init(&_pid_right);
    pid_set_sp(&_pid_right, 0);
    pid_set_consts(
        &_pid_left,                                              //
        CONF_MOT_PID_R_KP, CONF_MOT_PID_R_KI, CONF_MOT_PID_R_KD, //
        CONF_MOT_PID_R_INT_MIN, CONF_MOT_PID_R_INT_MAX
    );

    add_repeating_timer_ms(-5, _bobot_speed_control_irq, NULL, &_bobot_speed_control_timer);

    servo_init(&_servo, CONF_SERVO_PIN, CONF_SERVO_MIN, CONF_SERVO_MID, CONF_SERVO_MAX);
    servo_deg(&_servo, 0);

    rgb_sensor_init(&_rgb_sensor, CONF_RGB_SENS_SDA, CONF_RGB_SENS_SCL, CONF_RGB_SENS_I2C_IDX);
    rgb_led_init(&_rgb_led, CONF_RGB_LED_R, CONF_RGB_LED_G, CONF_RGB_LED_B);
    rgb_led_set_color(&_rgb_led, 0, 0, 0);

    hpwm_init(&_buzzer, CONF_BUZZ_PIN, CONF_BUZZ_FREQ, 0);

    ultra_init(&_ultra, CONF_ULTRA_TRIG, CONF_ULTRA_ECHO);

    if (!cyw43_is_initialized(&cyw43_state)) {
        if (cyw43_arch_init())
            puts("[ERROR] Failed to init onboard led");
    }
}

void bobot_motor(int8_t left, int8_t right) {
    pid_set_sp(&_pid_left, left);
    pid_set_sp(&_pid_right, right);
}

void bobot_servo_deg(int8_t deg) {
    servo_deg(&_servo, deg);
}

void bobot_rgb_led(uint8_t r, uint8_t g, uint8_t b) {
    rgb_led_set_color(&_rgb_led, r, g, b);
}

void bobot_buzzer(uint16_t freq, uint16_t volume) {
    hpwm_set_freq_duty(&_buzzer, freq, volume);
}

int32_t bobot_left_wheel_pos(void) {
    return encoder_get_position(&_encoder_right);
}

int32_t bobot_right_wheel_pos(void) {
    return encoder_get_position(&_encoder_right);
}

uint16_t bobot_ultra_distance(void) {
    return ultra_distance(&_ultra);
}

hsv_color bobot_color_hsv(void) {
    rgb_sensor_color_raw raw = rgb_sensor_measure(&_rgb_sensor);
    util_rgb_rel(&raw);

    return util_rgb_to_hsv(raw.r, raw.g, raw.b);
}

void bobot_led(bool value) {
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, value);
}
