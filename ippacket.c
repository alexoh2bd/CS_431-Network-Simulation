
#include <stdio.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>


#include "crc32.h"
#include "util.h"
#include "cs431vde.h"
#include "stack.h"
#include "ethernet.h"
#include "ip.h"
#include "arp.h"



uint16_t 
compute_headchksum(struct IPheader *ip){
    // ones complement of sum of ones complement of each octet
    int  ihl;
    ihl = (ip->ihl & 0xf)* 4;
    
    uint16_t *s = ip;
    uint32_t sum = 0;
    uint16_t checksum;
    while(ihl > 1) {
        sum+=(uint16_t) *s;
        ihl -=2;
        s++;
    }
    if(ihl > 0){
        sum+=(uint8_t) *s;
    }
    while(sum >> 16){
        sum = (sum &0xffff) + (sum>>16);
    }
    checksum = ~sum & 0xffff;
    return checksum;
}


int verify_checksum(struct IPheader *ip, uint16_t *shouldbe){
    // ih with zero checksum
    struct IPheader tempih;
    int ihl;
    uint16_t calculated_checksum;

    // printf("      ip->checksum: %04x\n", ntohs(ip->headchksum));
    // use IP header length to compute checksum
    ihl = (ip->ihl & 0xf) * 4;
    memcpy (&tempih, ip, ihl);
    tempih.headchksum = 0;
    calculated_checksum = compute_headchksum(&tempih);
    
    if(calculated_checksum != ntohs(ip->headchksum)){
        if(shouldbe){
            *shouldbe = calculated_checksum;
        }
        return 1;

    }
    return 0;

}
uint16_t compute_icmp_checksum(struct icmpheader *icmp){
     // ones complement of sum of ones complement of each octet
    int  ihl;
    ihl = 8;
    
    uint16_t *s = icmp;
    uint32_t sum = 0;
    uint16_t checksum;
    while(ihl > 1) {
        sum+=(uint16_t) *s;
        ihl -=2;
        s++;
    }
    if(ihl > 0){
        sum+=(uint8_t) *s;
    }
    while(sum >> 16){
        sum = (sum &0xffff) + (sum>>16);
    }
    checksum = ~sum & 0xffff;
    return checksum;
}
int compose_ICMP_frame(uint8_t *frame, struct icmpheader *icmp, uint8_t *data, size_t data_len){
    uint32_t fcs;
    memset(&icmp->unused, '\0', 8);
    icmp->checksum = compute_icmp_checksum(icmp);

    memcpy(frame, icmp, sizeof(*icmp));
    memcpy(frame + sizeof(*icmp), data, data_len);

    
    fcs = crc32(0, frame, sizeof(struct eth_header) + data_len);
    memcpy(frame+ sizeof(*icmp) + data_len, &fcs, ETH_FCS_LEN);
    printf("frame: %s\n", binary_to_hex(frame, data_len + sizeof(*icmp) + sizeof(fcs)));
    
    
    return data_len + sizeof(*icmp);
    
}