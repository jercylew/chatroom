/* 
 Chatroom client using fork
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
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/select.h>
#include <string.h>

#define BUF_SIZE 1024

void str_cli(char *name, FILE *fp, int sockfd)
{
    
   int len= BUF_SIZE;
   char	sendline[BUF_SIZE], recvline[BUF_SIZE];
   char sendline2[BUF_SIZE];
   pid_t pid;
   pid = fork();

   if(pid == 0){
     for(;;){
        len = recv(sockfd, recvline, len,0);
        if(len == 0)
        {
            printf("Server disconnected.\n"); 

            //TOTO: notify parent process
            exit(0); 
        }
            
        fputs(recvline, stdout);
     }
   }
   else { 
     for(;;) {
        strcpy(sendline2, name);
        if (fgets(sendline, BUF_SIZE, fp) == NULL) { /* Read a line */
            shutdown(sockfd, SHUT_WR);	/* send FIN */
            exit(0);
        }      
        strcat(sendline2, sendline);
        
        if(send(sockfd, sendline2, len, 0) <= 0) {
            perror("send");
        }
     }
   }
}

int main(int argc, char **argv)
{
   int sockfd;
   struct sockaddr_in	servaddr;
   char buffer[BUF_SIZE];
   char name[BUF_SIZE];
   const char *enter = " enter the chatroom ";
   int len;

   if (argc != 4){
       printf("Usage: chatcli name serverip serverpot\n");
       exit(0);
   }

   strcpy(name, argv[1]);
   strcat(name, " said:");

   /* Create a client socket  */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   bzero(&servaddr, sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_port = htons(atoi(argv[3]));
   inet_pton(AF_INET, argv[2], &servaddr.sin_addr);

   /* Connect to the server */
   if(connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) == -1){ 
      perror("connect");
      exit(1);
   }

   len = BUF_SIZE;
   snprintf(buffer,len,"%s%s\n",argv[1],enter);
   if(send(sockfd,buffer,len,0) == -1){
      perror("send");
      exit(1);
   }

   str_cli(name, stdin, sockfd);

   exit(0);
}
