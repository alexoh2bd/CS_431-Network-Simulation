#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <arpa/inet.h>

#include "arp.h"
#include "stack.h"
#include "ethernet.h"
#include "util.h"
#include "crc32.h"
#include "cs431vde.h"
#include "ip.h"

#define ARP_CACHE_SIZE 10

struct arp_entry{
    uint8_t *eth;
    uint32_t ip;
};
struct arp_entry arpTbl[ARP_CACHE_SIZE];


void arp_init(void){
    memset(arpTbl, '\0', sizeof(arpTbl));
}

// add an arp entry to cache
int add_arp(uint8_t *eth_addr, uint32_t ip_addr){
    // find empty arp entry to insert
    for (int i = 0; i<ARP_CACHE_SIZE; i++){
        if(arpTbl[i].eth == NULL){
            arpTbl[i].eth = malloc(6);
            memcpy(arpTbl[i].eth, eth_addr, 6);
            arpTbl[i].ip = (ip_addr);
            return 0;
        }
    }
    return -1;
}
// takes host(little endian) address as argument
uint8_t *arp_lookup(uint32_t ip){
    // search by ip address
    // ip = htonl(ip);
    // printf("IP: %08X\n", ip);
    for(int i = 0; i < ARP_CACHE_SIZE; i++){
        // printf("arbtbl[%d]: %08X    \n", i, arpTbl[i].ip);
        if(arpTbl[i].ip == ip){
            // printf("found arp for %08X\n", ntohl(ip));
            return arpTbl[i].eth;
        }
    }
    printf("could not find arp for %08X\n", (ip));
    return NULL;

}
int 
handle_arp_packet(struct interface *iface, uint8_t *packet, int packet_len){
    struct arp_header *arp = (struct arp_header *)packet;
    // printf("hardaddr: %04X\n",arp->protaddlen);
    // printf("arp opcode: %04X\n",arp-> opcode);

    // this is a request packet
    if(ntohs(arp->opcode) == 1){
        uint8_t frame[ETH_MAX_FRAME_LEN];
        struct eth_header *eh = malloc(sizeof(struct eth_header));
        memcpy(eh-> dst_addr, (arp->sendereth), 6);
        memcpy(eh-> src_addr, iface->eth_addr, 6);
        eh->type = (0x0608);


        // change sender mac and ip addresses and target mac address
        arp->opcode = (0x0200); 
        uint32_t tempip = (iface->ip_addr);
        
        memcpy(arp->sendereth, &iface->eth_addr, 6);
        memcpy(arp->senderip, &tempip, 4);

        uint8_t *temptargeteth = arp_lookup((arp->targetip));
        
        // enters new arp entry if no arp is found
        if (temptargeteth == NULL){
            printf("   ARP was not found\n");
            // add_arp(arp->sendereth, arp->targetip);
            // printf("   Added new arp entry to cache: Eth %s, IP %08X\n\n", binary_to_hex(arp->sendereth,7), arp->targetip);
            return 0;
        }else{
        memcpy(arp->targeteth, temptargeteth, 6);
        
        }
        printf("found eth %s for ip %08X\n", binary_to_hex(arp->targeteth,7), arp->targetip);
        int frame_len = compose_ethernet_frame(frame, eh, packet, sizeof(struct arp_header)+1);
        // printf("final frame:\n %s\n", binary_to_hex(frame, frame_len+1) );

        send_ethernet_frame(iface->out_fd, frame, frame_len);
    }else{
        printf("  ARP frame was not a a request\n");
    }


    return 0;
}


