#ifndef MAPMAP_H
#define MAPMAP_H

#include "pico/types.h"

void mapman_connect_server(void);

void mapman_arrive(void);
void mapman_leave(void);

void mapman_can_leave(void);

int mapman_get_parking_idx(void);

void _mapman_recv_byte(uint8_t b);

#endif
