#ifndef __ARP_H
#define __ARP_H

#include <stdint.h>
void arp_init(void);

int add_arp(uint8_t *eth_addr, uint32_t ipaddr);
uint8_t *arp_lookup(uint32_t ip);

#endif
