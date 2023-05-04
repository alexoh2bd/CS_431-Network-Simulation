#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <arpa/inet.h>

#include "crc32.h"
#include "util.h"
#include "cs431vde.h"
#include "stack.h"
#include "ethernet.h"
#include "ip.h"



const char broadcast[] = "\xff\xff\xff\xff\xff\xff";



int 
handle_ethernet_frame(struct interface *iface){
     

    uint8_t frame[1600];
    ssize_t frame_len;
    struct eth_header *eh;
    uint8_t fcs[5];
    uint32_t checkfcs;
    uint8_t hexfcs[5];
    char *hexsrc;
    uint8_t *payload;
    int payload_len;
    int isbroadcast = 0;

    
    // receive eth_frame and divide into eh frame struct
    frame_len = receive_ethernet_frame(iface->in_fd, frame);
    eh = (struct eth_header *) frame;
    hexsrc = binary_to_hex(eh->src_addr, 7);
    printf("received %ld byte frame on interface %s\n", frame_len, iface->name);


    // ETHERNET FRAME BREAKDOWN

    

    //  fcs bytes (4 bytes long)
    strncpy((char *)fcs, (char *)((frame + frame_len - 4)), sizeof(char) * 5);
    checkfcs = crc32(0, (void *) frame, frame_len - 4);
    memcpy(hexfcs, &checkfcs, sizeof(uint32_t));

    if(frame_len<ETH_MIN_DATA_LEN){
        printf("  ignoring %zd-byte frame (too short)\n", frame_len);
        return -1;
    }

    // check fcs
    else if(memcmp(hexfcs,fcs, 4) != 0){
        printf("  ignoring %zd-byte frame (bad fcs: got %s expected %s)\n" , frame_len, binary_to_hex((void *)fcs, 5), binary_to_hex((void *)hexfcs, 5));
        return -1;
    }

    // Broadcast frame
    else if((memcmp(eh->dst_addr, broadcast, 6)) == 0){
        isbroadcast = 1;
        printf("  received %zd-byte broadcast frame from %s\n", frame_len, hexsrc);
    }

    free(hexsrc);

    
    payload = frame + sizeof(struct eth_header);
    payload_len = frame_len - sizeof(struct eth_header);
    // determines if address if for 
    printf("eh->dst_addr: %s\n", binary_to_hex(eh->dst_addr, 7));
    if(memcmp(eh->dst_addr, iface->eth_addr, 6)==0 || isbroadcast == 1){
        char *hexmac = binary_to_hex(eh->src_addr, 7);
        printf("  frame is for me, from %s\n", hexmac);
        free(hexmac);

        // check type of address
        switch (ntohs(eh->type)){
            // handle IP packet with payload
            case ETH_TYPE_IP:    printf("  has type ip\n"); 
                // divide frame into payload to pass to ip
                handle_ip_packet(iface, payload, payload_len);
                break;

            case ETH_TYPE_ARP:  printf("  has type arp\n");
                handle_arp_packet(iface, payload, payload_len);
                break;

            default: printf("  dropping frame, unknown type\n");
                break;
        }

    }

    if(frame_len < 0) {
        perror("read");
        exit(1);
    }
    return 1;
}

int
compose_ethernet_frame(uint8_t *frame, struct eth_header *eh, uint8_t *data, size_t data_len){
    uint32_t fcs;


    if(data_len >ETH_MAX_DATA_LEN ){
        return -1;
    }


    // enter eth header and data into frame
    memcpy(frame, eh, sizeof(*eh));     // 8
    memcpy(frame + sizeof(*eh), data, data_len); // 


    // fill in the rest of the frame if too little data
    if(data_len <ETH_MIN_DATA_LEN){
        memset(frame + sizeof(eh) + data_len, '\0', ETH_MIN_DATA_LEN - data_len);
        data_len = ETH_MIN_DATA_LEN;
    }

    // add fcs
    fcs = crc32(0, frame, sizeof(struct eth_header) + data_len);
    memcpy(frame+ sizeof(*eh) + data_len, &fcs, ETH_FCS_LEN);

    // return total length of frame
    return sizeof(*eh) + data_len + ETH_FCS_LEN;

}
