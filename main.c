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
        newProc.c_pid = -1;
        push(newProc,temp);     //push proc to temp queue
          
    }
}
//Should use a Seeeeeeema-phore before allocating resources (mem + I/O)
void checkArrival(int timeNow){
    //okay so this is coming out as null.. means first item in temp has null arrival time????
    
    if (temp->next == NULL){
        printf("goodbyee");
        return;
    }
    while(temp->next->process.arrivalTime <= timeNow){
        proc_t arrivedProc = pop(temp); //remove element from temp queu.. now we must add it to its correct queue
        push(arrivedProc,queues[arrivedProc.priority]);
        //printf("done\n");
    }
}
//This section is used to find free memory. Returns location of memory. -1 if none avaliable
//Need to refactor and clean up this code, will do it soon. But works for now
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

//must clear that was being used.. 
void ClearMemory(int startPos, int amount){
    int i = startPos;
    int end = startPos + amount;
    for (i = startPos;i < end;i++){
        avail_mem[i] = 0;
    }
}


//for now, lets assume there will always be enough memory for priority procs
void runPriority(){
    while(queues[0]->next != NULL) {
        proc_t currProc = pop(queues[0]);
        int memoryStart = FindFreeMemory(currProc.memory);
        int status;
	    pid_t c_pid, pid;
        c_pid = fork();
        //child
        if (c_pid == 0){
            printf("executing ./process: \n");	
            execlp("./process", "./process", NULL);
            perror("execvp failed\n");
        }
        //parent
        else if(c_pid > 0){
            sleep(currProc.processorTime);
            kill(c_pid, SIGINT);
            if( (pid = wait(&status)) < 0){
                perror("wait");
                _exit(1);
            }
        }
        currTime = currProc.processorTime;
        checkArrival(currTime);

        printf("Arrival Time Finished Priority proc: %d\n",currProc.arrivalTime);
    }
}

void runQueueOne(){
    //lets run the process for one second, then check if this queue or queue above has anything 
    //if it does, then move down one slot
    proc_t currProc = pop(queues[1]);
    //this part is for if the process was running before. 
    if (currProc.c_pid != -1){
        kill(currProc.c_pid, SIGCONT);
        do{
            sleep(1);
            currProc.processorTime--;
            currTime++;
            checkArrival(currTime);
            //lets check if above queue or this queue is empty, as well there should be processor time remaining 
        } while(currProc.processorTime > 0 && queues[0]->next == NULL && queues[1]->next == NULL);
        if(currProc.processorTime > 0){
            kill(currProc.c_pid, SIGTSTP);
            // if( (currProc.c_pid = wait(&status)) < 0){
            //     perror("wait");
            //     _exit(1);
            // }
            push(currProc,queues[2]);
        }else{
            kill(currProc.c_pid, SIGINT);
        }
    }else {         //if process has not been ran before. So we're starting a new process here
    //child
        printf("log loc 1\n");
        int status;
	    pid_t c_pid, pid;
        c_pid = fork();
        if (c_pid == 0){ 
            printf("executing ./process: \n");	
            execlp("./process", "./process", NULL);
            perror("execvp failed\n");   
        }
        //parent
        else if(c_pid > 0){
            currProc.c_pid = c_pid;
            do{
                printf("log loc 2\n");
                sleep(1);
                currProc.processorTime--;
                currTime++;
                checkArrival(currTime);
                //lets check if above queue or this queue is empty, as well there should be processor time remaining 
            } while(currProc.processorTime > 0 && queues[0]->next == NULL && queues[1]->next == NULL);
            printf("log loc 3\n");
            if(currProc.processorTime > 0){
                printf("log loc 4\n");
                kill(currProc.c_pid, SIGTSTP);
                printf("log loc 5\n");
                // if( (currProc.c_pid = wait(&status)) < 0){
                //     perror("wait");
                //     _exit(1);
                // }
                push(currProc,queues[2]);
                printf("log loc 6\n");
            }else{
                kill(currProc.c_pid, SIGINT);
            }
        }
    }
}
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
    while(queues[0]->next != NULL || queues[1]->next != NULL){
        if(queues[0]->next != NULL)
            runPriority();
        if(queues[1]->next != NULL)
            runQueueOne();
    }
    
    //Just testing for now, this part will be removed
    //loop through each item and print out
    currItem = queues[0];
    while(currItem->next != NULL){  
        proc_t nextProc = pop(currItem);
        printf("Arrival Time for New Priority: %d\n",nextProc.arrivalTime);
    }
    currItem = queues[1];
    while(currItem->next != NULL){  
        proc_t nextProc = pop(currItem);
        printf("Arrival Time for New Queue 1: %d\n",nextProc.arrivalTime);
    }
    currItem = queues[2];
    while(currItem->next != NULL){  
        proc_t nextProc = pop(currItem);
        printf("Arrival Time for New Queue 2: %d\n",nextProc.arrivalTime);
    }
    currItem = queues[3];
    while(currItem->next != NULL){  
        proc_t nextProc = pop(currItem);
        printf("Arrival Time for New Queue 3: %d\n",nextProc.arrivalTime);
    }
    free(temp);
    for(i = 0;i < 4;i++){
        free(queues[i]);
    }
}