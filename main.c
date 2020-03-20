#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include "queue.h"

queue_t *temp = NULL;
// queue_t *realTime = NULL;
// queue_t *dispatch0 = NULL;
// queue_t *dispatch1 = NULL;
// queue_t *dispatch2 = NULL;

//read from dispatch list and add to temp queue
void readDispatchList(){
    proc_t newProc;
    FILE *fptr;
    int i = 0;
    fptr = fopen("dispatchlist", "r");
    char lines[10][256];
    char currline[256];
    //read each line    
    while (fgets(currline, 256, fptr) != NULL){
        strncpy(&lines[i],currline,sizeof(currline)); 
        i++;
    }
    fclose(fptr);
    //tokenize each line, and add the items to a new proc, and then add the new proc to the temp queue
    for (i = 0;i<10;i++){
        char *token;
        int arrivalTime;
        int priority;
        int processorTime;
        int memory;
        int printers;
        int scanners;
        int modems;
        int cds;
        memset(&newProc, 0, sizeof newProc);    //empty the new proc variable
        //time to tokenize!!
        token = strtok(&lines[i],", ");
        arrivalTime = atoi(token);
        token = strtok(NULL,", ");
        priority = atoi(token);
        token = strtok(NULL,", ");
        processorTime = atoi(token);
        token = strtok(NULL,", ");
        memory = atoi(token);
        token = strtok(NULL,", ");
        printers = atoi(token);
        token = strtok(NULL,", ");
        scanners = atoi(token);
        token = strtok(NULL,", ");
        modems = atoi(token);
        token = strtok(NULL,", ");
        cds = atoi(token);

        newProc.arrivalTime = arrivalTime;
        newProc.priority = priority;
        newProc.processorTime = processorTime;
        newProc.memory = memory;
        newProc.printers = printers;
        newProc.scanners = scanners;
        newProc.modems = modems;
        newProc.cds = cds;
        push(newProc,temp);     //push proc to temp queue
          
    }
}

int main(){
    temp = (queue_t *)malloc(sizeof(queue_t));  //initializing memory for temp queue
    temp->next = NULL;      //make next item null (part of initialization of temp queue)
    readDispatchList();     //read file now!!
    queue_t *currItem = temp;   

    //Just testing for now, this part will be removed
    //loop through each item and print out
    while(currItem->next != NULL){  
        proc_t nextProc = pop(currItem);
        printf("Arrival Time for New Item: %d\n",nextProc.arrivalTime);
    }
    free(temp);
}