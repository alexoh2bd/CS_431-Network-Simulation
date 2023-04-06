#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

#include "crc32.h"
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
    char fcs[18];
    uint32_t checkfcs;
    char *hexcheckfcs;


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

        puts(data_as_hex);
        printf("frame_len = %d\n", frame_len);
        // src address
        strncpy(src, (char *)(data_as_hex+(sizeof(char)*18)), 17);
        printf("src: %s\n", src);
        
        // dest address
        strncpy(dest, data_as_hex, 17);
        printf("dest: %s\n", dest);
        printf("ethernet: %s\n", ethaddress);
        int len = round(strlen(data_as_hex));
        printf("len: %d\n", len);

        //  fcs bytes (4 bytes long)
        strncpy(fcs, (char *)((data_as_hex + len - 13)), 12);
        printf("fcs: %s\n", fcs);
        
        char *data = malloc(len+1);
        strncpy(data, (char *)(frame), (frame_len - sizeof(char)*14));
        printf("data as hex: %X\n", data);
        printf("data: %s\n", data);


        checkfcs = crc32(0, (void *) data, len-14);
        
        printf("checkfcs: %u\n", checkfcs);
        hexcheckfcs = malloc(11);
        sprintf(hexcheckfcs, "%X", checkfcs);

        //hexcheckfcs = binary_to_hex(&checkfcs, 11);
        printf("hex checkfcs: %s\n", hexcheckfcs);
        free(hexcheckfcs);
    
        // length is too small
        if(len<64){
            printf("ignoring %zd-byte frame (too short)\n", frame_len);
        }
        /*  else if(fcs != checkfcs){
            printf("ignoring %d-byte frame (bad fcs: got %s expected %s)" , len, fcs, checkfcs);
        }*/

        // Broadcast frame
        else if((strcmp(dest, broadcast)) == 0){
            printf("received %zd-byte broadcast frame from %s\n", frame_len, src);
        }

        
        // destination address matches our ethernet address, is intended for us
        else if ((strcmp((char *)hex_to_binary(dest), (char *)hex_to_binary(ethaddress))) == 0){
           printf("received %zd-byte frame for me from %s\n", frame_len,src);
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
