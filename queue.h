typedef struct proc{
    int arrivalTime;
    int priority;
    int processorTime;
    int memory;
    int printers;
    int scanners;
    int modems;
    int cds;
    pid_t c_pid;
    pid_t pid;
    int status;
} proc_t;

typedef struct queue{
    struct proc process;
    struct queue *next;
} queue_t;

void push(proc_t process, queue_t *head){
    int i = 0;
    queue_t * curr;
    curr = head;
    
    while(curr->next != NULL){
        curr = curr->next; 
    }

    curr->next = (queue_t *)malloc(sizeof(queue_t)); 
    curr->next->process = process;
    curr->next->next = NULL;
}

proc_t pop(queue_t *head){
    
    queue_t *curr = head;
    if(curr->next == NULL){
        printf("WHAT THE HECK\n\n");
    }
    queue_t *remove = curr->next;
    proc_t retProc = remove->process;
    curr->next = remove->next;
    free(remove);
    return retProc;
}