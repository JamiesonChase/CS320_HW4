/*
* Antonio Maniscalco, Chase Jamieson, Josh Sedig
* CS 460: Project 2
* Instructor: Dr. Xuechen Zhang
* Header File
*/

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

//for our bool flags
typedef enum {T, F} boolean;

//prototypes
int checkArgs(int argc, char *argv[]);
void *fileRead(void *args);
void *cpuSchedule();
void *ioSchedule();
void init();
struct PCB *createPCB(int id);
struct PCB *findShortestNode(struct PCB **head, int mode);
struct PCB *qPop(struct PCB **head);
int isEmptyQ(struct PCB *first);
void printQ(struct PCB **head);
void freeMem(struct PCB *head);
void push(struct PCB **head, struct PCB *newPcb);

//structure will store our command line args
struct cmdArgs {
    FILE* fp;
    char algo[5];
    char file_name[120];
    int quant;
};

//structure for our pcb
struct PCB {
    int ID;
    int quantumTime, cpuTime, priority, cpuIndex, ioIndex;
    int totalBursts, numCPUBursts, totalBurstTime, numIOBursts;         
    int *CPUBurst, *IOBurst;   
    struct timespec ts_begin, ts_end;
    struct PCB *prev, *next;     
};

// global variables
sem_t sem_read, sem_cpu, sem_io, sem_mutex;
struct cmdArgs arg;
struct PCB *readyQ, *ioQ;
boolean scheduleDone, cpuBusy, readDone, ioBusy, isRR;
double totalWaitTime, elapsed, throughput, turnaroundTim, totalProcNum;



