#ifndef __IPPACKET_H
#define __IPPACKET_H

#include <stdint.h>

#include "stack.h"
#include "ip.c"
#include "ethernet.c"


uint16_t compute_headchksum(struct IPheader *ip);
int verify_checksum(struct IPheader *ip, uint16_t *shouldbe);
uint16_t compute_icmp_checksum(struct icmpheader *icmp);
int compose_ICMP_frame(uint8_t *frame, struct icmpheader *icmp, uint8_t *data, size_t data_len);

#endif