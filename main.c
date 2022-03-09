/*
* Antonio Maniscalco, Chase Jamieson, Josh Sedig
* CS 460: Project 2
* Instructor: Dr. Xuechen Zhang
* Main File
*/

#include "HEADERS.h"

int main(int argc, char *argv[]) {
    
    //check command line args exit if error if not initialize our globals
    if(checkArgs(argc, argv) == 0) exit(1);    
    init();

    
    //Create threads
    sem_init(&sem_mutex, 0, 1); 
    sem_init(&sem_read, 0, 1); // File read thread
    sem_init(&sem_cpu, 0, 0); // CPU scheduler thread
    sem_init(&sem_io, 0, 0); // IO scheduler thread

    pthread_t threads[3];

    pthread_create(&threads[0], NULL, fileRead, &arg);
    pthread_create(&threads[1], NULL, cpuSchedule, &arg);
    pthread_create(&threads[2], NULL, ioSchedule, NULL);

    // join all threads
    for (int i = 0; i < 3; i++) {
        if(pthread_join(threads[i], NULL) != 0) fprintf(stderr, "%s\n", strerror(errno));
        }

    //destroy the semaphores
    sem_destroy(&sem_mutex); 
    sem_destroy(&sem_read); 
    sem_destroy(&sem_cpu); 
    sem_destroy(&sem_io);

    //write info to stdout
    printf("Throughput                      : %lf processes / ms\n", turnaroundTim / 100);
    printf("Avg. Turnaround time            : %lf ms\n", (turnaroundTim / totalProcNum) * 1000);
    printf("Avg. Waiting time in ready queue: %lf ms\n", (totalWaitTime / totalProcNum) * 100);
    printf("\nInput File Name                 : %s\n", arg.file_name);
    printf("CPU Scheduling Algorithm        : %s", arg.algo);
    if(arg.quant != 0) printf(": %i\n", arg.quant);
    else printf("\n");
    


    return(0);
}
