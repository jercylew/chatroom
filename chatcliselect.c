/* 
 Chatroom client using select
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
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/select.h>
#define BUF_SIZE 1024

void str_cli(char *name,FILE *fp, int sockfd)
{
    
   int maxfdp1, stdineof = 0;
   int len;
   fd_set rset;
   char	sendline[BUF_SIZE], recvline[BUF_SIZE];
   char sendline2[BUF_SIZE];
   FD_ZERO(&rset);
   char *ptr;
   for ( ; ; ) {
    
     FD_SET(sockfd, &rset);
     if (stdineof == 0)
       FD_SET(fileno(fp), &rset);

     if(fileno(fp) > sockfd)
       maxfdp1= fileno(fp)+1;
     else
       maxfdp1 = sockfd+1;
     
     select(maxfdp1, &rset, NULL, NULL, NULL);
     len = BUF_SIZE;
     if (FD_ISSET(sockfd, &rset)) {	/* socket is readable */
        len = recv(sockfd, recvline, len,0);
        if(len ==0)
        {
           if (stdineof == 1)
	      return;		/* normal termination */
           else{
              printf("str_cli: server terminated prematurely");
              exit(1);
           }			
        }   
        fputs(recvline, stdout);
     }
     if (stdineof == 0)
        FD_SET(fileno(fp), &rset);
 
     if (FD_ISSET(fileno(fp), &rset)) {  /* input is readable */
       strcpy(sendline2,name);
       len = BUF_SIZE;
       if (fgets(sendline, BUF_SIZE, fp) == NULL) {
         stdineof = 1;
         shutdown(sockfd, SHUT_WR);	/* send FIN */
         FD_CLR(fileno(fp), &rset);
         continue;
       }
          
       strcat(sendline2,sendline);
       send(sockfd, sendline2, len,0);
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
   strcpy(name,argv[1]);
   strcat(name," said:");   
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   bzero(&servaddr, sizeof(servaddr));
   servaddr.sin_family = AF_INET;
   servaddr.sin_port = htons(atoi(argv[3]));
   inet_pton(AF_INET, argv[2], &servaddr.sin_addr);
   if(connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr))==-1){ 
      perror("connect");
      exit(1);
   };
   len = BUF_SIZE;
   snprintf(buffer,len,"%s%s\n",argv[1],enter);
   if(send(sockfd,buffer,len,0)==-1){
      perror("send");
      exit(1);
   }
   printf("len is %d\n",len);
   str_cli(name,stdin, sockfd);
   exit(0);
}

