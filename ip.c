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

#define MAX_ROUTES 10

struct route routingTbl[MAX_ROUTES];

int route_packet(uint8_t *packet, ssize_t packet_len);
uint16_t compute_headchksum(struct IPheader *ip);
struct route * lookup_route(uint32_t destip);
int verify_checksum(struct IPheader *ip, uint16_t *shouldbe);


extern struct interface interfaces[];

void 
ip_init(void){
    memset(routingTbl, '\0', sizeof(routingTbl));
}

int handle_ip_packet(struct interface *iface, uint8_t *packet, int packet_len){
    // printf("hex packet: %s\n", binary_to_hex(packet, packet_len));
    struct IPheader *ip;
    ip =(struct IPheader *)packet;
    uint16_t checksum_should_be;


    char *hexsrcaddress = binary_to_hex(&ip->srcAddress, 5);
    char *hexdestaddress = binary_to_hex(&ip->dstAddress, 5);
    

    // Time to live = 0
    if(ip->ttl == 0){
        printf("Dropping packet from %s to %s (TTL exceeded)\n", hexsrcaddress, hexdestaddress);
    }

    // Length of IP packet does not match
    else if(ntohs(ip->length) != packet_len){
        printf("Dropping packet from %s (wrong length)\n", hexsrcaddress);
        return -1;
    }

    // check header checksum
    else if(verify_checksum(ip, &checksum_should_be) == 0){
        printf("Dropping packet: bad IP header checksum (is %04x, should be %04x)\n", ip->headchksum, checksum_should_be);
        printf("headchecksum: %u\n", ntohs(ip->headchksum));
        return -1;
    }

    for(int i = 0; i < MAX_INTERFACES; i++){
        if(ip->dstAddress == interfaces[i].ip_addr){
            printf("  delivering locally\n");
            return 0;
        }
    }

    printf("Routing packet to %s\n", hexdestaddress);
    return route_packet(packet, packet_len);


    // // check for arp entry in cache
    // else if(inArp ==0){
    //         printf("Dropping packet from %s to %s (no ARP)\n", hexsrcaddress, hexdestaddress);
    // }`

}
int add_route(uint32_t dest, uint32_t nm, uint32_t gw, struct interface *iface){
    // fill in first empty route entry in table
    for(int i = 0; i< MAX_ROUTES; i++){
        if(routingTbl[i].iface == NULL){
            routingTbl[i].destination = dest;
            routingTbl[i].netmask = nm;
            routingTbl[i].gateway = gw;
            routingTbl[i].iface = iface;
            return 0;
        }
    }

    return -1;
}



// Routes packet
int route_packet(uint8_t *packet, ssize_t packet_len){
    struct IPheader *ip = (struct IPheader *)packet;
    struct route *r;
    uint8_t *destethaddr;
    struct eth_header tempeth;
    uint8_t frame[ETH_MAX_FRAME_LEN];

    // find eth address associated with MAC address
    r = lookup_route(ntohl(ip->dstAddress));
    if(r == NULL){
        printf("  dropping packet: no matching route\n");
        return -1;
    }
    printf("  route is dest: %08x, nm: %08x, r->gateway: %08x\n", r->destination, r->netmask, r->gateway);


    ip->ttl--;
    ip->headchksum = 0;
    ip->headchksum = compute_headchksum(ip);


    
    // Determines if packet is meant for a device directly connected to network
    if(r->gateway == 0x00000000){
        printf("  destination host is on attached network\n");
        destethaddr = arp_lookup(ip->dstAddress);
    }else{
        // finds gateway network address in ARP to forward packets
        printf("  packet must be routed\n");
        destethaddr = arp_lookup(r->gateway);
    }
    if(destethaddr == NULL){
        printf("  dropping packet: no ARP\n");
        return 0;
    }


    // Create new eth_frame to pass on to next interface/host
    printf("  forwarding IP packet\n");
    memcpy(tempeth.dst_addr, destethaddr, 6);
    memcpy(tempeth.src_addr,r->iface->eth_addr, 6);
    tempeth.type = htons(ETH_TYPE_IP);

    size_t frame_len = compose_ethernet_frame(frame, &tempeth, packet, packet_len);


    // Send frame to next interface
    printf("  sending a %ld-byte ethernet frame\n", frame_len);
    send_ethernet_frame(r->iface->out_fd, frame, frame_len);

    return 0;

}

// look for matching route
struct route *
lookup_route(uint32_t destip){
    for(int i = 0; i< MAX_ROUTES&& routingTbl[i].iface; i++){
        // printf("Route:%08x, %08x\n", (destip & routingTbl[i].netmask), routingTbl[i].destination);

        //use bitwise and to find destination network address
        if((destip & routingTbl[i].netmask) ==routingTbl[i].destination){
            return &routingTbl[i];
        }
    }
    printf("Could not find route\n");
    return NULL;
}

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

    // use IP header length to compute checksum
    ihl = (ip->ihl & 0xf) * 4;
    memcpy (&tempih, ip, ihl);
    tempih.headchksum = 0;
    calculated_checksum = compute_headchksum(&tempih);
    
    if(calculated_checksum != ip->headchksum){
        if(shouldbe){
            *shouldbe = calculated_checksum;
        }
        return -1;

    }
    return 0;


}