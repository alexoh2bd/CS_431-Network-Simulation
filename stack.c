#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>

#include "util.h"
#include "cs431vde.h"

const char ethaddress[] = "11 22 33 aa bb cc";
const char broadcast[] = "ff ff ff ff ff ff";


int main(int argc, char *argv[])
{
    int fds[2];


    uint8_t frame[1600];
    ssize_t frame_len;
    char *data_as_hex;
    char dest[18];
    char src[18];
    // char *type;
    // char *data;
    // char *fcs;


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
        data_as_hex = binary_to_hex(frame, frame_len);

        // src address
        strncpy(src, (char *)(data_as_hex+(sizeof(char)*18)), 17);
        printf("src: %s\n", src);
        
        // dest address
        strncpy(dest, data_as_hex, 17);
        printf("dest: %s\n", dest);
        printf("ethernet: %s\n", ethaddress);
        int len = strlen(data_as_hex)/3 + 1;

        // length is too small
        if(len<64){
            printf("ignoring %d-byte frame (too short)\n", len);
        }
        // Broadcast frame
        else if((strcmp(dest, broadcast)) == 0){
            printf("received %d-byte broadcast frame from %s\n", len, src);
        }
        // destination address does not match our ethernet address
        else if ((strcmp(dest, ethaddress)) != 0){
           printf("ignoring %d-byte frame (not for me)\n", len);
        }
        
        
        // destination address matches our ethernet address, is intended for us
        else{ 
        printf("received %d-byte frame for me from %s\n", len,src);
        }

        puts(data_as_hex);

        free(data_as_hex);
    }

    if(frame_len < 0) {
        perror("read");
        exit(1);
    }

    return 0;
}
