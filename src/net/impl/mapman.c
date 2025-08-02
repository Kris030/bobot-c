#include "mapman/mapman.h"
#include "net/net.h"

#include "config/config.h"

#include "lwip/err.h"
#include "lwip/pbuf.h"
#include "lwip/tcp.h"
#include "lwip/tcpbase.h"

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static struct tcp_pcb* _mapman_client_pcb = NULL;

static volatile bool _net_mapman_connected = false;
static volatile uint8_t _net_mapman_preconn_buf[0xff];
static volatile size_t _net_mapman_preconn_buf_count = 0;

static err_t _mapman_connected_cb(void* arg, struct tcp_pcb* tpcb, err_t err) {
    // shouldn't happen...
    if (err != ERR_OK) {
        printf("Connection failed with error: %d\n", err);
        return err;
    }

    puts("Connected to mapman server");
    return ERR_OK;
}

static err_t _mapman_recv_cb(void* _arg, struct tcp_pcb* tpcb, struct pbuf* p, err_t err) {
    if (p == NULL)
        return tcp_close(tpcb);

    if (err != ERR_OK)
        return err;

    struct pbuf* n = p;
    while (n != NULL) {
        for (typeof(n->len) i = 0; i < n->len; i++) {
            _mapman_recv_byte(((uint8_t*) n->payload)[i]);
        }

        n = n->next;
    }

    if (p->tot_len > 0)
        tcp_recved(tpcb, p->tot_len);

    pbuf_free(p);

    return ERR_OK;
}

// static err_t _mapman_sent_cb(void* arg, struct tcp_pcb* tpcb, u16_t len) {}

bool _net_connect_mapman_server(void) {
    ip_addr_t server_ip;
    ip4addr_aton(CONF_MAPMAN_SERVER_IP, &server_ip);

    _mapman_client_pcb = tcp_new_ip_type(IP_GET_TYPE(server_ip));
    if (!_mapman_client_pcb) {
        puts("[ERROR] Failed to create mapman server PCB");
        return false;
    }

    // tcp_sent(_mapman_client_pcb, _mapman_sent_cb);
    tcp_recv(_mapman_client_pcb, _mapman_recv_cb);
    tcp_err(_mapman_client_pcb, _net_err_cb);
    tcp_arg(_mapman_client_pcb, NULL);

    return tcp_connect(_mapman_client_pcb, &server_ip, CONF_MAPMAN_SERVER_PORT, _mapman_connected_cb) == ERR_OK;
}

void _net_mapman_write(const void* buf, size_t size) {
    if (!_net_mapman_connected) {
        memcpy((void*) (_net_mapman_preconn_buf + _net_mapman_preconn_buf_count), buf, size);
        _net_mapman_preconn_buf_count += size;
        return;
    }

    if (tcp_write(_mapman_client_pcb, buf, size, TCP_WRITE_FLAG_COPY) != ERR_OK)
        puts("[ERROR] Failed to send data to mapman server");
}
