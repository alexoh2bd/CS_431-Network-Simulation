#ifndef __ETHERNET_H
#define __ETHERNET_H

#include "stack.h"
#include <stdint.h>

struct eth_header{
    uint8_t dst_addr[6];
    uint8_t src_addr[6];
    uint16_t type;

};

#define ETH_MIN_DATA_LEN 46
#define ETH_MAX_DATA_LEN 1500
#define ETH_FCS_LEN 4

#define ETH_MIN_FRAME_LEN (sizeof(struct eth_header) + ETH_MIN_DATA_LEN + ETH_FCS_LEN)
#define ETH_MAX_FRAME_LEN (sizeof(struct eth_header) + ETH_MAX_DATA_LEN + ETH_FCS_LEN)

int 
handle_ethernet_frame(struct interface *iface);
int 
compose_ethernet_frame(uint8_t *frame, struct eth_header *eh, uint8_t *data, size_t data_len);
#endif
