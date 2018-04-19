#ifndef CWORK_H
#define CWORK_H

// initializing the instance with correct port num, and sockets to other port,
// uses consensus protocol to check (if other port balances are 0)
int initialize(int myport, int receiver_port[4]);

void *receiving_ports(int receiver_port[]);

// itself is coordinator which checks (always passes) with other ports, and then credits to account
void credit(float money);

// itself coordinator which checks (that there's enough money) with other ports, and then debits to account
void debit(float money);

// return current balance
void query();

#endif
