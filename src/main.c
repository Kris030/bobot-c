#include "bobot/bobot.h"
#include "debug/debug.h"
#include "mapman/mapman.h"
#include "net/net.h"
#include "pid/pid.h"
#include "util/util.h"

#include "pico/stdio.h"
#include "pico/time.h"

#include <stdbool.h>
#include <stdio.h>

typedef enum map_color { Red, Green, Blue, Other } map_color;
typedef enum bobot_mode { HugWall, FollowLine, Stop } bobot_mode;

volatile float line_kp = 0, line_ki = 0, line_kd = 0, line_min_int = -0.7 * 0xff, line_max_int = 0.7 * 0xff;
volatile int8_t line_fw_speed = 0x7f;

volatile float wall_kp = 0, wall_ki = 0, wall_kd = 0, wall_min_int = -0.7 * 0xff, wall_max_int = 0.7 * 0xff;
volatile int8_t wall_fw_speed = 0x7f;

int parking_markers;

map_color classify_color(hsv_color color) {
    // black / white
    if (color.s < 0.3)
        return Other;

    if (300 < color.h || color.h < 60)
        return Red;

    if (60 < color.h && color.h < 180)
        return Green;

    if (180 < color.h && color.h < 300)
        return Blue;

    return Other;
}

void go_through_marker(uint8_t speed, map_color marker_col) {
    bobot_motor(speed, speed);

    while (true) {
        hsv_color hsv = bobot_color_hsv();
        map_color col = classify_color(hsv);

        if (col != marker_col) {
            marker_col = col;
            return;
        }
    }
}

bobot_mode follow_line(void) {
    pid pid;
    pid_init(&pid);

    while (true) {
        pid_set_consts(&pid, line_kp, line_ki, line_kd, line_min_int, line_max_int);

        hsv_color hsv = bobot_color_hsv();
        map_color col = classify_color(hsv);

        switch (col) {
            case Green:
                parking_markers++;
                go_through_marker(line_fw_speed, col);

                // go forward until we leave the green
                if (parking_markers == mapman_get_parking_idx())
                    return HugWall;

                continue;

            case Red:
                // intersection, go forward
                go_through_marker(line_fw_speed, col);
                continue;

            case Blue:
                go_through_marker(line_fw_speed, col);
                // switch to wall hugging
                return HugWall;

            // the line
            case Other: break;
        }

        float out = pid_update(&pid, hsv.v);
        bobot_motor(line_fw_speed + out, line_fw_speed - out);
    }
}

bobot_mode hug_wall(void) {
    pid pid;
    pid_init(&pid);

    while (true) {
        pid_set_consts(&pid, wall_kp, wall_ki, wall_kd, -0.7 * 0xff, 0.7 * 0xff);

        map_color col = classify_color(bobot_color_hsv());
        switch (col) {
            case Green:
                go_through_marker(wall_fw_speed, col);

                // we reached the parking spot
                bobot_motor(0, 0);
                return Stop;

            case Blue:
                // switch to line following
                go_through_marker(wall_fw_speed, col);
                return FollowLine;

            // ignore
            default: break;
        }

        uint16_t dist = bobot_ultra_distance();
        float out = pid_update(&pid, dist);
        bobot_motor(50 + out, 50 - out);
    }
}

void do_course(void) {
    // look to the left
    bobot_servo_deg(90);

    // wait for servo to turn left
    sleep_ms(500);

    parking_markers = 0;

    go_through_marker(wall_fw_speed, Green);

    bobot_mode next = HugWall;
    while (next != Stop) {
        if (next == HugWall)
            next = hug_wall();
        else if (next == FollowLine)
            next = follow_line();
    }
}

