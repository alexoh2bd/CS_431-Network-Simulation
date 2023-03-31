/*
 * sender.c
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>

#include "util.h"
#include "cs431vde.h"

int 
main(int argc, char *argv[])
{
    int fds[2];

    uint8_t frame[1600];
    ssize_t frame_len;

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
    // memset(frame, '\xff', 64);
    char temp[] = "11 22 33 aa bb cc 77 88 99 dd ee ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff ff a1 a2 a3 a4 a5";
    strncpy((char *)frame, hex_to_binary(temp), 208);
    

    frame_len = 71;

    printf("sending frame, length %ld\n", frame_len);

    send_ethernet_frame(fds[1], frame, frame_len);

    // while(1){
    //     char *line;
    //     size_t linelen = 128;

    //     line = malloc(linelen * sizeof(char));
    //     if(line == NULL)
    //     {
    //         perror("Unable to allocate buffer");
    //         exit(1);
    //     }

    //     getline(&line, &linelen, stdin);

    //     strncpy(frame, hex_to_binary(line), frame_len);
        
    //     send_ethernet_frame(fds[1], frame, frame_len);
    //     free(line);
        
    // }






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
