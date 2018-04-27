// Name: Aneesha Dasari and Veena Nalluri
// VUnetID: dasaria, nallurv
// Email: aneesha.dasari@vanderbilt.edu, veena.nalluri@vanderbilt.edu
// Class: CS 3281 - Vanderbilt University
// Description: Creates a bank account ledger that uses the two commit protocol for distributed consensus.
// This supports upto 5 instances of this program using sockets for communication between them.

#define _GNU_SOURCE
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <errno.h>


int curr_balance; //bank account balance
int value; //amount entered
int set_balance; //resetting the balance if disconnected
int printed = 0; // set if printed transaction failed message
int sockfd;     //filedescripter for socket
int acknowledge=0; //setting acknowledgement if message received is Yes
int sec_ack = 0; //second acknowledge
int myport = -1; //coordinator port[atm where the transaction is done]
//receiving ports[other atms where the transaction can be done]
int receiver_port[] = {-1,-1,-1,-1};
struct sockaddr_in myaddr,addr[4],address;
socklen_t addressLength = sizeof(address);
socklen_t myaddrLength = sizeof(myaddr);

void show_prog_help()
{
    printf("Within the program, you can do any of the following actions:\n");
    printf("\tquery - You can call this by typing \"query\". This prints the account balance.\n");
    printf("\tcredit:<val> - You can call this by typing something like \"credit:10\". This adds val to the account balance.\n");
    printf("\tdebit:<val> - You can call this by typing something like \"debit:10\". This remove val from the account balance.\n");
    printf("\thelp - You can call this by typing \"help\". This shows this help message.\n");
    printf("\texit - You can call this by typing \"exit\". This exits the program.\n");
}

void show_help() 
{
    printf( "Use: main [options]\n");
    printf("Where options are:\n");
    printf("-p <port number> This is the port number of this instance of the program\n");
    printf("-n <four port numbers> These need to be four unique port numbers that will be connected to instance of program\n");
    printf("-h <> Shows this help text.\n\n");
    printf("To properly use this program, you must open five terminals where you will create five instances of the program each with different port numbers and connecting them to the other ports.\n");
    printf("To start an instance properly, you must give it an argument for p and four arguments for n.\n");
    printf("For example, these might be the calls you make in five different terminals:\n");
    printf("\t1. ./main -p 5600 -n 5601,5602,5603,5604\n"); 
    printf("\t2. ./main -p 5601 -n 5600,5602,5603,5604\n"); 
    printf("\t3. ./main -p 5602 -n 5601,5600,5603,5604\n"); 
    printf("\t4. ./main -p 5603 -n 5601,5602,5600,5604\n"); 
    printf("\t5. ./main -p 5604 -n 5601,5602,5603,5600\n\n"); 
    show_prog_help();
}

// receiving_ports
// implements two phase commit protocol
void *receiving_ports(void *arg) {

    int messagelen;
    char receive_buffer[2048];
    socklen_t addressLength = sizeof(address);

    while (1) {

        messagelen = recvfrom(sockfd, receive_buffer, 2048, 0, (struct sockaddr *) &address, &addressLength);
        
	if (messagelen > 0) {
            char *command = strtok(receive_buffer, ":"); //type of transaction
            char *amount = strtok(NULL, ":");//amount to be credited/debited
	    //printf("command:%s\n", command);

            if (strcmp(command, "credit") == 0) {//if command is credit
                value = atoi(amount);//set the value
                char credit_ack[20] = "";
                    sprintf(credit_ack, "Yes:%d",myport );
                    printf("message sent is %s\n",credit_ack);
                    sendto(sockfd, credit_ack, strlen(credit_ack), 0, (struct sockaddr *) &address,sizeof(address));//send Yes

                //setting the balance


            } else if (strcmp(command, "debit") == 0) {//if the commmand is debit
                value = atoi(amount);

                if (curr_balance+value >= 0) {//check if the debit operation can be performed
                    char debit_ack[20] = "";
                    sprintf(debit_ack, "Yes:%d",myport );
                    printf("message sent is %s\n",debit_ack);
                    sendto(sockfd, debit_ack, strlen(debit_ack), 0, (struct sockaddr *) &address,sizeof(address));//send Yes

                } else {
                    char debit_ack[20] = "";
                    sprintf(debit_ack, "No:%d",myport );
                    printf("message sent is %s\n",debit_ack);
                    sendto(sockfd, debit_ack, strlen(debit_ack), 0, (struct sockaddr *) &address,sizeof(address));//send No to cancel the transaction
                }
		printed = 0;

            } else if (strcmp(command, "Yes") == 0) {
                printf("Acknowledge is from port %s\n",amount);
                ++acknowledge;
                //
                //printf("Acknowledge from the port%d\n",)

                if (acknowledge == 4) {//if all the atms send acknowledgement consensus is achieved
                    curr_balance = curr_balance+value;
                    printf("Transaction Succeeded\n");
                    for (int i = 0; i < 4; i++) {
                        sendto(sockfd, "commit", strlen("commit"), 0, (struct sockaddr *) &addr[i], sizeof(addr[i]));//send commit the transaction to other atms
                    }
		    acknowledge = 0;
                }

            } else if (strcmp(command, "No") == 0) {
                for (int i = 0; i < 4; i++) {
                    sendto(sockfd, "abort", strlen("abort"), 0, (struct sockaddr *) &addr[i], sizeof(addr[i]));//send abhort to cancel the transaction
                }
		if (printed != 1) {
			printf("Transaction failed.\n");
			printed = 1;
		}

            } else if (strcmp(command, "commit") == 0) { // if confirmation of transaction is recieved
                curr_balance = curr_balance + value;//change the balance in all atms

            } else if (strcmp(command, "abort") == 0) {
                //do nothing
            
            } else if (strcmp(command, "disconnect") == 0) {//terminates and reconnects
                char ReConnect[20] = "";
                sprintf(ReConnect, "setbalance:%d", curr_balance);//setting the balance
                sendto(sockfd, ReConnect, strlen(ReConnect), 0, (struct sockaddr*) &address, sizeof(address)); // send balance

            } else if (strcmp(command, "setbalance") == 0) {
		value = atoi(amount);
                if (sec_ack == 0) {
                    set_balance = value;
                    ++sec_ack;
                } else {
                    if (set_balance == value) {
                        ++sec_ack;
                        if (sec_ack == 4) {
			    curr_balance = set_balance;
                            sec_ack = 0;//reset the acknowledge
                        }
                    }
                }
            }
	    
	    // sets receive_buffer back
            for (int i = 0; i < messagelen; ++i) {
                receive_buffer[i] = '\0';
            }

        } else {
                printf("There's an error with recvfrom:s %d\n", errno);//recvfrom has an error
        }
    }
}

