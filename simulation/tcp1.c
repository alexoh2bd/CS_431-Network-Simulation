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
#include "tcp1.h"

#define maxtcptablesize 10

uint16_t compute_tcp_checksum(struct IPheader * ip, struct tcpheader * tcp, uint8_t * payload, size_t payload_len);
int add_tcp(uint8_t *eth_addr, uint32_t ip_addr);
struct tcpconnection tcpTbl [maxtcptablesize];
int composeAck(struct eth_header * eth,struct IPheader * ip, struct tcpheader * tcp, uint8_t * payload, size_t payload_len);
uint8_t * lookup_tcp(uint32_t ip);

void tcp_init(void){
    memset(tcpTbl, '\0', sizeof(tcpTbl));
}
int
handle_tcp_packet(uint8_t * payload, size_t payload_len){
 
    
    struct IPheader * ip;
    ip = (struct IPheader *) payload;
    struct tcpheader * tcp;
    uint8_t * smallerpayload = payload + sizeof(struct IPheader);
    
    tcp = (struct tcpheader *)(smallerpayload);
    struct eth_header * eth = malloc(sizeof(struct eth_header));

    // respond to ACK packet
    // establish connection through adding tcp to tcp connnection table
    // printf("payload_len: %04X\n", payload_len-sizeof(struct IPheader)-20);
    if(tcp->flags == 0x10){
                printf("    Received ACK response: "); 
        uint8_t *tempeth = arp_lookup(ip->srcAddress);
        if(tempeth == NULL){
            printf("    Unbelievable, there is no arp entry\n");
        }

        add_tcp(tempeth, ip->srcAddress);
        printf("Connection established\n\n");
    }else{

    // prints payload
    // printf("    Payload: \n%s\n\n", binary_to_hex(payload+sizeof(struct IPheader)+20, payload_len-sizeof(struct IPheader)-20));
    if(composeAck(eth,ip,tcp,payload,payload_len) ==-1){
        return -1;
    }}
    free(eth);
    printf("    ACK packet sent\n");


    return 1;
}
// composes Acknowledgement frames for 
int 
composeAck(struct eth_header * eth, struct IPheader * ip, struct tcpheader * tcp, uint8_t * payload, size_t payload_len){

    uint8_t frame[ETH_MAX_FRAME_LEN];
    struct route * r;
    uint16_t tempport  = tcp->sourcePort;
    tcp->sourcePort = tcp ->destinationPort;
    tcp->destinationPort = tempport;
    tcp->headlen = 90;
    tcp->checksum = 0;

    // SYN packet
    if(tcp->flags == 0x02){
        // now a SYN ACK packet
        printf("    SYN ACK packet\n");
        tcp->flags = 0x12;
        uint32_t number = tcp->ackNumber;
        tcp->ackNumber = tcp->seqNumber;
        tcp->ackNumber +=htonl(1);
        tcp->seqNumber = htonl(number+1);
        tcp->checksum = compute_tcp_checksum(ip, tcp, payload+sizeof(*ip) + 20, payload_len - sizeof(struct IPheader)-20);
        printf("    Sending SYN ACK packet: %u\n\n\n", tcp->ackNumber);

    }
    // PSH ACK packet
    else if(tcp->flags == 0x18){
        // respond to PSH - ACK packet
        printf("    PSH ACK packet\n");
    
        // ensures PSH is coming from connected TCP
        uint8_t * tempeth = lookup_tcp(ip->srcAddress);
        if(tempeth == NULL){
            printf("    How is there no tcp entry\n");
            return -1;
        }
        // now an ACK packet
        tcp->flags = 0x10;
        uint32_t number = tcp->ackNumber;
        tcp->ackNumber = tcp->seqNumber;
        tcp->seqNumber = number;
        tcp->ackNumber += htonl(payload_len - 40+4);
        uint8_t * data =payload+sizeof(*ip) + 20;
        size_t data_len = payload_len-40 + 4;
        // printf("Payload length: %u\n", payload_len -40);
        tcp->checksum = compute_tcp_checksum(ip, tcp, data, 0);
        payload_len = 40;
        
        
        char printingdata[data_len];
        memcpy(printingdata, data, data_len);
        // printf("    Received hex message: \n        %s\n", binary_to_hex(printingdata, data_len));

        printf("    Received string message: \n        %s\n", printingdata);



        printf("    Sending ACK packet: %u\n\n \n", tcp->ackNumber);
    }
    // FIN ACK packet
    else if(tcp->flags == 0x11){
        printf("    FIN ACK packet\n");

        //now an ACK packet
        tcp->flags = 0x10;
        uint32_t number = tcp->ackNumber;
        tcp->ackNumber = tcp->seqNumber;
        tcp->ackNumber +=htonl(1);
        tcp->seqNumber = (number);
        tcp->checksum = compute_tcp_checksum(ip, tcp, payload+sizeof(*ip) + 20, 0);
        payload_len = 40;
        printf("    Sending ACK packet: %u\n\n\n", tcp->ackNumber);

    }
    // printf("checksum: %04X\n", tcp->checksum);


    // reset ip addresses
    uint32_t tempaddress = ip->dstAddress;
    ip->dstAddress = ip->srcAddress;
    ip->srcAddress = tempaddress;
    ip->length = htons(payload_len);
    ip->headchksum = 0;
    ip->headchksum = compute_headchksum(ip);

    uint8_t *tempeth = arp_lookup(ip->dstAddress);
    if(tempeth == NULL){
        printf("    There is no arp entry\n");
        return -1;
    }
    r = lookup_route(ip->srcAddress);
    if(r == NULL){
        printf("    could not look up return route\n");
        return -1;
    }


    memcpy(eth->dst_addr, tempeth, 6);
    memcpy(eth->src_addr, r->iface->eth_addr, 6);
    eth->type = little_ETH_TYPE_IP;
    size_t frame_len = compose_ethernet_frame(frame, eth, payload, payload_len);
    // printf("frame: %s\n", binary_to_hex(frame,frame_len));


    send_ethernet_frame(r->iface->out_fd, frame, frame_len);

    return 1;
}

