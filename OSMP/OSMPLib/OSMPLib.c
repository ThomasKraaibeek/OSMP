//
// Created by gurur on 24.04.19.
//

#include "OSMPLib.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

//SHM Struct
shm* shm_start;
sem_t mutex;
sem_t emptyslots;

clock_t start;

int processes = 0, rank = 0;

void debug(char* message, ...) {
#ifdef DEBUG
    clock_t stamp = clock();
	double runtime = ((double) (stamp - start)) / CLOCKS_PER_SEC;

	va_list args;
	va_start(args, message);
	printf("[PROC-%03d][%09.5f] ", rank, runtime);
	vprintf(message, args);
	printf("\n");
	fflush(stdout);
	va_end(args);
#endif
}

void error(char* msg, ...){
    debug("%s | %s\n", msg, strerror(errno));
    exit(0);
}


/**
 * Erzeugen des Shared Memory, zuschneiden und mappen.
 * @param 
 * @return 1=success, 0=fail
 */
int OSMP_Init(int *argc, char ***argv){

    start = clock();

    int fd = shm_open(SHMNAME, O_CREAT | O_RDWR, 0640);


    if(fd==-1){
        error("[OSMPLib.c] Fehler bei shm_open");
    }

    struct stat *shm_stat = calloc(1, sizeof(struct stat));
    if (shm_stat == NULL) {
        error("[OSMPLib.c] Calloc fail");
    }

    if(fstat(fd, shm_stat)!=0){
        error("[OSMPLib.c] fstat fail");
    }

    size_t shm_size = (size_t) shm_stat->st_size;
    free(shm_stat);

    processes = (int) (shm_size - (OSMP_MAX_SLOTS * sizeof(message))- (sizeof(process))) ; //sizeof(process) = emptyslot
    processes /= (int) sizeof(process);

    //Mappe den erzeugten shared memory in den Prozessspeicher
    shm_start = mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    //Fehlerbehandlung für das Mapping
    if (shm_start == MAP_FAILED) {
        printf("line: %d ", __LINE__ -3);
        error("[OSMPLib.c] Fehler beim Mapping");
    }
    OSMP_Rank(&rank);

    return OSMP_SUCCESS;
}

/**
 * Schreibe Anzahl von aktuell präsenten Prozessen in *size aus dem OSMPExecutable
 * @param size
 * @return OSMP_SUCCESS
 */
int OSMP_Size(int *size){
    // & -> Keine Warning mehr: Assignment make Integer from pointer without a cast
    *size = shm_start->processAmount;
    //printf("ProcessAmount: %d",shm_start->processAmount);

    return OSMP_SUCCESS;

}
/**
 * Schreibe Rank (ID) des aktuellen Prozesses in *rank aus dem OSMPExecutable
 * @param rank
 * @return OSMP_SUCCESS
 */
int OSMP_Rank(int *rank){

    //Über alle Prozesse iterieren, mit dem aktuellen die PID matchen
    for (int i = 0; i <= processes; i++) {
        if (shm_start->p[i].pid == getpid()) *rank = i;
    }

    return OSMP_SUCCESS;

}

/**
 *
 * @param buf Nachricht
 * @param count Anzahl der Elemente in der Nachricht
 * @param datatype
 * @param dest Zielprozess
 * @return
 */
