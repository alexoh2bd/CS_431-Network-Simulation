/*
 * sender.c
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#include "util.h"
#include "cs431vde.h"
#include "crc32.h"


const char srcIPaddress[] = "\x08\x10\x20\x40";
const char dstIPaddress[] = "\x12\x34\x56\x78";


int
main(int argc, char *argv[])
{
    int fds[2];

    // uint8_t frame[1600];
    // ssize_t frame_len;

    int connect_to_remote_switch = 0;
    char *local_vde_cmd[] = { "vde_plug", "/tmp/net1.vde", NULL };
    char *remote_vde_cmd[] = { "ssh", "pjohnson@weathertop.cs.middlebury.edu",
                                      "/home/pjohnson/cs431/bin/vde_plug",
                                      NULL };
    char **vde_cmd = connect_to_remote_switch ? remote_vde_cmd : local_vde_cmd;

    if(connect_to_vde_switch(fds, vde_cmd) < 0) {
        printf("Could not connect to switch, exiting.\n");
        exit(1);
    }
   

    // includes Eth header
    char frame[] = {    
                        // arp
                        // "\xff\xff\xff\xff\xff\xff"      //  DEST
                        // "\x77\x88\x99\xdd\xee\xff"      //  SOURCE
                        // "\x08\x06"                      //  ARP TYPE
                        // "\x00\x01\x08\x00"              // 
                        // "\x06\x04\x00\x01"
                        // "\x77\x88\x99\xdd"              //  SOURCE MAC
                        // "\xee\xff\x08\x10"              //  SOURCE MAC   |   SOURCE IP
                        // "\x20\x40\x00\x00"              //  SOURCE IP    |   0's for dest mac
                        // "\x00\x00\x00\x00"              //  Target 0's for dest MAC
                        // "\x35\x35\xaf\xcd"              //  Target IP whose MAC we want
                        // "\x00\x00\x00\x00"              // 0's for
                        // "\x00\x00\x00\x00"
                        // "\x00\x00\x00\x00"
                        // "\x00\x00\x00\x00"
                        // "\x02\xc1\x70\xb1"


                        // no route
                        // "\x11\x22\x33\xaa\xbb\xcc"  // |    Dest ETH address        |
                        // "\x45\x45\x45\x45\x45\x15"  // |    Source eth address      |
                        // "\x08\x00"                  // |    IP Type        |
                        // "\x45\x14\x00\x94"          // | Version | IHL | Type of Service | Total Length    |
                        // "\x00\x00\x00\x00"          // |       ID     | Flags     |      Fragment offset   |
                        // "\x10\x01\x8c\xf3"          // | Time to Live  | Protocol  | Header Checksum       |
                        // "\x08\x10\x20\x40"          // |     Source Address          | 
                        // "\x45\x45\xaf\xcd"          // |     Destination Address     |
                        // "\x00\x11\x40\x15\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff"
                        // "\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff"
                        // "\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff"
                        // "\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff"
                        // "\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff"
                        // "\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff"
                        // "\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff"
                        // "\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff"    
                        // "\x35\x7f\xea\xb5" //           |     Frame Check Sequence   |

                        // checking ttl
                        "\x11\x22\x33\xaa\xbb\xcc"  // |    Dest ETH address        |
                        "\x77\x88\x99\xdd\xee\xff"  // |    Source eth address      |
                        "\x08\x00"                  // |    Type        |
                        "\x45\x00\x00\x94"          // | Version | IHL | Type of Service | Total Length    |
                        "\x00\x00\x00\x00"          // |       ID     | Flags     |      Fragment offset   |
                        "\x10\x01\x9d\x17"          // | Time to Live  | Protocol  | Header Checksum       |
                        "\x08\x10\x20\x40"          // |     Source Address          | 
                        "\x35\x35\xaf\xcd"          // |     Destination Address     |
                        "\x08\x00\x32\x5d\xaf\x04\x00\x00"
                        "\x01\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xf1"
                        "\x02\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xf2"
                        "\x03\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xf3"
                        "\x04\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xf4"
                        "\x05\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xf5"
                        "\x06\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xf6"
                        "\x07\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xf7"
                        "\x08\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xf8"    
                        "\x43\xaf\xf0\x83" 



// "\x11\x22\x33\xaa\xbb\xcc"  // |    Dest ETH address        |
//                         "\x77\x88\x99\xdd\xee\xff"  // |    Source eth address      |
//                         "\x08\x00" 
//                         "\x45\x00\x00\x94"          // | Version | IHL | Type of Service | Total Length    |
//                         "\x00\x00\x00\x00"          // |       ID     | Flags     |      Fragment offset   |
//                         "\x01\x01\xac\x17"          // | Time to Live  | Protocol  | Header Checksum       |
//                         "\x08\x10\x20\x40"          // |     Source Address          | 
//                         "\x35\x35\xaf\xcd" 
// "\x08\x00\x40\x4e\xaf\x04\x00\x00\x00\x00\x24\x93\x32\x47  
// \xc6\xcf\x08\x09\x0a\x0b\x0c\x0d\x0e\x0f\x10\x11\x12\x13\x14\x15
// \x16\x17\x1819 1a 1b 1c 1d 1e 1f 20 21 22 23 24 25  
// 26 27 28 29 2a 2b 2c 2d 2e 2f 30 31 32 33 34 35  
//  36 37"                                     


    };
    int frame_len;
    char * data_as_hex;

    send_ethernet_frame(fds[1], frame, 174);

    while((frame_len = receive_ethernet_frame(fds[0], frame)) > 0) {
        data_as_hex = binary_to_hex(frame, frame_len+4);
        printf("received frame, length %d:\n", frame_len);
        puts(data_as_hex);
        free(data_as_hex);
    }

    /* If the program exits immediately after sending its frames, there is a
     * possibility the frames won't actually be delivered.  If, for example,
     * the "remote_vde_cmd" above is used, the user might not even finish
     * typing their password (which is accepted by a child process) before
     * this process terminates, which would result in send frames not actually
     * arriving.  Therefore, we pause and let the user manually end this
     * process. */

    printf("Press Control-C to terminate sender.\n");
    pause();

    return 0;
}
