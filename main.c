#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h> 
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>
#include "queue.h"
#define MEMORY 1024

int avail_mem[MEMORY] = {0};
queue_t *temp = NULL;
queue_t *queues[4];
int currTime = 0;

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

void checkArrival(int timeNow){
    //okay so this is coming out as null.. means first item in temp has null arrival time????
    
    if (temp->next == NULL){
        printf("goodbyee");
        return;
    }
    while(temp->next->process.arrivalTime == timeNow){
        proc_t arrivedProc = pop(temp); //remove element from temp queu.. now we must add it to its correct queue
        push(arrivedProc,queues[arrivedProc.priority]);
        //printf("done\n");
    }
}

int FindFreeMemory(int amountNeeded){
    int startPos = 0;
    int currPos = 0;
    bool found = false;
    printf("amountn eeded %d\n",amountNeeded);
    while(found == false && startPos < MEMORY){

        while(startPos < MEMORY && avail_mem[startPos] != 0  ){
            startPos++;
        }
        for(currPos = startPos;currPos-startPos+1 <= amountNeeded;currPos++){
            if (currPos >= MEMORY){
                break;
            }
            if(avail_mem[currPos] == 1){
                startPos = currPos+1;
                break;
            }
            if(currPos-startPos+1 == amountNeeded){
                found = true;
                break;
            }
        }
    }
    if (found == true){
        for (currPos = startPos;currPos-startPos + 1<= amountNeeded;currPos++){
            avail_mem[currPos] = 1;
        }
        return startPos;
    }else{
        return -1;
    }
}

// void runPriority(){

// }

int main(){
    //initializing memory for ALL quues.. 
    int i = 0;
    temp = (queue_t *)malloc(sizeof(queue_t)); 
    temp->next = NULL;      //make next item null (part of initialization of temp queue) 
    for(i = 0;i < 4;i++){
        queues[i] = (queue_t *)malloc(sizeof(queue_t));
        queues[i]->next = NULL;
    }
    //proc_t testProc;
   // testProc.arrivalTime = 5;
    //push(testProc,queues[0]);
    // realTime = (queue_t *)malloc(sizeof(queue_t));
    // dispatch0 = (queue_t *)malloc(sizeof(queue_t));
    // dispatch1 = (queue_t *)malloc(sizeof(queue_t));
    // dispatch2 = (queue_t *)malloc(sizeof(queue_t));

    readDispatchList();     //read file now!!
    queue_t *currItem = temp;//queues[0];   
    int startTime = currItem->next->process.arrivalTime;    //have of the first process
    checkArrival(startTime);
    currItem = queues[0];
    //Just testing for now, this part will be removed
    //loop through each item and print out
    while(currItem->next != NULL){  
        proc_t nextProc = pop(currItem);
        printf("Arrival Time for New Item: %d\n",nextProc.arrivalTime);
    }

    free(temp);
    for(i = 0;i < 4;i++){
        free(queues[i]);
    }
}