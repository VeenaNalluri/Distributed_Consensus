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
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <errno.h>

// CHANGE accordingly
void show_help() {
    printf("-p <port_num> The port number of the instance");
    printf("-n <connecting_portnum>");
    printf("-c <credit> Credit amount");
    printf("-d <debit> Debit amount");
    printf("-q <query> Balnace enquiry");
}




int curr_balance;
int amount;
int sockfd;
struct sockaddr_in myaddr,addr[4],address;
socklen_t addressLength = sizeof(address);
socklen_t myaddrLength = sizeof(myaddr);


int acknowledge=0;
socklen_t receiverAddressLength[4];

//connects to other instances

void *receiving_ports(void *arg) {

    int messagelen;
    char receive_buffer[2048];
    socklen_t addressLength = sizeof(address);

    for(int i =0;i<4;i++){
        receiverAddressLength[i]= sizeof(addr[i]);
    }
    while (1) {
        //printf("In thread\n");
        messagelen = recvfrom(sockfd, receive_buffer, 2048, 0, (struct sockaddr *) &address, &addressLength);
        printf("message length %d \n", messagelen);
        printf("buffer received %s\n", receive_buffer);
        if (messagelen > 0) {
            //printf("hi");
            char *command = strtok(receive_buffer, ":");
            char *amount = strtok(NULL, ":");
            printf("%d\n",command);
            if (strcmp(command, "credit") == 0) {
                amount = atoi(amount);
                printf("Sending Yes to credit");
                sendto(sockfd, "Yes", strlen("Yes"), 0, (struct sockaddr *) &address,sizeof(address));
            } else if (strcmp(command, "debit") == 0) {
                int amount = atoi(amount);

                if (curr_balance >= 0) {
                    printf("Sending Yes to Debit");
                    sendto(sockfd, "Yes", strlen("Yes"), 0, (struct sockaddr *) &address, sizeof(address));
                } else {
                    printf("Sending No to Debit");
                    sendto(sockfd, "No", strlen("No"), 0, (struct sockaddr *) &address, sizeof(address));
                }

            } else if (strcmp(command, "Yes") == 0) {
                ++acknowledge;
                printf("Acknowledge is %s\n", acknowledge);
                if (acknowledge == 4) {
                    acknowledge = 0;
                    curr_balance = curr_balance + amount;
                    printf("Transaction Succeeded");
                    for (int i = 0; i < 4; i++) {

                        sendto(sockfd, "Commit", strlen("Commit"), 0, (struct sockaddr *) &addr[i], sizeof(addr[i]));
                    }

                }
            } else if (strcmp(command, "No") == 0) {

                for (int i = 0; i < 4; i++) {
                    sendto(sockfd, "Abhort", strlen("Abhort"), 0, (struct sockaddr *) &addr[i], sizeof(addr[i]));
                }

            } else if (strcmp(command, "commit") == 0) { // if confirmation of transaction is recieved
                curr_balance = curr_balance + amount;
                printf("Current Balance is %d\n", curr_balance);
            } else if (strcmp(command, "abort") == 0) {
                printf("Abhorted");
            }
            for (int i = 0; i < messagelen; ++i) {
                receive_buffer[i] = '\0';
            }}
        else {
                printf("error with recvfrom %d\n", errno);
        }
        }
    }


// Change accordingly
int main(int argc, char* argv[])
{

    int myport = atoi(argv[1]);
    int receiver_port[4];
    for(int i=0;i< 4;i++){
        receiver_port[i] = atoi(argv[i+2]);
        //printf("receiver port is \n%d",receiver_port[i]);
    }
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
        receiverAddressLength[j]= sizeof(addr[j]);
        printf("%d\n",receiver_port[j]);

    }
    pthread_t p1;
    int create = pthread_create(&p1,NULL,receiving_ports,NULL);

    if (create != 0) {
        printf("pthread_create failed\n");
        exit(1);
    }
    printf("about to enter transaction\n");
    while(1){
        //input has to be taken from console
        printf("Enter transaction type and amount\n");
        char * send_buffer;
        size_t bufsize = 32;
        send_buffer = (char*)malloc(bufsize*sizeof(char));
        getline(&send_buffer,&bufsize, stdin);
        printf("%s\n", send_buffer);
        char *command = strtok(send_buffer,":");
        printf("%s\n",command);
        char *amount = strtok(NULL,":");
        printf("%s\n",amount);
        if(strcmp(command,"credit")==0){
            acknowledge = 0;
            amount = atoi(amount);
            printf("Amount is %d\n",amount);
            //printf("Current Balance after credit is %d\n",curr_balance);
            char credit_buf[] = ""; // intialize debit message
            sprintf(credit_buf, "credit: %d",amount);
            //printf("%s",send_buffer);
            //printf("vefor %s\n",credit_buf);
            for (int j = 0; j < 4; j++) {
                //printf("%s\n",credit_buf);
                sendto(sockfd,credit_buf, strlen(credit_buf), 0, (struct sockaddr *) &addr[j], sizeof(addr[j]));
            }


        }
        else if(strcmp(command,"query")==0) {
            //amount = atoi(amount);
            printf("Current Balance after query %d\n",curr_balance);


        }else if(strcmp(command,"debit")==0) {
            acknowledge = 0;
            int amount = -atoi(amount);
            printf("Amount is %d\n",amount);
            //printf("Curr balance after debit %d\n",curr_balance);
            for(int j=0;j<4;j++){
                char debit_buf[] = "";
                sprintf(debit_buf, "debit:%d",amount);
                //printf("%s\n",debit_buf);
                sendto(sockfd, debit_buf, strlen(debit_buf), 0, (struct sockaddr*) &addr[j], sizeof(addr[j]));
            }

        }
    }

 }