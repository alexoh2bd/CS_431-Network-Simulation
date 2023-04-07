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

int 
main(int argc, char *argv[])
{
    int fds[2];

    // uint8_t frame[1600];
    // ssize_t frame_len;

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
    //memset(frame, '\xff', 2);
    char temp[] = {"\xab\xff\xff\xff\xff\xff\x77\x88\x99\xdd\xee\xff\xff\xff\xff\xff\xff\xbb\xbb\xff\xff\x33\xdd\xee\xff\xff\xff\xbb\xbb\xff\xff\x33\xdd\xee\xff\xff\xff\xbb\xbb\xff\xff\x33\xdd\xee\xff\xff\xff\xbb\xbb\xff\xff\x33\xdd\xee\xff\xff\xff\xbb\xbb\xff\xff\xff\xaa\xbb\xcc\x70\x83\x42\x3e\xee\xff\xff\xff\xff\xff\xff\xbb\xee\xff\xff\xff\xff\xff\xff\xbb\x83\x42\x3e\xee\xff\xff\xff\xff\xff\xff\xbb\xee\xff\xff\xff\xff\xff\xff\xbb\x83\x42\x3e\xee\xff\xff\xff\xff\xff\xff\xbb\xee\xff\xff\xff\xff\xff\xff\xbb\x83\x42\x3e\xee\xff\xff\xff\xff\xff\xff\xbb\xee\xff\xff\xff\xb1\x0a\xf6\x15"
    };


    send_ethernet_frame(fds[1], temp, strlen(temp));


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
