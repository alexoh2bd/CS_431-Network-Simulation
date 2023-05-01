
#ifndef __IP_H
#define __IP_H

#include <stdint.h>

#include "stack.h"

#define IP_INITIAL_TTL 64

struct route {
    uint32_t destination;
    uint32_t netmask;
    uint32_t gateway;
    struct interface *iface;
};

struct IPheader{
    uint8_t ihl;
    uint8_t tos;
    uint16_t length;
    uint16_t id;
    uint16_t flags;
    uint8_t ttl;
    uint8_t protocol;
    uint16_t headchksum;
    uint32_t srcAddress;
    uint32_t dstAddress;
};


void ip_init(void);
int add_route(uint32_t dest, uint32_t nm, uint32_t gw, struct interface *iface);
int handle_ip_packet(struct interface *iface, uint8_t *packet, int packet_len);


#endif