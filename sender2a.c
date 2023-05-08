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
    char *local_vde_cmd[] = { "vde_plug", "/tmp/net2.vde", NULL };
    char *remote_vde_cmd[] = { "ssh", "pjohnson@weathertop.cs.middlebury.edu",
                                      "/home/pjohnson/cs431/bin/vde_plug",
                                      NULL };
    char **vde_cmd = connect_to_remote_switch ? remote_vde_cmd : local_vde_cmd;

    if(connect_to_vde_switch(fds, vde_cmd) < 0) {
        printf("Could not connect to switch, exiting.\n");
        exit(1);
    }
    //memset(frame, '\xff', 2);
     // includes Eth header
    char frame[] = {    "\x10\x10\x10\x10\x10\x10"  // |        Destination ETH Address          |
                        "\x50\x50\x50\x50\x50\x10"  // |        Source ETH Address               |
                        "\x08\x00"
                        "\x45\x14\x00\x90" // | Version | IHL | Type of Service | Total Length    |
                        "\xff\xff\xff\xff" // |       ID     | Flags     |      Fragment offset   |
                        "\x10\x02\x9d\x06" // | Time to Live  | Protocol  | Header Checksum       |
                        "\x35\x35\xaf\xcd" // |     Source Address          | 
                        "\x08\x10\x20\x40" // |     Destination Address     |
                        "\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff"
                        "\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff"
                        "\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff"
                        "\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff"
                        "\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff"
                        "\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff"
                        "\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb\xcc\xdd\xee\xff"
                        "\x00\x11\x22\x33\x44\x55\x66\x77\x88\x99\xaa\xbb"
                        "\x09\xdf\x58\xe0"         
    };

    send_ethernet_frame(fds[1], frame, 162);
    int frame_len;
    char * data_as_hex;
    while((frame_len = receive_ethernet_frame(fds[0], frame)) > 0) {
        data_as_hex = binary_to_hex(frame, frame_len);
        printf("received frame, length %d:\n", frame_len);
        puts(data_as_hex);
        free(data_as_hex);
    }

    /* If the program exits immediately after sending its frames, there is a
     * possibility  q the frames won't actually be delivered.  If, for example,
     * the "remote_vde_cmd" above is used, the user might not even finish
     * typing their password (which is accepted by a child process) before
     * this process terminates, which would result in send frames not actually
     * arriving.  Therefore, we pause and let the user manually end this
     * process. */

    printf("Press Control-C to terminate sender.\n");
    pause();

    return 0;
}
