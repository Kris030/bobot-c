#include "mapman/mapman.h"
#include "net/net.h"

#include "pico/unique_id.h"

// server -> pico:
//   - 0x01: start course
//
// pico -> server:
//   - on connect:
//     - u64: pico id
//
//   - 0x01 - leaving parking lot
//   - 0x02 - arrived at parking lot
//   - 0x03 - progress info

void mapman_connect_server(void) {
    _net_connect_mapman_server();

    pico_unique_board_id_t id;
    pico_get_unique_board_id(&id);
    _net_mapman_write(&id, sizeof(id));
}

void mapman_leave(void) {
    uint8_t leave = 0x01;
    _net_mapman_write(&leave, sizeof(leave));
}
void mapman_arrive(void) {
    uint8_t arrive = 0x02;
    _net_mapman_write(&arrive, sizeof(arrive));
}

void mapman_can_leave(void) {}

void _mapman_recv_byte(uint8_t b) {}
