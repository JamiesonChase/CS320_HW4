/*
* Antonio Maniscalco, Chase Jamieson, Josh Sedig
* CS 460: Project 2
* Instructor: Dr. Xuechen Zhang
* Threads File
*/
#include "HEADERS.h"

//cpu scheduler function. Will sleep for a specified burst time then puts node into io que
void *cpuSchedule(void *args) {
    int sWait;
    boolean isRR = F,  rrFinished = F;
    struct cmdArgs *arguments = args;
    struct timespec ts;

    while (1) {
        //if all are true we are done and can break from look
        if (readDone == T && ioBusy == F && !isEmptyQ(ioQ) && !isEmptyQ(readyQ)) {
            scheduleDone = T;
            cpuBusy = F;
            break;
        }

        //set ts to our system clock then add one second
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += 1;

        //wait for sem read
        while ((sWait = sem_timedwait(&sem_cpu, &ts)) == -1 && errno == EINTR) continue;

        //check for time out
        if (sWait == -1) fprintf(stderr, "%s\n", strerror(errno));

        if (sem_wait(&sem_mutex) == -1) {
            fprintf(stderr, "%s\n", strerror(errno));
            exit(errno);
        }

        //now make our temp structure and set cpu to busy
        cpuBusy = T;
        struct PCB *temp;


        //Figure out which algorithm we are using
        if (strcmp(arguments->algo, "RR") == 0) {
            isRR = T;
            temp = qPop(&readyQ);
            if(temp->cpuTime > temp->quantumTime){
                usleep(temp->quantumTime);
                temp->cpuTime -= temp->quantumTime;
            } else {
                usleep(temp->cpuTime);
                temp->cpuTime = 0;
                rrFinished = T;
            }
        }
        else if (strcmp(arguments->algo, "PR") == 0) temp = findShortestNode(&readyQ, 1);
        else if (strcmp(arguments->algo, "SJF") == 0) temp = findShortestNode(&readyQ, 0);
        else if (strcmp(arguments->algo, "FIFO") == 0) temp = qPop(&readyQ);


        //if not rr we will sleep and increment cpuIndex
        if(isRR == F){
            usleep(temp->CPUBurst[temp->cpuIndex]);
            temp->cpuIndex += 1;
        }

        //record clock time if rr is done or cpu index is equal to num of bursts
        if (rrFinished == T || temp->cpuIndex == temp->numCPUBursts ) {
            clock_gettime(CLOCK_MONOTONIC, &temp->ts_end);
            elapsed = temp->ts_end.tv_sec - temp->ts_begin.tv_sec;
            elapsed += (temp->ts_end.tv_nsec - temp->ts_begin.tv_nsec) / 1000000000.0;

            //increment process num calculate turnaround and wait time
            totalProcNum += 1;
            turnaroundTim += (elapsed * 1000);
            totalWaitTime += (elapsed * 1000) - ((float)temp->totalBurstTime / 1000);
            

            //we are done with our node so free it and set busy flag to false
            free(temp->IOBurst);
            free(temp->CPUBurst);
            free(temp);
            cpuBusy = F;
        } else {
            temp->next = NULL;
            push(&ioQ, temp);
            cpuBusy = F;
        }
        //check our semaphores
        if(sem_post(&sem_io) == -1 || sem_post(&sem_read) == -1) {
            fprintf(stderr, "error: %s\n", strerror(errno));
            exit(errno);
        }
    }
    return(NULL);
}

