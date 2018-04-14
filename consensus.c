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

// CHANGE accordingly
void show_help() {
    printf("-p <port_num> The port number of the instance");
    printf("-n <connecting_portnum>");
    printf("-c <credit> Credit amount");
    printf("-d <debit> Debit amount");
    printf("-q <query> Balnace enquiry");
}


//connects to other instances
int initialize(int port_num, int other_ports){
    int sockfd[5];
    /* Create the socket */
    for (int i = 0;i<=5;i++) {
        sockfd[i] = socket(AF_INET, SOCK_DGRAM, 0);
        if (sockfd < 0) {
            printf("client_conn_init: failed to create socket on the client (%d)\n");
            return -1;
        }

    }

    /* Set buffer size */
    int buf_size = (1024 * 1024);
    for(int i =0;i<=5;i++){
        if (setsockopt(sockfd[i], SOL_SOCKET, SO_RCVBUF, &buf_size,
                       sizeof(buf_size)) < 0) {
            printf("client_conn_init: failed to set recv buf (%d)\n");
            return -1;
        }
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



 }