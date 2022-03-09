# CS460-Project-2

## Group Members:
Chase Jamieson, Antonio Maniscalco, Joshua Sedig

## To Compile:
  > make

## To Run:
  > ./prog -alg [FIFO | SJF | PR | RR] [-quantum [integer (ms) ]] -input [file name]

## Included Files:
- Main.c: Main thread, initilizes values, creates other threads and prints stats after.
- parseArguments.c: Verify Command line arguments 
- HelperFunctions.c: Functions to help initialize, manipulate queues and structures.
- - HEADERS.h: All includes, declarations and globals found here.

Threads.c: contains methods for 3 main threads mentioned below:

1.fileRead: Allocate file contents to proper variables and designated structs.

2. cpuScheduler: Emulate CPU scheduler, handle processes.

3.ioScheduler: Emulate IO device.




  