//File read function. Will read the specified file and handle the different commands
void *fileRead(void *args) {
    struct cmdArgs *arg = args;
    struct PCB *currentPcb;

    boolean isProc = F;
    char *word, *line = NULL;
    int burstTime = 0, lineCount = 1, strVal;
    size_t linenum = 0;
    ssize_t getLineRes = getline(&line, &linenum, arg->fp);

    //read each line until eof
    while (getLineRes > 0) {
        //get the first word from our line we grabbed from the file
        word = strtok(line, " ");

        //for counting our traversal through each line to grab command, priority etc.
        int count = 1;

        //loop through line
        while(word != NULL) {
            //When count is 1 we are handling the command 
            if(count == 1) {
                if (strcmp(word, "stop") == 0) {
                    free(line);
                    fclose(arg->fp);
                    readDone = T;
                    return(void *)0;
                }

                //if it is sleep convert str to int and sleep for that amount then break
                else if (strcmp(word, "sleep") == 0) {
                    usleep(atoi(strtok(NULL, " ")));
                    break;
                }

                //handle process command
                else if (strcmp(word, "proc") == 0) {

                    isProc = T;
                    //make new pcb structure
                    currentPcb = createPCB(lineCount);
                    lineCount++;
                    burstTime = 0;
                    count+=2;


                    currentPcb->quantumTime = arg->quant;
                    currentPcb->priority = atoi(strtok(NULL, " "));

                    //grab the next word then take the cieling of total bursts divided by two
                    currentPcb->totalBursts = atoi(strtok(NULL, " "));
                    currentPcb->numCPUBursts = ceil((double)currentPcb->totalBursts / 2);
                    currentPcb->numIOBursts = ceil((double)currentPcb->totalBursts / 2);

                    //initialize io and cpu arr
                    currentPcb->IOBurst = malloc(sizeof(int) * currentPcb->numIOBursts);
                    currentPcb->CPUBurst = malloc(sizeof(int) * currentPcb->numCPUBursts);
                }
            }

            //if not even we deal with io if even we deal with cpu convert string to int and save value
            strVal = atoi(word);
            if (count%2 != 0) currentPcb->IOBurst[(count-3) / 2] = strVal;
            else {
                currentPcb->CPUBurst[(count-3) / 2] = strVal;
                currentPcb->cpuTime += strVal;
            }
            //add burst to integer value of string, grab next word then increment count
            burstTime += strVal;
            word = strtok(NULL, " ");
            count++;
        }
        // push proc to que
        if (isProc == T) {
            isProc = F;
            currentPcb->totalBurstTime = burstTime;

            if (sem_post(&sem_cpu) == -1) {
                fprintf(stderr, "error: %s\n", strerror(errno));
                exit(errno);
            }

            //get time and push to ready que
            clock_gettime(CLOCK_MONOTONIC, &currentPcb->ts_begin);
            push(&readyQ, currentPcb);

            if (sem_wait(&sem_read) == -1) {
                fprintf(stderr, "error: %s\n", strerror(errno));
                exit(errno);
            }
        }
        //grab next line
        getLineRes = getline(&line, &linenum, arg->fp);
    }
    return (void *)1;
}

//IO scheduler function. Function will 
void *ioSchedule()
{
    struct timespec ts;
    int t;
    while (1)
    {
        if (sem_post(&sem_mutex) == -1) {
            fprintf(stderr, "error: %s\n", strerror(errno));
            exit(errno);
            }
        
        //read is done cpu isnt busy and ques are not 
        if (readDone == T && cpuBusy == F && isEmptyQ(readyQ) == 0 && isEmptyQ(ioQ) == 0) {
            ioBusy = F;
            break;
        }

        //get time and lock semaphore then set io flag to true
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += 1;
        while ((t = sem_timedwait(&sem_io, &ts)) == -1 && errno == EINTR) continue;
        ioBusy = T;
        //check que then pop top and push to ready que
        if (isEmptyQ(ioQ) == 1) { 
            struct PCB *temp = qPop(&ioQ); 
            if(isRR == F) usleep(temp->IOBurst[temp->ioIndex]); 
            temp->ioIndex++;
            push(&readyQ, temp);
            if(sem_post(&sem_cpu) == -1) {
                fprintf(stderr, "error: %s\n", strerror(errno));
                exit(errno);
            }
        }

        ioBusy = F;
    }
    return(NULL);
}
