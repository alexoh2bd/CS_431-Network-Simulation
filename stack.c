#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <poll.h>

#include "crc32.h"
#include "util.h"
#include "cs431vde.h"
#include "stack.h"
#include "ethernet.h"
#include "ip.h"
#include "arp.h"

struct arp_entry{
    char ip [16];
    char mac [24];
};

int num_interfaces = 0;
struct arp_entry arp_cache[10];
int arpsize = 0;


struct interface interfaces[MAX_INTERFACES];


int add_interface(char *vde_control_file, uint8_t *eth_addr, uint32_t ip_addr, char * name);

int main(int argc, char *argv[])
{
    ip_init();
    arp_init();

    memset(interfaces, '\0', sizeof(interfaces));
    add_interface("/tmp/net1.vde", (uint8_t *) "\x11\x22\x33\xaa\xbb\xcc", 0x40201077, "interface1");
    add_interface("/tmp/net2.vde", (uint8_t *) "\x10\x10\x10\x10\x10\x10", 0xcdaf3522, "interface2");

   
    add_route(0x40201000, 0xffffff00, 0x00000000, &interfaces[0]);  // network 1: sender1s
    add_route(0xcdaf3500, 0xffffff00, 0x00000000, &interfaces[1]);  // network 2: sender2a

    add_route(0xcdaf4500, 0xffffff00, 0xcdaf4545, &interfaces[1]);  // network 2: gateway test


    // |  eth address | ip address |
    add_arp((uint8_t *) "\x77\x88\x99\xdd\xee\xff", 0x40201008);    // sender1a
    add_arp((uint8_t *) "\x45\x45\x45\x45\x45\x15", 0x40201020);    // sender1b

    add_arp((uint8_t *) "\x50\x50\x50\x50\x50\x10", 0xcdaf3535);    // sender2a
    add_arp((uint8_t *) "\x40\x40\x40\x40\x40\x40", 0xcdaf4545);    // sender2b

    //  ASSIGNMENT 3 POLLLLS
    struct pollfd fds[num_interfaces];
    for(int i = 0; i < num_interfaces; i++){
        fds[i].fd = interfaces[i].in_fd;
        fds[i].events = POLLIN;
    }

    while(1){
        if (poll(fds, num_interfaces, -1) < 0) {
            perror("ERROR in poll");
            exit(1);
        }

        for(int i = 0; i < num_interfaces; i++){
            if(fds[i].revents & POLLIN){
                // if there is a readable input, handle ethernet frame
                // printf("stack received ethernet frame on interface %d\n", i);
                handle_ethernet_frame(&interfaces[i]);
            }
        }
        
    }

    return 0;

}
int 
add_interface(char *vde_control_file, uint8_t *eth_addr, uint32_t ip_addr, char * name){
    int fds[2];
    struct interface *iface = NULL;
    char *vde_cmd[] = {"vde_plug", vde_control_file, NULL};
    char *  mac_as_string;

    // check for available interface in array
    for(int i = 0; i<MAX_INTERFACES; i++){
        if(interfaces[i].name == NULL){
            iface = &interfaces[i];
            break;
        }
    }
    if(iface == NULL){
        return -1;
    }

    if(connect_to_vde_switch(fds, vde_cmd) < 0) {
        printf("Could not connect to switch 1, exiting.\n");
        exit(1);
    }

    // set all interface values
    memcpy(iface->eth_addr, eth_addr, 6);
    iface -> ip_addr = ip_addr;
    iface -> name = strdup(name);
    iface -> in_fd = fds[0];
    iface -> out_fd = fds[1];
    
    mac_as_string = binary_to_hex(iface->eth_addr, 7);
    printf("Connected to interface %s to switch %s, IP is %u, MAC is %s\n", iface->name, vde_control_file, iface->ip_addr, mac_as_string);
    free(mac_as_string);
    num_interfaces ++;

    return 0;

}