int main(int argc, char* argv[])
{
    char c;
    char *nums;



    // For each command line argument given,
    // override the appropriate configuration value.
    while ((c = getopt(argc, argv, "p:n:h")) != -1) {
    	switch(c) {
	case 'p':
		myport = atoi(optarg);
		break;
	case 'n':
		nums = strtok(optarg, ",");
		for(int i=0;i< 4 && nums != NULL;i++){
       			receiver_port[i] = atoi(nums);
			nums = strtok(NULL,",");
     		}
		break;
	case 'h':
		show_help();
		exit(1);
		break;
	}
    }

    //if not set properly, exit
    if (myport == -1){
	printf("Port number was not set. Run ./main -h for help.\n");
	exit(1);
    }
    for (int i = 0; i < 4; i++) 
    {
	if (receiver_port[i] == -1) {
		printf("Not enough ports given. Run ./main -h for help.\n");
		exit(1);
	}
     }
    
   //Create the socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        printf("Failed to create a socket\n");
        exit(1);
    }

    // This binds to an arbitrary return address because this is the client side and 
    // we don't care about the address. Since no application will initiate communication 
    // here - it will just send responses. 
    // INADDR_ANY is the IP address and 0 is the socket
    // htonl converts a long integer (e.g. address) to a network representation
    // htons converts a short integer (e.g. port) to a network representation

    memset((char *)&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    myaddr.sin_port = htons(myport);

    int i = bind(sockfd, (struct sockaddr*)& myaddr, sizeof(myaddr));

   // error checking
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

    // create thread to send and receive commands
    pthread_t p1;
    int create = pthread_create(&p1,NULL,receiving_ports,NULL);
    
    // error checking
    if (create != 0) {
        printf("pthread_create failed\n");
        exit(1);
    }

    // Used for Fault Case
    // Uses commit protocol to update balance
    for(int j=0;j<4;j++){
                sendto(sockfd, "disconnect", strlen("disconnect"), 0, (struct sockaddr*) &addr[j], sizeof(addr[j]));
    }

     // takes in command
     while(1){

        //input has to be taken from console
        printf("Enter desired action:\n");

        char * send_buffer; //buffer to store the input
        size_t bufsize = 32;
        send_buffer = (char*)malloc(bufsize*sizeof(char));
        getline(&send_buffer,&bufsize, stdin);

        char *command = strtok(send_buffer,":");//type of transaction

	//CREDIT command
        if(strcmp(command,"credit")==0){
            acknowledge = 0;
            value = atoi(strtok(NULL, ":"));//amount to be debited/credited
	    // printf("Amount is %d\n",value);
            char credit_buf[20] = ""; // intialize credit message
            sprintf(credit_buf, "credit:%d",value);//credit_buffer to send credit info
            for (int j = 0; j < 4; j++) {
                sendto(sockfd,credit_buf, strlen(credit_buf), 0, (struct sockaddr *) &addr[j], sizeof(addr[j]));
            }

	// DEBIT command
        }else if(strcmp(command,"debit")==0) {//if command is debit
            acknowledge = 0;
            value = -atoi(strtok(NULL,":")); //amount to be debited/credited

            for(int j=0;j<4;j++){
                char debit_buf[20] = "";
                sprintf(debit_buf, "debit:%d",value);
                sendto(sockfd, debit_buf, strlen(debit_buf), 0, (struct sockaddr*) &addr[j], sizeof(addr[j]));
            }
         
         //QUERY command
        } else if(strcmp(command,"query")==0  || strcmp(command, "query\n") == 0) {
            printf("Current Balance: %d\n",curr_balance);

	// EXIT command
        } else if (strcmp(command, "exit") == 0  || strcmp(command, "exit\n") == 0) { //exit
            exit(0);

	// HELP command
	} else if (strcmp(command, "help") == 0 || strcmp(command, "help\n") == 0) {//show help
	    show_prog_help();

	// if command not found
        } else {
           printf("This is an unknown command.\n");
        }
     }
 }