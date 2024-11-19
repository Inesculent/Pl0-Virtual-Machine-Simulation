Inesculent
4/7/2024

The following "VM Simulation" utilizes 3 different files to simulate a virtual machine that can execute code in pl/0.

Example:

Given code in pl0 with the following syntax:

var a, x, b;
x := b+ 5 * a.

First execute compiler.c to turn the code into machine code.
gcc compiler.c
./a.out yourFile.txt


From here, the output can be taken and input into vm-1.c
gcc compiler.c
./a.out yourFile.txt

If the code is syntactically correct, the following operations should return the output of the above code, simulating the operations the virtual machine would have to undergo in this process.
