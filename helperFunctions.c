/*
* Antonio Maniscalco, Chase Jamieson, Josh Sedig
* CS 460: Project 2
* Instructor: Dr. Xuechen Zhang
* Helper Functions File
*/
#include "HEADERS.h"

//Function to initialize our global variables
void init() {
    readyQ = NULL;
    ioQ = NULL;
    readDone = F;
    scheduleDone = F;
    cpuBusy = F;
    ioBusy = F;
    isRR = F;
    throughput = 0;
    totalWaitTime = 0;
    turnaroundTim = 0;
    totalProcNum = 0;
}

//Function to create a new pcb
struct PCB *createPCB(int id) {
    //allocate memory then set id then other attributes to null or zero then return new pcb
    struct PCB *pcb = malloc(sizeof(struct PCB));
    pcb->ID = id;
    pcb->prev = pcb->next = NULL;
    pcb->ioIndex = pcb-> cpuIndex = 0;
    return(pcb);
};

//Function to push a node into a que
void push(struct PCB **head, struct PCB *newPcb){
    if(*head == NULL) {
        *head = newPcb;
        return;
    } 
    //make temp pcb to grab end of linked list
    struct PCB *temp = *head;
    while(temp->next != NULL) temp = temp->next;
    temp->next = newPcb;
    newPcb->next = NULL;
    newPcb->prev = temp;
    
}

//Function to find the node with the shortest burst time
struct PCB *findShortestNode(struct PCB **head, int mode){
    //in the case there is one item in the list return that one
    if((*head)->next == NULL) return(qPop(head));
    //we will use temp and shortest to traverse our ques 
    struct PCB *temp = *head;           
    struct PCB *shortest = temp;  

    //loop to compare nodes
    temp = temp->next;
    while(temp != NULL) {
        //check the mode sjf is mode is one and pr if it is 0
        if(mode == 1) {
            if(temp->priority > shortest->priority) shortest = temp;
        }
        else if(mode == 0) {
            if(temp->totalBurstTime < shortest->totalBurstTime) shortest = temp;
        }
        //grab next node
        temp = temp->next;
    }
    //take the node off of the que
    if(shortest->prev != NULL) shortest->prev->next = shortest->next;
    else *head = shortest->next;
    if(shortest->next != NULL) shortest->next->prev = shortest->prev;

    return(shortest);
}


//pop head of que and return node
struct PCB *qPop(struct PCB **head){
    if(*head == NULL) return(NULL);
    struct PCB *temp = *head;
    *head = temp->next;
    return(temp);
}

//function to check if que is empty
int isEmptyQ(struct PCB *head){
   return(head == NULL ? 0 : 1); 
}  

