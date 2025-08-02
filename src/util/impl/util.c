#include "util/util.h"

#include <math.h>

void util_rgb_rel(rgb_sensor_color_raw* raw) {
    if (raw->c == 0) {
        raw->r = 0;
        raw->g = 0;
        raw->b = 0;
        return;
    }

    raw->r = (0xffff * raw->r) / raw->c;
    raw->g = (0xffff * raw->g) / raw->c;
    raw->b = (0xffff * raw->b) / raw->c;
    raw->c = 0;
}

hsv_color util_rgb_to_hsv(uint16_t rr, uint16_t gg, uint16_t bb) {
    float r = (float) rr / 0xffff;
    float g = (float) gg / 0xffff;
    float b = (float) bb / 0xffff;

    float max = MAX(MAX(r, g), b);
    float min = MIN(MIN(r, g), b);
    float delta = max - min;

    hsv_color hsv = (hsv_color) {
        .h = 0,
        .s = max == 0 ? 0 : (max - min) / max,
        .v = max,
    };

    if (delta == 0)
        return hsv;

    if (r == max) {
        hsv.h = (g - b) / delta;
    } else {
        if (g == max)
            hsv.h = 2 + (b - r) / delta;
        else
            hsv.h = 4 + (r - g) / delta;
    }

    hsv.h *= 60;
    if (hsv.h < 0)
        hsv.h += 360;

    return hsv;
}
