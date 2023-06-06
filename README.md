Network-Interface

This repository contains the majority of my work from the class CS_431: Computer Networks. The goal was to simulate the behavior of a network router and the protocols by which they operate. 

I utilized UTM to run a FreeBSD virtual environment in which I tested all of my code. The command vde_switch creates a virtual switch which the program connects to in order to simulate interface behavior.  Stack.c is the program in which I implement a network interface that currently receives ethernet frames and checks them for destination and source addresses, valid frame check sequence, and frame size.  Sender.c is used to send frames to Stack.c. I utilized wireshark to see the packages being sent and debug.

I extended the interface to implement Ethernet, IP, ARP, ICMP, and TCP protocols. The router may receive multiple packets from multiple sources and routing them to multiple destinations. This includes writing and sending ARP and ICMP responses.