uint16_t
compute_tcp_checksum(struct IPheader * ip, struct tcpheader * tcp, uint8_t * payload, size_t payload_len){
     // ones complement of sum of ones complement of each octet
    
    
    
    size_t tcpsize = 20;
    uint32_t sum = 0;
    uint16_t checksum;
    size_t size = tcpsize+payload_len;
    uint8_t zeros = 0;
    uint16_t totalsize = size+ 12;
    uint8_t tempframe [size+14];
    uint16_t tsize= htons(size);
    
    uint8_t pseudoframe [12];
    memcpy(pseudoframe, &ip->srcAddress, 4);
    memcpy(pseudoframe+4, &ip->dstAddress, 4);
    memcpy(pseudoframe+8, &zeros, 1);
    memcpy(pseudoframe+9, &ip->protocol, 1);
    memcpy(pseudoframe+10, &tsize, 2);
    memcpy(tempframe, pseudoframe, 12);
    memcpy(tempframe+12, (void *)tcp, tcpsize);
    memcpy(tempframe+12+ tcpsize, (void *)(payload), payload_len);

    
    //pad frame with 00s 
    if(size%2 ==1){
        memcpy(tempframe+size, &zeros, 1);
        totalsize++;
    }

    
    // printf("totalsize: %d\n", totalsize);
    // printf("data_len: %d\n", data_len);

    uint16_t *s;
    s = (uint16_t *)tempframe;

    // printf("%d\n ", data_len);
    // printf("pseudoframe: %s\n", binary_to_hex(pseudoframe, 13));
    // printf("tempframe: %s\n", binary_to_hex(tempframe , totalsize+1));
    while(totalsize > 1) {
        sum+=(uint16_t) *s;
        totalsize -=2;
        s++;
    }
    
    while(sum >> 16){
        sum = (sum &0xffff) + (sum>>16);
        // printf("sum: %04X\n", ~sum);
    }
    checksum = ~sum & 0xffff;
    // printf("checksum %04X\n", checksum);
    return checksum;
}

uint8_t *
lookup_tcp(uint32_t ip){
    for(int i = 0; i < maxtcptablesize; i++){
            // printf("arbtbl[%d]: %08X    \n", i, arpTbl[i].ip);
            if(tcpTbl[i].ipaddr == ip){
                // printf("found arp for %08X\n", ntohl(ip));
                return tcpTbl[i].eth;
            }
        }
        printf("    could not find tcp for %08X\n", (ip));
        return NULL;
}
int add_tcp(uint8_t *eth_addr, uint32_t ip_addr){
    // find empty arp entry to insert
    for (int i = 0; i<maxtcptablesize; i++){
        if(tcpTbl[i].eth == NULL){
            tcpTbl[i].eth = malloc(6);
            memcpy(tcpTbl[i].eth, eth_addr, 6);
            tcpTbl[i].ipaddr = (ip_addr);
            return 0;
        }
    }
    return -1;
}