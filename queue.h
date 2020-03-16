typedef struct proc{
    char name[256];
    int priority;
    int pid;
    int address;
    int memory;
    int runtime;
    bool suspended;
} proc_t;

typedef struct queue{
    struct proc process;
    struct queue *next;
} queue_t;

void push(proc_t process, queue_t *head){
    int i = 0;
    queue_t * curr;
    curr = head;
    
    for(i = 0; i < count;i++){
        curr = curr->next; 
    }

    curr->next = (queue_t *)malloc(sizeof(queue_t)); 
    curr->next->process = process;
    curr->next->next = NULL;
}

struct proc pop(queue_t *head){
    queue_t *curr = head;
    queue_t *remove = curr->next;
    struct proc retProc = remove->process;
    curr->next = remove->next;
    free(remove);
    return retProc;
}