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

// CHANGE accordingly
void show_help() {
    printf("-p <port_num> The port number of the instance");
    printf("-n <connecting_portnum>");
    printf("-c <credit> Credit amount");
    printf("-d <debit> Debit amount");
    printf("-q <query> Balnace enquiry");
}


//connects to other instances
int initialize(int port_num, int other_ports[]){
    int sockfd;
    /* Create the socket */

        sockfd = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0) {
            printf("client_conn_init: failed to create socket on the client (%d)\n");
            return -1;
        }


    struct sockaddr_in myaddr;
    /* bind to an arbitrary return address */
    /* because this is the client side, we don't care about the address */
    /* since no application will initiate communication here - it will */
    /* just send responses */ /* INADDR_ANY is the IP address and 0 is the socket */
    /* htonl converts a long integer (e.g. address) to a network representation */
    /* htons converts a short integer (e.g. port) to a network representation */

    memset((char *)&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    myaddr.sin_port = htons(port_num);

    int i = bind(sockfd, (struct sockaddr*)& myaddr, sizeof(myaddr));

    if(i<0)

    {
        perror(" bind failed\n");
    }

    char* host="localhost";
    struct hostent *hp;     /* host information */
    struct sockaddr_in servaddr;    /* server address */
    memset((char*)&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(other_ports[0]);
    servaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    char* message = "debit : 400";
    int j = sendto(sockfd, message, strlen(message), 0, (struct sockaddr*) &servaddr, sizeof(servaddr));
    if (j <0)
    {
       perror("send to failed\n");
    }
}





// Change accordingly
int main(int argc, char* argv[])
{
    char c;

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
            for (int i = 0;i<=3;i++){
                other_ports[i] = atof(optarg);
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
    }

    int initialize(int port_num, int other_ports);

 }