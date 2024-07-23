

#ifndef __STACK_H
#define __STACK_H

#include <stdint.h>

struct interface {
    uint8_t     eth_addr[6];
    uint32_t    ip_addr;
    char *      name;
    int         in_fd;
    int         out_fd;
};


#define MAX_INTERFACES 5
#endif
