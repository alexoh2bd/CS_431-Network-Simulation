/*
 * receiver.c
 */

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>

#define RECV_BUF_SIZE (1 * 1024 * 1024)
#define PAUSE_BUF_SIZE 16

int main(int argc, char *argv[])
{
    char *dest_hostname, *dest_port;
    struct addrinfo hints, *res;
    int conn_fd;
    char buf[RECV_BUF_SIZE];
    char pause_buf[PAUSE_BUF_SIZE];
    int pause_bytes_read;
    int bytes_to_recv;
    int bytes_received;
    int rc;

    dest_hostname = argv[1];
    dest_port     = argv[2];

    /* create a socket */
    conn_fd = socket(PF_INET, SOCK_STREAM, 0);

    /* client usually doesn't bind, which lets kernel pick a port number */

    /* but we do need to find the IP address of the server */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if((rc = getaddrinfo(dest_hostname, dest_port, &hints, &res)) != 0) {
        printf("getaddrinfo failed: %s\n", gai_strerror(rc));
        exit(1);
    }

    /* connect to the server */
    if(connect(conn_fd, res->ai_addr, res->ai_addrlen) < 0) {
        perror("connect");
        exit(2);
    }

    printf("Connected\n");

    /* infinite loop of reading from terminal, sending the data, and printing
     * what we get back */
    while(1) {
        write(1, "Number of bytes to recv: ", 25);
        pause_bytes_read = read(0, pause_buf, PAUSE_BUF_SIZE);
        pause_buf[pause_bytes_read-1] = '\0';
        bytes_to_recv = atoi(pause_buf);
        if(bytes_to_recv > RECV_BUF_SIZE) {
            bytes_to_recv = RECV_BUF_SIZE;
        }

        if((bytes_received = recv(conn_fd, buf, bytes_to_recv, 0)) < 0) {
            perror("recv");
            /* don't bail here so we can see what happens when we keep going */
        }

        printf("received %d bytes\n", bytes_received);
        puts(buf);
    }

    close(conn_fd);
}