int OSMP_Send(const void *buf, int count, OSMP_Datatype datatype, int dest){

    debug("OSMPLib.c: OSMP_Send. Start");

    if(sem_wait(&shm_start->p[rank].freeslots)==-1)
        error("[OSMPLib.c] sem_wait Error");
    if(sem_wait(&shm_start->emptymsg.freeslots)==-1)
        error("[OSMPLib.c] sem_wait Error");
    if(sem_wait(&shm_start->emptymsg.mutex)==-1)
        error("[OSMPLib.c] sem_wait Error");

    int first = shm_start->emptymsg.firstmsg;
    shm_start->emptymsg.firstmsg = shm_start->msg[first].nextmsg;

    if(sem_post(&shm_start->emptymsg.mutex)==-1)
        error("[OSMPLib.c] sem_post Error");

    shm_start->msg[first].len = (size_t) count;
    shm_start->msg[first].type = datatype;
    shm_start->msg[first].src = rank;
    shm_start->msg[first].nextmsg = -1;

    memcpy(shm_start->msg[first].data,buf, (size_t) count);
    printf("sendbuf: %s\n", shm_start->msg[first].data);

    if(sem_wait(&shm_start->p[rank].mutex)==-1)
        error("[OSMPLib.c] sem_wait Error");

    if(shm_start->p[dest].firstmsg == -1){
        shm_start->p[dest].firstmsg = first;
    }else{
        int last = shm_start->p[dest].lastmsg;
        shm_start->msg[last].nextmsg = first;
    }
    shm_start->p[dest].lastmsg = first;

    if(sem_post(&shm_start->p[rank].mutex))
        error("[OSMPLib.c] sem_post Error");

    if(sem_post(&shm_start->p[dest].fullslots))
        error("[OSMPLib.c] sem_post Error");


    return OSMP_SUCCESS;
}

/**
 *
 * @param buf
 * @param count
 * @param datatype
 * @param source
 * @param len
 * @return
 */
int OSMP_Recv(void *buf, int count, OSMP_Datatype datatype,  int *source, int *len){

    debug("OSMPLib.c: OSMP_Recv. Start");


    if(sem_wait(&shm_start->p[rank].fullslots)==-1)
        error("[OSMPLib.c] sem_wait Error");

    if(sem_wait(&shm_start->p[rank].mutex)==-1)
        error("[OSMPLib.c] sem_wait Error");

    int first = shm_start->p[rank].firstmsg;
    shm_start->p[rank].firstmsg = shm_start->msg[first].nextmsg;

    if(sem_post(&shm_start->p[rank].mutex)==-1)
        error("[OSMPLib.c] sem_post Error");

    *source = shm_start->msg[first].src;
    *len = (int) shm_start->msg[first].len;
    memcpy(buf,shm_start->msg[first].data,shm_start->msg[first].len);

    if(sem_wait(&shm_start->emptymsg.mutex)==-1)
        error("[OSMPLib.c] sem_wait Error");

    shm_start->msg[shm_start->emptymsg.lastmsg].nextmsg = first;
    shm_start->emptymsg.lastmsg = first;

    if(sem_post(&shm_start->emptymsg.mutex)==-1)
        error("[OSMPLib.c] sem_post Error");

    if(sem_post(&shm_start->emptymsg.freeslots)==-1)
        error("[OSMPLib.c] sem_post Error");

    if(sem_post(&shm_start->p[rank].freeslots)==-1)
        error("[OSMPLib.c] sem_post Error");


    return OSMP_SUCCESS;

}

/**
 *
 * @return
 */
int OSMP_Finalize(void){

    if(shm_start->p[rank].pid==getpid()){
        shm_start->p[rank].pid = -1;
        shm_start->p[rank].firstmsg = -1;
        shm_start->p[rank].lastmsg = -1;
    }

    int i = 0,rv=0;

    //Prüft, ob in der Menge der Prozesse noch welche vorhanden sind, welche nicht bereits auf -1 gesetzt wurden. Falls ja, break => i!= processes => Kein shm_unlink & Kein emptymsg auf -1
    while(i<processes){
        if(shm_start->p[i].pid!=-1)
            break;
        i++;
    }

    if(i==processes){
        shm_start->emptymsg.firstmsg = -1;
        shm_start->emptymsg.lastmsg = -1;
    }

    rv = munmap(shm_start, (sizeof(process) + OSMP_MAX_SLOTS * sizeof(message) + (unsigned int) processes * sizeof(process)));

    //printf("rechnung: %d", sizeof(process) + OSMP_MAX_SLOTS * sizeof(message) + processes * sizeof(process));

    if(i==processes){
        printf("Unlinking SHM of Rank: %d\n",rank);
        rv=shm_unlink(SHMNAME);
    }
    return rv;
}

