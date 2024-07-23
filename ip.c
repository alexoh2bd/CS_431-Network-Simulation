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
#include "tcp1.h"

#define MAX_ROUTES 10

struct route routingTbl[MAX_ROUTES];

int route_packet(uint8_t *packet, ssize_t packet_len);
int verify_checksum(struct IPheader *ip, uint16_t *shouldbe);
uint16_t compute_icmp_checksum(struct icmpheader *icmp, uint8_t *data, size_t data_len);
int compose_ICMP_frame(uint8_t *frame, struct icmpheader *icmp, uint8_t *data, size_t data_len);



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

    
    char *hexdestaddress = binary_to_hex((&ip->dstAddress), 5);

    // take out fcs count
    packet_len -= 4;


    if(ip->ttl ==0){
        printf("  Dropping packet: TTL is 0\n");
        return -1;
    }

    // check header checksum
    if(verify_checksum(ip, &checksum_should_be) != 0){
        printf("  Dropping packet: bad IP header checksum (is %04x, should be %04x)\n", (ip->headchksum), checksum_should_be);
        return -1;
    }

    // handling tcp packets
    for(int i = 0; i < MAX_INTERFACES; i++){
        if(ip->dstAddress == interfaces[i].ip_addr){
            printf("  delivering locally: handling TCP packet\n");

            handle_tcp_packet(packet, packet_len);
            return 0;
        }
    }

    printf("  Routing packet to %s\n", (hexdestaddress));
    free(hexdestaddress);
    return route_packet(packet, packet_len+4);
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
   
    struct icmpheader *icmp;
    struct IPheader *ip = (struct IPheader *)packet;
    struct route *r;
    uint8_t *destethaddr;
    struct eth_header tempeth;
    size_t frame_len;
    ip->tos = 0;
    ip->ttl--;
    ip->headchksum = 0;
    ip->headchksum = compute_headchksum(ip);
        


   
    // find associated eth address
    r = lookup_route((ip->dstAddress));
    if(r == NULL){
        // Network Unreachable error, no route
        icmp = malloc(sizeof(icmp));
        printf("  dropping packet: no matching route. Network Unreachable.\n");
        icmp->type = 3;
        icmp->code = 0;
        icmp->checksum = 0;
        send_ICMP(icmp, packet, packet_len, r);
        free(icmp);

        return -1;
    }
    printf("  route is dest: %08x, nm: %08x, r->gateway: %08x\n", r->destination, r->netmask, r->gateway);


    if(ip->ttl <= 0){   
        printf("Dropping packet, (TTL exceeded)\n");
        icmp = malloc(sizeof(icmp));
        // set ICMP values
        icmp->type = 11;
        if(ip-> ttl ==0){
            icmp->code = 0;
        }else{
            icmp->code = 1;
        }
        icmp->checksum = 0;

        send_ICMP(icmp, packet, packet_len, r);
        free(icmp);
        return -1;
    }

  
    // Determines if packet is meant for a device directly connected to network
    if(r->gateway == 0x00000000){
        printf("  destination host is on attached network\n");
        destethaddr = arp_lookup((ip->dstAddress));
    }
    else{
        // finds gateway network address in ARP to forward packets
        printf("  packet must be routed\n");
        destethaddr = arp_lookup(htonl(r->gateway));
    }
    if(destethaddr == NULL){
        printf("  dropping packet: no ARP. Host Unreachable.\n");
        icmp = malloc(sizeof(icmp));
        icmp->type = 3;
        icmp->code = 1;
        icmp->checksum = 0;
        send_ICMP(icmp, packet, packet_len,r);
        free(icmp);
        return 0;
    }


    // Create new eth_frame to pass on to next interface/host
    memcpy(tempeth.dst_addr, destethaddr, 6);
    memcpy(tempeth.src_addr,r->iface->eth_addr, 6);
    tempeth.type = (0x0008);

    uint8_t frame[ETH_MAX_FRAME_LEN];

    frame_len = compose_ethernet_frame(frame, &tempeth, packet, packet_len-4);

    // Send frame to next interface
    printf("  Forwarding a %ld-byte ethernet frame to MAC Address %s and interface in:%d out:%d\n", frame_len, binary_to_hex(destethaddr,7), r->iface->in_fd, r->iface->out_fd);
    send_ethernet_frame(r->iface->out_fd, frame, frame_len);

    return 0;

}

