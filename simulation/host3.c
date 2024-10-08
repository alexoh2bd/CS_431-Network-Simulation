/*
 *
 *host3.c
 *
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <stdlib.h>

#include "util.h"
#include "cs431vde.h"
#include "crc32.h"
#include "ethernet.h"

const char srcEth[] = "\x20\x20\x20\x20\x20\x20";
int
main(int argc, char *argv[])
{

    int fds[2];

    // uint8_t frame[1600];
    // ssize_t frame_len;
    int connect_to_remote_switch = 0;
    char *local_vde_cmd[] = { "vde_plug", "/tmp/net2.vde", NULL };
    char *remote_vde_cmd[] = { "ssh", "pjohnson@weathertop.cs.middlebury.edu",
                                      "/home/pjohnson/cs431/bin/vde_plug",
                                      NULL };
    char **vde_cmd = connect_to_remote_switch ? remote_vde_cmd : local_vde_cmd;

    
    if(connect_to_vde_switch(fds, vde_cmd) < 0) {
        printf("Could not connect to switch, exiting.\n");
        exit(1);
    }
   

    // includes Eth header
    int frame_len;
    char frame[] = {    
                        "\x11\x22\x33\xaa\xbb\xcc"  	// |    Dest ETH address        |
                        "\x20\x20\x20\x20\x20\x20"  	// |    Source eth address      |
                        "\x08\x00"                  	// |    Type        |
                        "\x45\x00\x00\x94"         	// | Version | IHL | Type of Service | Total Length    |
                        "\x00\x00\x00\x00"          	// |       ID     | Flags     |      Fragment offset   |
                        "\x10\x01\x9d\x17"          	// | Time to Live  | Protocol  | Header Checksum       |
                        "\x08\x10\x20\x40"          	// |     Source Address          | 
                        "\x35\x35\xaf\xcd"         	// |     Destination Address     |
                        "\x08\x00\x32\x5d\xaf\x04\x00\x00"
                        "\x01\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xf1"
                        "\x02\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xf2"
                        "\x03\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xf3"
                        "\x04\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xf4"
                        "\x05\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xf5"
                        "\x06\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xf6"
                        "\x07\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xf7"
                        "\x08\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xf8"    
                        "\x43\xaf\xf0\x83" 	 	// | 	Frame Check Sequence 	|


    };
    char * data_as_hex;
    struct eth_header  *eth;
    char * hexEth;
    while((frame_len = receive_ethernet_frame(fds[0], frame))>0) {
	eth = (struct eth_header *) frame;
        data_as_hex = binary_to_hex(frame, frame_len);
	hexEth = binary_to_hex(eth->dst_addr, 7);
	// puts( hexEth);
	if((memcmp(eth->dst_addr, srcEth, 6)) == 0){
             printf("received frame from Ethernet address: %s, length %d:\n",hexEth, frame_len);
             puts(data_as_hex);
	}
	else{
             printf("Rejected frame from Ethernet address: %s, wrong Eth address: %d\n", hexEth,frame_len);
	}
        free(data_as_hex);
    }

    printf("Press Control-C to terminate sender.\n");
    pause();

    return 0;
}
