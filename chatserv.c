/* 
 Chatroom server using select
 Copyright (C) 2004-2006, Li Suke, School of Software and Microelectronics,
 Peking University
 This  is free software; you can redistribute it and/or
 modify it freely.

 This software  is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
*/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/select.h>
#include <string.h>
#include <errno.h>


#define  BACK_LOG 10
#define BUF_SIZE 1024

/* User info */
struct UserInfo
{
    char m_strName[64];
    int m_nFd;
};

int main(int argc, char *argv[]){
    if(argc != 2){
        printf("Usage chatsrv port\n");
        exit(0);
    }

    int sockfd;
    int connfd;
    int clifd;
    int clifd2;
    int clilen;
    int port;
    int client[FD_SETSIZE];
    int max_fd;
    int i;
    int nready;
    int max_index;
    char buffer[BUF_SIZE];
    int j;
    int len;
    struct sockaddr_in servaddr, cliaddr;

    port = atoi(argv[1]);
    bzero(&servaddr,sizeof(struct sockaddr_in));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);
    
    /* Get the server socket */
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0))==-1){
        perror("socket");
        exit(1);
    }

        /* Bind the socket to server address */
    if(bind(sockfd, (struct sockaddr *)&servaddr, sizeof(struct sockaddr))==-1){
        perror("bind");
        exit(1);
    }

    /* Start listening */
    if(listen(sockfd, BACK_LOG)==-1){
        perror("listen");
        exit(1);
    }

    fd_set allset,rdset;
    max_fd = sockfd;
    FD_ZERO(&allset);
    FD_ZERO(&rdset);
    FD_SET(sockfd, &allset);

    for(i=0; i< FD_SETSIZE; i++)
        client[i] = -1;

    max_index = -1;
    for( ; ; ) {
        rdset = allset;
        nready = select(max_fd + 1, &rdset, NULL, NULL, NULL);

        /* If there is a incoming connection from a client
         * TOTO: What if there are multiple requests at the same time?
         */
        if(FD_ISSET(sockfd, &rdset)) {
            clilen = sizeof(cliaddr);

            /* Accept a connection request from client */
            connfd = accept(sockfd, (struct sockaddr *)&cliaddr, &clilen);
            if(connfd == -1){
                perror("accept");
                continue;
            }

            for(i = 0; i < FD_SETSIZE; i++){
                if(client[i] == -1){
                    client[i] = connfd;
                    break;
                }
            }
        
            if(i == FD_SETSIZE) {
                printf("Too many client here!\n");
                exit(1);
            }

            if(i > max_index)
                max_index = i;

            if(connfd > max_fd)
                max_fd = connfd;

            /* Add this client fd to the monitor read list */
            FD_SET(connfd, &allset);

            if(--nready <= 0) /* No connection request */
                continue;        
        }

        for (i = 0; i <= max_index; i++){
            if((clifd = client[i]) < 0)
                continue;

            if(FD_ISSET(clifd, &rdset)) {
                len = BUF_SIZE;

                if((len = recv(clifd, buffer, len, 0)) <= 0) {
                    printf("%d exit the group\n", clifd);
                    close(clifd);
                    FD_CLR(clifd,&allset);
                    client[i] = -1;
                }
                else {
                    /* Now send to all the group members */
                    for(j = 0;j <= max_index;j++){
                        clifd2 = client[j];
                        if(clifd2 > 0 && clifd2 != sockfd)
                        send(clifd2, buffer, len, 0);
                    } //for
                    if(--nready <= 0) 
                        break;
                }//else
            }//if
        }//for   
   }//for 

}
