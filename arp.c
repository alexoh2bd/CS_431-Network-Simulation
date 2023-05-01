#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <arpa/inet.h>
#include "arp.h"

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
            arpTbl[i].ip = ip_addr;
            return 0;
        }
    }
    return -1;
}

uint8_t *arp_lookup(uint32_t ip){
    // search by ip address
    printf("looking up arp for %08X\n", ip);
    for(int i = 0; i < ARP_CACHE_SIZE; i++){
        if(arpTbl[i].ip == ntohl(ip)){
            printf("found arp for %08X\n", ntohl(ip));
            return arpTbl[i].eth;
        }
    }
    return NULL;

}


