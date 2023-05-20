#ifndef __TCP1_H
#define __TCP1_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <arpa/inet.h>


struct tcpheader {
    uint16_t sourcePort;
    uint16_t destinationPort;
    uint32_t seqNumber;
    uint32_t ackNumber;
    uint8_t headlen;
    uint8_t flags;
    uint16_t window;
    uint16_t checksum;
    uint16_t urgentpointer;
    uint8_t options[3];
    uint8_t padding;
};

struct tcpconnection {
    uint16_t port;
    uint32_t ipaddr;
    uint8_t * eth;
};
void tcp_init(void);


int handle_tcp_packet(uint8_t *payload, size_t payload_len);


#endif 