#include <lwip/stats.h>
int main() {
    stdio_init_all();
    // bobot_init();

    if (!net_init()) {
        while (true) {
            puts("Network failed to init");
            sleep_ms(1000);
        }
    }

    if (!debug_connect_server()) {
        while (true) {
            puts("Network failed to init");
            sleep_ms(1000);
        }
    }

    // while (true) {
    //     do_course();
    // }

    volatile int8_t left = 12, right = 21;
    debug_add_remote_var(left, -100, 100);
    debug_add_remote_var(right, -100, 100);

    volatile float Ki = -1;
    debug_add_remote_var(Ki, 0, 100);

    volatile bool net_log = true;
    debug_add_remote_var(net_log);

    volatile uint32_t sleep = 1000;
    debug_add_remote_var(sleep, 1, 10 * 1000);

    // TODO: irq disable on read...?
    volatile hsv_color col = (hsv_color) { .h = 123, .s = 0.8, .v = 0.9 };
    debug_add_remote_var(col);

    debug_send_remote_vars();

    for (uint32_t i = 0;; i++) {
        // bobot_motor(left, right);

        bobot_led(i % 2);

//        MEM_STATS_DISPLAY();

        if (net_log) {
            debug_printf(
                "%d left: %03d right: %03d "
                "Ki: %02.2f h: %03.2f s: %.2f v: %.2f "
                "\n", //
                (int) i, (int) left, (int) right, (int) Ki, col.h, col.s, col.v
            );
        } else {
            printf(
                "%d left: %03d right: %03d "
                "Ki: %02.2f h: %03.2f s: %.2f v: %.2f "
                "\n", //
                (int) i, (int) left, (int) right, Ki, col.h, col.s, col.v
            );
        }

        sleep_ms(sleep);
    }
}

//////////////////////////////////////////

// #include "hardware/gpio.h"
// #include "hardware/irq.h"
//
// #include "pico.h"
// #include "pico/stdio.h"
// #include "pico/time.h"
//
// #include <pico/platform/sections.h>
// #include <stdint.h>
// #include <stdio.h>
//
// #define IRQ_RISE(pin) (ints[pin / 8] & (GPIO_IRQ_EDGE_RISE << (4 * (pin % 8))))
// #define IRQ_FALL(pin) (ints[pin / 8] & (GPIO_IRQ_EDGE_FALL << (4 * (pin % 8))))
//
// volatile uint32_t irqs = 0;
//
// static void _bobot_gpio_irq(void) {
//     uint64_t now = time_us_64();
//
//     for (uint gpio8 = 0; gpio8 < NUM_BANK0_GPIOS; gpio8 += 8) {
//
//         // events for 8 gpios
//         uint32_t events8 = io_bank0_hw->proc0_irq_ctrl.ints[gpio8 / 8];
//
//         // note we assume events8 is 0 for non-existent GPIO
//         for (uint gpio = gpio8; events8 && gpio < gpio8 + 8; gpio++) {
//
//             // events for a single gpio
//             uint32_t event_mask = events8 & 0b00001111;
//
//             if (event_mask != 0) {
//                 io_bank0_hw->intr[gpio / 8] = event_mask << (4 * (gpio % 8));
//                 gpio_acknowledge_irq(gpio, event_mask);
//                 printf("IRQ on pin %d at %lld!\n", gpio, now);
//             }
//
//             events8 >>= 4;
//         }
//     }
//
//     io_ro_32 ints[4] = { io_bank0_hw->proc0_irq_ctrl.ints[0], io_bank0_hw->proc0_irq_ctrl.ints[1],
//                          io_bank0_hw->proc0_irq_ctrl.ints[2], io_bank0_hw->proc0_irq_ctrl.ints[3] };
//
//     io_bank0_hw->intr[0] = 0;
//
//     if (IRQ_RISE(6))
//         puts("rise on pin 6");
//
//     irqs++;
// }
//
// int main() {
//     stdio_init_all();
//
//     sleep_ms(1000);
//     puts("Starting...");
//
//     printf(
//         "has handler: %s, is enabled: %s\n",              //
//         irq_has_handler(IO_IRQ_BANK0) ? "true" : "false", //
//         irq_is_enabled(IO_IRQ_BANK0) ? "true" : "false"
//     );
//
//     irq_set_exclusive_handler(IO_IRQ_BANK0, _bobot_gpio_irq);
//     irq_set_enabled(IO_IRQ_BANK0, true);
//
//     uint a[] = { 6, 7, 8, 9 };
//     for (uint i = 0; i < sizeof(a) / sizeof(a[0]); i++) {
//         gpio_init(a[i]);
//         gpio_set_irq_enabled(a[i], GPIO_IRQ_EDGE_RISE | GPIO_IRQ_EDGE_FALL, true);
//     }
//
//     for (int i = 0;; i++) {
//         sleep_ms(1000);
//         printf("%d irqs: %d\n", i, (int) irqs);
//     }
// }
