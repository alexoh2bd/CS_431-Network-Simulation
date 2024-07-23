#ifndef __ARP_H
#define __ARP_H

#include <stdint.h>
#include "stack.h"
void arp_init(void);

int add_arp(uint8_t *eth_addr, uint32_t ipaddr);
uint8_t *arp_lookup(uint32_t ip);

int handle_arp_packet(struct interface *iface, uint8_t *packet, int packet_len);
struct arp_header{
    uint16_t htype;
    uint16_t ptype;
    uint8_t hardaddlen;
    uint8_t protaddlen;
    uint16_t opcode;
    uint8_t sendereth[6];
    uint8_t senderip[4];
    uint8_t targeteth[6];
    uint32_t targetip;
};

#endif
