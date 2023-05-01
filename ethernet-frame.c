#ifndef __ETHERNET_FRAME_H
#define __ETHERNET_FRAME_H

#include <stdint.h>
#include <sys/types.h>

struct eth_header{
    uint8_t dst_addr[6];
    uint8_t src_addr[6];
    uint16_t type;

};

#define ETH_MIN_DATA_LEN 46
#define ETH_MAX_DATA_LEN 1500

#define ETH_MIN_FRAME

#endif