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
                        "\x45\x00\x00\xb9" // | Version | IHL | Type of Service | Total Length    |
                        "\xff\xff\xff\xff" // |       ID     | Flags     |      Fragment offset   |
                        "\x02\x02\xf4\x9b" // | Time to Live  | Protocol  | Header Checksum       |
                        "\x08\x10\x20\x40" // |     Source Address          | 
                        "\x12\x34\x56\x78" // |     Destination Address     |
                        "\x11\x22\x33\xaa\xbb\xcc\x77\x88\x99\xdd\xee\xff\xff\xff\xff"
                        "\xff\xff\xbb\xbb\xff\xff\x33\xdd\xee\xff\xff\xff\xbb\xbb\xff"
                        "\xff\x33\xdd\xee\xff\xff\xff\xbb\xbb\xff\xff\x33\xdd\xee\xff"
                        "\xff\xff\xbb\xbb\xff\xff\x33\xdd\xee\xff\xff\xff\xbb\xbb\xff"
                        "\xff\xff\xaa\xbb\xcc\x70\x83\x42\x3e\xee\xff\xff\xff\xff\xff"
                        "\xff\xbb\xee\xff\xff\xff\xff\xff\xff\xbb\x83\x42\x3e\xee\xff"
                        "\xff\xff\xff\xff\xff\xbb\xee\xff\xff\xff\xff\xff\xff\xbb\x83"
                        "\xff\xff\xbb\x83\x42\x3e\xee\xff\xff\xff\xff\xff\xff\xbb\xee"
                        "\xff\xff\xbb\x83\x42\x3e\xee\xff\xff\xff\xff\xff\xff\xbb\xee"
                        "\xff\xff\xbb\x83\x42\x3e\xee\xff\xff\xff\xff\xff\xff\xbb\xee"
                        "\xff\xff\xbb\x83\x42\x3e\xee\xff\xff\xff\xff\x30\xea\xe2\x0d"
    };

    send_ethernet_frame(fds[1], frame, 185);

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