// look for matching route
struct route *
lookup_route(uint32_t destip){

    for(int i = 0; i< MAX_ROUTES&& routingTbl[i].iface; i++){  
        // printf("route: %08x\n", routingTbl[i].destination);
        // printf("destip & netmask: %08x\n", (destip & routingTbl[i].netmask));

        //use bitwise and to find destination network address
        if(((destip & routingTbl[i].netmask) == routingTbl[i].destination)|| ((destip & routingTbl[i].netmask) == ntohs(routingTbl[i].destination))){
            return &routingTbl[i];
        }
    }
    printf("Could not find route\n");
    return NULL;
}
int 
send_ICMP(struct icmpheader *icmp, uint8_t * packet, ssize_t packet_len, struct route * r){
    size_t icmp_len;
    uint8_t icmpframe[ETH_MAX_FRAME_LEN];    
    uint8_t ipframe[ETH_MAX_FRAME_LEN];
    uint8_t frame[ETH_MAX_FRAME_LEN];
    struct eth_header *tempeth = malloc(sizeof(struct eth_header));
    struct IPheader *ip = (struct IPheader *)packet;
    
    size_t frame_len;


    
    if(r == NULL){
        printf("could not lookup return route to send ICMP packet\n");
        return -1;
    }

    // find ethernet address to return ICMP packet
    uint8_t *temparpeth ;
    temparpeth = malloc(6);
    temparpeth = arp_lookup((ip->srcAddress));
    if(temparpeth == NULL){
        printf("Couldn't find ARP entry to return ICMP packet: %08X\n", ip->srcAddress);
        return -1;
    }
    
    if(packet_len > 8 +sizeof(*ip)){
        packet_len = 8+sizeof(*ip);
    }
    


    // IP header
    ip->ttl++;
   
    ip->headchksum = 0;
    ip->headchksum = compute_headchksum(ip);
   
    // compose ICMP header 
    icmp_len = compose_ICMP_frame(icmpframe, icmp, packet, packet_len);
    


    ip->protocol = 0x01;
    ip->ttl = 0x10;
    // uint32_t tempdestip = ip->dstAddress;
    ip->dstAddress = ip->srcAddress;
    ip->srcAddress = r->iface->ip_addr;
    ip->length = htons(icmp_len + sizeof(*ip));

    

    ip->headchksum = 0;
    ip->headchksum = compute_headchksum(ip);
    
    memcpy(ipframe, packet, sizeof(*ip));
    memcpy(ipframe + sizeof(*ip), icmpframe, icmp_len);
     printf("ip frame: %s\n", binary_to_hex(ipframe, htons(ip->length)));

    // Ethernet header
    memcpy (tempeth->dst_addr, temparpeth, 6); // destination eth
    memcpy (tempeth->src_addr, r->iface->eth_addr, 6); // source eth
    tempeth->type = (htons(ETH_TYPE_IP));


    frame_len = compose_ethernet_frame(frame, tempeth, ipframe, htons(ip->length));
    
    // reroute, send it back
    free(tempeth);

    send_ethernet_frame(r->iface -> out_fd, frame, frame_len);
    return 0;
}


uint16_t 
compute_headchksum(struct IPheader *ip){
    // ones complement of sum of ones complement of each octet
    int  ihl;
    ihl = (ip->ihl & 0xf)* 4;
    
    uint16_t *s;
    s = (uint16_t *)ip; 
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
    return ntohs(checksum);
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
    
    if(calculated_checksum != htons(ip->headchksum) && calculated_checksum != (ip->headchksum)){
        if(shouldbe){
            *shouldbe = calculated_checksum;
        }
        return 1;

    }
    return 0;

}
uint16_t compute_icmp_checksum(struct icmpheader *icmp, uint8_t *data, size_t data_len){
    
    size_t icmpsize = sizeof(struct icmpheader);
    uint32_t sum = 0;
    uint16_t checksum;
    size_t size = data_len+sizeof(*icmp);
    
    uint8_t tempframe [size+1];
    memcpy(tempframe, icmp, icmpsize);
    memcpy(tempframe+icmpsize, data, data_len);

    uint16_t *s;
    s = (uint16_t *)tempframe;

    while(size > 1) {
        sum+=(uint16_t) *s;
        size -=2;
        s++;
    }
    if(size > 0){
        sum+=(uint8_t) *s;
        sum+=0xff;
    }
    while(sum >> 16){
        sum = (sum &0xffff) + (sum>>16);
    }
    checksum = ~sum & 0xffff;
    return checksum;
}

int compose_ICMP_frame(uint8_t *frame, struct icmpheader *icmp, uint8_t *data, size_t data_len){
    memset(&icmp->unused, '\0', 8);
    icmp->checksum = 0;

    icmp->checksum = (compute_icmp_checksum(icmp, data, data_len));
   
    memcpy(frame, icmp, sizeof(*icmp));
    memcpy(frame + sizeof(*icmp), data, data_len);

    return data_len +sizeof(*icmp);
    
}
