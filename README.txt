Readme for consensus project

In order to build this, simply compile it with a c compiler. You can use the same directions that we have been used during this class. Go into the folder and issue the following commands at the terminal:
- mkdir build
- cd build
- cmake ..

Then you can issue "make" to compile the program and run it using ./main. Use "./main -h" to understand how to run and use the program. I have also pasted the help information below.

Use: main [options]
Where options are:\n");
-p <port number> This is the port number of this instance of the program
-n <four port numbers> These need to be four unique port numbers that will be connected to instance of program\n
-h <> Shows this help text.

To properly use this program, you must open five terminals where you will create five instances of the program each with different port numbers and connecting them to the other ports.
To start an instance properly, you must give it an argument for p and four arguments for n.
For example, these might be the calls you make in five different terminals:
	1. ./main -p 5600 -n 5601,5602,5603,5604
	2. ./main -p 5601 -n 5600,5602,5603,5604
	3. ./main -p 5602 -n 5601,5600,5603,5604
	4. ./main -p 5603 -n 5601,5602,5600,5604
	5. ./main -p 5604 -n 5601,5602,5603,5600

Within the program, you can do any of the following actions:\n");
	query:0 - You can call this by typing \"query:0\". This prints the account balance.
	credit:<val> - You can call this by typing something like \"credit:10\". This adds val to the account balance.
	debit:<val> - You can call this by typing something like \"debit:10\". This remove val from the account balance.
	balance - You can call this by typing something like\"balance\". This resets the account balance.