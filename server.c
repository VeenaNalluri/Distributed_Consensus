//
// Created by veena1 on 4/16/18.
//

#include <stdio.h>

#include <stdlib.h>

#include <string.h>
#include <strings.h>

#include <pthread.h>

#include <sys/socket.h>

#include <arpa/inet.h>
#include <memory.h>



int server_connect(int port_num, int other_ports[]){
    int messageLen;
    char buffer[2048];
    struct sockaddr_in clientaddr;
    socklen_t clientaddrLen = sizeof(clientaddr);
    struct sockaddr_in myAddr;

for(int i=0;i<4;i++){
    int sockfd[5];
    sockfd[i] = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd[i] < 0)
        perror("ERROR opening socket");

    memset(&myAddr, '0', sizeof(myAddr)); // initialize struct
    myAddr.sin_family = AF_INET;
    myAddr.sin_addr.s_addr = htonl(INADDR_ANY); // specify IP address to be current machine's address (localhost)

    myAddr.sin_port = htons(other_ports[i]); // specify port number



    int j = bind(sockfd, (struct sockaddr*)& myAddr, sizeof(myAddr));

    if(j<0)

    {
        perror(" bind failed\n");
    }


    while (1){
        bzero(buffer, 2048);

        messageLen = recvfrom(sockfd, buffer, 2048, 0, (struct sockaddr*) &clientaddr, &clientaddrLen);

        if (messageLen > 0) {

            printf("message received: size %d\n",messageLen);
            printf("server received %s\n", buffer);
        }
        if(messageLen<0)
            perror("error in receiving message\n");
    }

}


}