void* t_send(void *request){
    IRequest *req = (IRequest *) request;
    OSMP_Send(req->buffer,req->count,req->type,req->dest);
    if(sem_post(&req->mutex)==-1)
        error("[OSMPLib.c] sem_post");

    return 0;
}

void* t_recv(void *request){
    IRequest *req = (IRequest *) request;
    OSMP_Recv(req->buffer,req->count,req->type,req->source,req->length);
    if(sem_post(&req->mutex)==-1)
        error("[OSMPLib.c] sem_post");

    return 0;
}


int OSMP_Isend(const void *buf, int count, OSMP_Datatype datatype, int dest, OSMP_Request request){

    printf("buf: %s\n", (char*)buf);

    debug("OSMPLib.c: OSMP_iSend. Start");


    if(buf==NULL){
        error("[OSMPLib.c] buf null");
    }
    if(request==NULL){
        error("[OSMPLib.c] request null");
    }

    int rv = 0;

    IRequest *req = (IRequest*) request;

    req->buffer = (char *) buf;
    req->type = datatype;
    req->count = count;
    req->dest = dest;

//    memcpy(req->buffer,buf,count);

    pthread_t pthread;

    pthread_create(&pthread,NULL,(*t_send),request);
    pthread_detach(pthread);

    rv = OSMP_SUCCESS;

    return rv;

}

int OSMP_Irecv(void *buf, int count, OSMP_Datatype datatype, int *source, int *len, OSMP_Request request){

    debug("OSMPLib.c: OSMP_iRecv. Start");


    if(buf==NULL){
        error("[OSMPLib.c] buf null");
    }
    if(request==NULL){
        error("[OSMPLib.c] request null");
    }

    IRequest *req = (IRequest *) request;

    req->buffer = buf;
    req->count = count;
    req->type = datatype;
    req->length = len;
    req->source = source;

    //memcpy(req->buffer,buf,(size_t) req->length);

    pthread_t pthread;

    pthread_create(&pthread,NULL,(*t_recv),request);
    pthread_detach(pthread);
    //debug("Length: %d",len);

    return OSMP_SUCCESS;
}

int OSMP_Test(OSMP_Request request, int *flag){

    return OSMP_SUCCESS;

}

int OSMP_Wait (OSMP_Request request){
    debug("OSMPLib.c: Wait. Start");

    if(request==NULL){
        error("[OSMPLib.c] OSMP_Request null");
    }

    IRequest *req = (IRequest*) request;
    if(sem_wait(&req->mutex)){
        error("[OSMPLib.c] sem_wait");
    }
    if(sem_post(&req->mutex)){
        error("[OSMPLib.c] sem_post");
    }

    return OSMP_SUCCESS;
}

int OSMP_CreateRequest(OSMP_Request *request){
    debug("OSMPLib.c: OSMP_CreateRequest");

    if(request==NULL){
        error("[OSMPLib.c] OSMP_Request null");
    }

    *request = calloc(1,sizeof(IRequest));
    if(request==NULL){
        error("[OSMPLib.c] OSMP_Request null");
    }

    IRequest *req = (IRequest*) *request;
    if(sem_init(&req->mutex, 0, 0)){
        error("[OSMPLib.c] OSMP_Request sem_init");
    }

    req->buffer = NULL;
    req->length = NULL;
    req->source = NULL;
    req->dest = 0;
    req->count = 0;
    req->type = NULL;

    *request = req;

    return OSMP_SUCCESS;
}

int OSMP_RemoveRequest(OSMP_Request *request){
    if(request==NULL){
        error("[OSMPLib.c] OSMP_Request null");
    }
    IRequest *req = (IRequest *) request;
    if(sem_destroy(&req->mutex)){
        error("[OSMPLib.c] OSMP_Request sem_destroy");
    }
    free(request); //@TODO *request?
    return OSMP_SUCCESS;
}
