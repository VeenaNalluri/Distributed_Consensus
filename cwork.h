#ifndef CWORK_H
#define CWORK_H

// initializing the instance with correct port num, and sockets to other port,
// uses consensus protocol to check (if other port balances are 0)
void initialize(int port_num, int* other_ports);

// itself is coordinator which checks (always passes) with other ports, and then credits to account
void credit(float money);

// itself coordinator which checks (that there's enough money) with other ports, and then debits to account
void debit(float money);

// return current balance
void query();

#endif
