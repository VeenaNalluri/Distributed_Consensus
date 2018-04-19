// Name: Aneesha Dasari and Veena Nalluri
// VUnetID: dasaria, nallurv
// Email: aneesha.dasari@vanderbilt.edu, veena.nalluri@vanderbilt.edu
// Class: CS 3281 - Vanderbilt University
// Description: Creates a bank account ledger that uses the two commit protocol for distributed consensus.
// This supports upto 5 instances of this program using sockets for communication between them.

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

// CHANGE accordingly
void show_help() {
    printf("-p <port_num> The port number of the instance");
    printf("-n <connecting_portnum>");
    printf("-c <credit> Credit amount");
    printf("-d <debit> Debit amount");
    printf("-q <query> Balnace enquiry");
}

char * buffer;
size_t bufsize = 32;
size_t transaction;
int amount;
int curr_balance = 0;
int sockfd;
struct sockaddr_in myaddr,addr[4];
socklen_t addressLength = sizeof(myaddr);
int query;
int credit;
int debit;
//connects to other instances

void *receiving_ports(int receiver_port[]){

    int message;

    char acknowledge = 'yes';
    message = recvfrom(sockfd, buffer, 2048, 0, (struct sockaddr*) &myaddr, &addressLength);
    char *command = strtok(message,":");
    amount = strtok(NULL,":");
    if(strcmp(command,query)){

        sendto(sockfd,acknowledge,strlen(acknowledge),0, (struct sockaddr*)&myaddr,&addressLength );
    }
    else if(strcmp(command,credit)){
        sendto(sockfd,acknowledge,strlen(acknowledge),0, (struct sockaddr*)&myaddr,&addressLength );
    }
    else{
        sendto(sockfd,acknowledge,strlen(acknowledge),0, (struct sockaddr*)&myaddr,&addressLength );
    }

}

int initialize(int myport, int receiver_port[4]){


    /* Create the socket */

        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0) {
            printf("Failed to create a socket\n");
            return -1;
        }



    /* bind to an arbitrary return address */
    /* because this is the client side, we don't care about the address */
    /* since no application will initiate communication here - it will */
    /* just send responses */ /* INADDR_ANY is the IP address and 0 is the socket */
    /* htonl converts a long integer (e.g. address) to a network representation */
    /* htons converts a short integer (e.g. port) to a network representation */

    memset((char *)&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    myaddr.sin_port = htons(myport);

    int i = bind(sockfd, (struct sockaddr*)& myaddr, sizeof(myaddr));

    if(i<0)

    {
        perror(" bind failed\n");
    }
    for(int j=0;j<4;j++){

        memset((char *)&addr[j], 0,sizeof(addr[j]));
        addr[j].sin_family = AF_INET;
        addr[j].sin_addr.s_addr = htonl(INADDR_LOOPBACK);
        addr[j].sin_port = htons(receiver_port[j]);

    }
    pthread_t p1;
    int create = pthread_create(&p1,NULL,receiving_ports,NULL);
    while(1){
        //input has to be taken from console
        printf("Enter transaction type and amount");

        buffer = (char*)malloc(bufsize*sizeof(char));
        transaction = getline(&buffer,&bufsize, stdin);
        char *command = strtok(transaction,":");
        while(command!=NULL){
            printf("%s\n",command);
            amount = strtok(NULL,":");
        }
        if(strcmp(command,query)){
            printf("Current Balance is %d\n",curr_balance);
        }
        else if(strcmp(command,credit)) {
            curr_balance = curr_balance + amount;
            for (int j = 0; j < 4; j++) {
                sendto(sockfd,transaction, strlen(transaction), 0, (struct sockaddr *) &addr[j], sizeof(addr[j]));
            }
        }else {
            curr_balance = curr_balance-debit;
                for(int j=0;j<4;j++){
                    sendto(sockfd, transaction, strlen(transaction), 0, (struct sockaddr*) &addr[j], sizeof(addr[j]));
                }
            }

        }
    }











// Change accordingly
int main(int argc, char* argv[])
{
    /*char c;

    // These are the default configuration values used
    // if no command line arguments are given.

    int port_num = 5600;
    int other_ports[4] = {5601,5602,5603,5604};
    double money = 0;
    double credit = 0;
    double debit = 0;
    double curr_balance = 0;


    // For each command line argument given,
    // override the appropriate configuration value.

    while ((c = getopt(argc, argv, "p:n:c:d:h")) != -1) {
        switch (c) {
        case 'p':
            port_num    = atof(optarg);
            break;
        case 'n':

//delim = strtok(other_ports[i],)
            for (int i = 0;i<=3;i++){

                char str = atof(optarg);
                int *otherports;
                other_ports[i] = strtok(str,",");
            }

            break;
        case 'c':
            credit = atof(optarg);
            break;
        case 'd':
            debit = atoi(optarg);
            break;
        case 'h':
            show_help();
            exit(1);
            break;
        }
    }*/
    int myport = atoi(argv[1]);
    int receiver_port[4];
    for(int i=0;i< 4;i++){
        receiver_port[i] = atoi(argv[i+2]);
    }
    int initialize(int myport, int receiver_port[4]);
    /*pthread_t p1;
    int create = pthread_create(&p1,NULL,receiving_ports,NULL);
*/
 }