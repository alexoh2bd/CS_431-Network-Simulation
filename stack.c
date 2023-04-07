#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "crc32.h"
#include "util.h"
#include "cs431vde.h"

const char ethaddress[] = "\x11\x22\x33\xaa\xbb\xcc";
const char broadcast[] = "\xff\xff\xff\xff\xff\xff";


int main(int argc, char *argv[])
{
    int fds[2];


    uint8_t frame[1600];
    ssize_t frame_len;
    char *data_as_hex;
    char dest[7];
    char src[7];
    // char *type;
    // char *data;
    uint8_t fcs[5];
    uint32_t checkfcs;
    uint8_t hexfcs[5];
    char *hexsrc;


    int connect_to_remote_switch = 0;
    char *local_vde_cmd[] = { "vde_plug", NULL };
    char *remote_vde_cmd[] = { "ssh", "pjohnson@weathertop.cs.middlebury.edu",
                                      "/home/pjohnson/cs431/bin/vde_plug",
                                      NULL };
    char **vde_cmd = connect_to_remote_switch ? remote_vde_cmd : local_vde_cmd;

    if(connect_to_vde_switch(fds, vde_cmd) < 0) {
        printf("Could not connect to switch, exiting.\n");
        exit(1);
    }




    while((frame_len = receive_ethernet_frame(fds[0], frame)) > 0) {
        data_as_hex = binary_to_hex(frame, frame_len + 2);

        // puts(data_as_hex);
        // src address (7-12 bytes)
        strncpy(src, (char *)(frame+(sizeof(char)*6)), 6);
        hexsrc = binary_to_hex(src, 7);
        
        // dest address (first 6 bytes)
        strncpy(dest, (char *)frame, 6);


        //  fcs bytes (4 bytes long)
        strncpy((char *)fcs, (char *)((frame + frame_len - 4)), 5);
        checkfcs = crc32(0, (void *) frame, frame_len - 4);
        memcpy(hexfcs, &checkfcs, sizeof(uint32_t));



        // length is too small
        if(frame_len<64){
            printf("ignoring %zd-byte frame (too short)\n", frame_len);
        }
        // check fcs
        else if(strcmp(binary_to_hex(hexfcs, 5), binary_to_hex(fcs, 5)) != 0){
            printf("ignoring %zd-byte frame (bad fcs: got %s expected %s)\n)" , frame_len, binary_to_hex((void *)fcs, 5), binary_to_hex((void *)hexfcs, 5));
        }
        // Broadcast frame
        else if((strcmp(dest, broadcast)) == 0){
            printf("received %zd-byte broadcast frame from %s\n", frame_len, hexsrc);
        }
        // destination address matches our ethernet address, is intended for us
        else if ((strcmp(dest, ethaddress)) == 0){
           printf("received %zd-byte frame for me from %s\n", frame_len, hexsrc);
        }
        // destination address does not match our ethernet address
        else{ 
            printf("ignoring %zd-byte frame (not for me)\n", frame_len);
        }


        free(data_as_hex);

    }

    if(frame_len < 0) {
        perror("read");
        exit(1);
    }

    return 0;

}
