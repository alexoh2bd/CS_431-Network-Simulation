#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "crc32.h"
#include "util.h"
#include "cs431vde.h"

const char routingTbl[3][3] = {
"\x12\x34",
"\x08\x10",
"\x18\x26"
};

struct arp_entry{
    char ip [16];
    char mac [24];
};

struct arp_entry arp_cache[10];
int arpsize = 0;

const char ethaddress[] = "\x11\x22\x33\xaa\xbb\xcc";
const char broadcast[] = "\xff\xff\xff\xff\xff\xff";

struct IPheader{
    ssize_t ihl;
    char dstAddress[5];
    char srcAddress[5];
    uint8_t ttl;
    uint16_t headchksum;
    uint16_t length;
};

int main(int argc, char *argv[])
{
    int fds1[2];
    // int fds2[2];
    // int fds3[2];


    uint8_t frame[1600];
    ssize_t frame_len;
    char dest[7];
    char src[7];

    uint8_t fcs[5];
    uint32_t checkfcs;
    uint8_t hexfcs[5];
    char *hexsrc;


    int connect_to_remote_switch = 0;
    char *net1_vde_cmd[] = { "vde_plug", "/tmp/net1.vde", NULL };
    // char *net2_vde_cmd[] = { "vde_plug", "/tmp/net2.vde", NULL };
    // char *net3_vde_cmd[] = { "vde_plug", "/tmp/net3.vde", NULL };
    // char *net4_vde_cmd[] = { "vde_plug", "/tmp/net4.vde", NULL };

    char *remote_vde_cmd[] = { "ssh", "pjohnson@weathertop.cs.middlebury.edu",
                                      "/home/pjohnson/cs431/bin/vde_plug",
                                      NULL };

    char **vde_cmd1 = connect_to_remote_switch ? remote_vde_cmd : net1_vde_cmd;
    // char **vde_cmd2 = connect_to_remote_switch ? remote_vde_cmd : net2_vde_cmd;
    // char **vde_cmd3 = connect_to_remote_switch ? remote_vde_cmd : net3_vde_cmd;
    // char **vde_cmd4 = connect_to_remote_switch ? remote_vde_cmd : net4_vde_cmd;


    if(connect_to_vde_switch(fds1, vde_cmd1) < 0) {
        printf("Could not connect to switch 1, exiting.\n");
        exit(1);
    }
    // if(connect_to_vde_switch(fds2, vde_cmd2) < 0) {
    //     printf("Could not connect to switch 2, exiting.\n");
    //     exit(1);
    // }
    // if(connect_to_vde_switch(fds3, vde_cmd3) < 0) {
    //     printf("Could not connect to switch 3, exiting.\n");
    //     exit(1);
    // }
    // if(connect_to_vde_switch(fds, vde_cmd4) < 0) {
    //     printf("Could not connect to switch 4, exiting.\n");
    //     exit(1);
    // }



    while((frame_len = receive_ethernet_frame(fds1[0], frame)) > 0) {
        struct IPheader ip;
        // printf("frame: %s\n", binary_to_hex((char *)frame, frame_len));

        // IP HEADER BREAKDOWN
        // find length of ip header
        char *s = (binary_to_hex(frame,3));
        ip.ihl =  4 * atoi(&s[1]);

        



        //dest and source addresses
        strncpy(ip.srcAddress, &frame[12], 4);
        strncpy(ip.dstAddress, &frame[16], 4);
        char *hexsrcaddress = binary_to_hex(ip.srcAddress, 5);
        char *hexdestaddress = binary_to_hex(ip.dstAddress, 5);
        // printf("dest address: %s\n", binary_to_hex((void *)ip.dstAddress, 5));
        // printf("src address: %s\n", binary_to_hex((void *)ip.srcAddress, 5));
        
        // Find Destination Address in Routing table
        char tempdest[3];
        strncpy(tempdest, ip.dstAddress, 2);
        
        uint8_t foundDest = 0;
        for(char **p = routingTbl; *p !=NULL; p++){
 
            // find a matching route
            if(strcmp(p, tempdest)==0){
                printf("foundmatch\n");
                foundDest = 1;
                break;
            }
        }

        // Find IP entry in cache
        uint8_t inArp = 0;
        for(int i =0; i < arpsize; i++){
            if(strcmp(ip.srcAddress,arp_cache[i].ip) ==0){
                inArp =1;
                printf("found ip entry in cache\n");
                break;
            }
        }




        // manually add an arp entry
        strcpy(arp_cache[0].ip, ip.srcAddress);
        strcpy(arp_cache[0].mac, "\x10\x0\x03\x01\x20\x30");
        arpsize++;

        // calculate header checksum
        ip.headchksum = frame[10] * 256 + frame[11];

        // ones complement of sum of ones complement of each octet
        uint8_t complement;
        uint16_t sum = 0;
        for(int i = 0; i< ip.ihl; i++){
            printf("~frame%d : %u\n", i,  ~frame[i]);
            if(i !=10 && i!=11){
            complement = ~frame[i];
            sum += complement;
            }
        }
        sum = ~sum;
        
        



        // create separate ethernet frame
        ssize_t eframe_len = frame_len - ip.ihl;
        char *eframe = malloc (eframe_len);
        strncpy(eframe, &frame[ip.ihl], eframe_len);

        // ETHERNET FRAME BREAKDOWN

        // puts(data_as_hex);
        // src address (7-12 bytes)
        strncpy(src, (char *)(eframe+(sizeof(char)*6)), 6);
        hexsrc = binary_to_hex(src, 7);
        
        // dest address (first 6 bytes)
        strncpy(dest, (char *)eframe, 6);

        //  fcs bytes (4 bytes long)
        strncpy((char *)fcs, (char *)((eframe + eframe_len - 4)), sizeof(char) * 5);
        checkfcs = crc32(0, (void *) eframe, eframe_len - 4);
        memcpy(hexfcs, &checkfcs, sizeof(uint32_t));




        // time to live
        ip.ttl = frame[8];
        ip.ttl --;
        frame[8] = ip.ttl;
        printf("ttl: %u\n", ip.ttl);

        
        // packet length (whole packet)
        ip.length = frame[2]*256 + frame[3];
        

        // Time to live = 0
        if(ip.ttl == 0){
            printf("Dropping packet from %s to %s (TTL exceeded)\n", hexsrcaddress, hexdestaddress);
        }

        // Length of IP packet does not match
        else if(ip.length != frame_len){
            printf("Dropping packet from %s (wrong length)\n", hexsrcaddress);
            // printf("listed packet length: %u\n", ip.length);
            // printf("expected packet length: %u\n", frame_len);
        }

        // check header checksum
        else if(ip.headchksum != sum){
            printf("Dropping packet from %s (bad IP header checksum)\n", hexsrcaddress);
        }

        // No stored route address
        else if(foundDest == 0){
            printf("Dropping packet from %s to %s (no route)\n", hexsrcaddress, hexdestaddress);
        }
    
        // check for arp entry in cache
        else if(inArp ==0){
                printf("Dropping packet from %s to %s (no ARP)\n", hexsrcaddress, hexdestaddress);
        }
        


        // ETHERNET CHECKS
        // length is too small
        else if(eframe_len<64){
            printf("ignoring %zd-byte frame (too short)\n", eframe_len);
        }

        // check fcs
        else if(strcmp(binary_to_hex(hexfcs, 5), binary_to_hex(fcs, 5)) != 0){
            printf("ignoring %zd-byte frame (bad fcs: got %s expected %s)\n)" , eframe_len, binary_to_hex((void *)fcs, 5), binary_to_hex((void *)hexfcs, 5));
        }

        // Broadcast frame
        else if((strcmp(dest, broadcast)) == 0){
            printf("received %zd-byte broadcast frame from %s\n", eframe_len, hexsrc);
        }

        // destination address matches our ethernet address, is intended for us
        else if ((strcmp(dest, ethaddress)) == 0){
           printf("received %zd-byte frame for me from %s\n", eframe_len, hexsrc);
        }

        // destination address does not match our ethernet address
        else{ 
            printf("ignoring %zd-byte frame (not for me)\n", eframe_len);
        }



    }

    if(frame_len < 0) {
        perror("read");
        exit(1);
    }

    return 0;

}
