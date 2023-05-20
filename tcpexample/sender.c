/*
 * sender.c
 */

#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>

#define BACKLOG 10
#define SEND_BUF_SIZE 1 * 1024 * 1024
#define PAUSE_BUF_SIZE 16

int main(int argc, char *argv[])
{
    char *listen_port;
    int listen_fd, conn_fd;
    struct addrinfo hints, *res;
    int rc;
    struct sockaddr_in remote_sa;
    uint16_t remote_port;
    socklen_t addrlen;
    char *remote_ip;
    char buf[SEND_BUF_SIZE];
    char pause_buf[PAUSE_BUF_SIZE];
    int input_bytes_read;
    int bytes_to_send;
    int yes;

    listen_port = argv[1];

    /* create a socket */
    listen_fd = socket(PF_INET, SOCK_STREAM, 0);

    /* bind it to a port */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if((rc = getaddrinfo(NULL, listen_port, &hints, &res)) != 0) {
        printf("getaddrinfo failed: %s\n", gai_strerror(rc));
        exit(1);
    }

    yes = 1;
    if(setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(socklen_t)) < 0) {
        perror("setsockopt");
        exit(1);
    }

    if(bind(listen_fd, res->ai_addr, res->ai_addrlen) < 0) {
        perror("bind");
        exit(1);
    }

    /* start listening */
    listen(listen_fd, BACKLOG);

    /* infinite loop of accepting new connections and handling them */
    while(1) {
        /* accept a new connection (will block until one appears) */
        addrlen = sizeof(remote_sa);
        conn_fd = accept(listen_fd, (struct sockaddr *) &remote_sa, &addrlen);

        /* announce our communication partner */
        remote_ip = inet_ntoa(remote_sa.sin_addr);
        remote_port = ntohs(remote_sa.sin_port);
        printf("new connection from %s:%d\n", remote_ip, remote_port);

        /* wait for the user to press enter, then call send(2), and loop */
        while(1) {
            write(1, "Number of bytes to send: ", 25);
            input_bytes_read = read(0, pause_buf, PAUSE_BUF_SIZE);
            pause_buf[input_bytes_read-1] = '\0';
            bytes_to_send = atoi(pause_buf);

            printf("sending %d bytes\n", bytes_to_send);
            if(send(conn_fd, buf, bytes_to_send, 0) < 0) {
                perror("send");
                /* don't bail here so we can see what happens when we keep going */
            }
        }
        printf("\n");

        close(conn_fd);
    }
}
