#include "net/net.h"

#include "config/net.h"

#include "cyw43.h"
#include "pico/cyw43_arch.h"

#include <stdio.h>

static bool _net_init = false;

bool net_init(void) {
    if (net_isinit()) {
        puts("[WARNING] net already init");
        return false;
    }

    if (!cyw43_is_initialized(&cyw43_state)) {
        if (cyw43_arch_init()) {
            puts("[ERROR] Failed to init network");
            return false;
        }
    } else
        puts("[INFO] cyw43 already inited outside net, skipping");

    cyw43_arch_enable_sta_mode();

    if (cyw43_arch_wifi_connect_blocking(CONF_WIFI_SSID, CONF_WIFI_PASSW, CONF_WIFI_AUTH)) {
        puts("[ERROR] Failed to connect to wifi");
        cyw43_arch_deinit();
        return false;
    }

    _net_init = true;

    return true;
}

void net_poll(void) {
    cyw43_arch_poll();
}

void _net_err_cb(void* arg, err_t err) {
    (void) arg;

    char* e;

    switch (err) {
        case ERR_OK: e = "No error, everything OK"; break;
        case ERR_MEM: e = "Out of memory error"; break;
        case ERR_BUF: e = "Buffer error"; break;
        case ERR_TIMEOUT: e = "Timeout"; break;
        case ERR_RTE: e = "Routing problem"; break;
        case ERR_INPROGRESS: e = "Operation in progress"; break;
        case ERR_VAL: e = "Illegal value"; break;
        case ERR_WOULDBLOCK: e = "Operation would block"; break;
        case ERR_USE: e = "Address in use"; break;
        case ERR_ALREADY: e = "Already connecting"; break;
        case ERR_ISCONN: e = "Conn already established"; break;
        case ERR_CONN: e = "Not connected"; break;
        case ERR_IF: e = "Low-level netif error"; break;
        case ERR_ABRT: e = "Connection aborted"; break;
        case ERR_RST: e = "Connection reset"; break;
        case ERR_CLSD: e = "Connection closed"; break;
        case ERR_ARG: e = "Illegal argument"; break;

        default: e = "unknown error";
    }

    printf("[ERROR] lwip error: %s\n", e);
}

bool net_isinit(void) {
    return _net_init;
}
