//
// Created by Gurur Öndarö and Thomas Kraaibeek on 24.04.19.
//

#include "OSMPLib.h"
#include <stdio.h>
#include <stdarg.h>
#include <time.h>

/**
 * Aufbau der Request-Struktur. Für Nutzer opaque, deswegen nicht in .h
 */
typedef struct {
    char* buffer;
    int* length;
    int* source;
    OSMP_Datatype type;
    int dest;
    int count;
    sem_t mutex;
} IRequest;

//SHM Struct
shm* shm_start;

// Name des shared memory. Wird nach Starterprozesspid benannt
char* shmname;
int processes = 0, rank = 0;

void debug(char* message, ...) {
#ifdef DEBUG
	va_list args;
	va_start(args, message);
    printf("[PROC-%03d] ", rank);
	vprintf(message, args);
	printf("\n");
	fflush(stdout);
	va_end(args);
#endif
}

void error(char* msg, ...){
    debug("ERROR: %s", msg);
}

int OSMP_Init(int *argc, char ***argv){
    debug("[OSMPLib.c] OSMP_Init - Start");

    // Hole parent pid und ermittle damit den Namen des zu benutzenden shared memorys
    pid_t ppid = getppid();
    char* itospid = itos(ppid);

    if(itospid == NULL){
        error("[OSMPLib.c] OSMP_Init itos failed");
        return OSMP_ERROR;
    }

    shmname = calloc(1, strlen("myshm_") + strlen(itospid) + 1);

    if(shmname == NULL){
        error("[OSMPLib.c] OSMP_Init calloc shm_name");
        free(itospid);
        return OSMP_ERROR;
    }

    // Baue den Namen des shared memorys zusammen
    if(sprintf(shmname,"myshm_%s",itospid) < 0) {
        error("[OSMPLib.c] OSMP_Init sprintf shm_name");
        free(itospid);
        free(shmname);
        return OSMP_ERROR;
    }
    //printf("ppid: %d, name: %s\n", ppid, shmname);

    free(itospid);

    int fd = shm_open(shmname, O_CREAT | O_RDWR, 0640);

    free(shmname);

    if(fd == -1){
        error("[OSMPLib.c] OSMP_Init Fehler bei shm_open");
        return OSMP_ERROR;
    }

    struct stat *shm_stat = calloc(1, sizeof(struct stat));
    if (shm_stat == NULL) {
        error("[OSMPLib.c] OSMP_Init Calloc fail");
        return OSMP_ERROR;
    }

    if(fstat(fd, shm_stat) != 0){
        error("[OSMPLib.c] OSMP_Init fstat fail");
        free(shm_stat);
        return OSMP_ERROR;
    }

    // Groesse des shared memorys, um Anzahl der Prozesse zu berechnen
    size_t shm_size = (size_t) shm_stat->st_size;
    free(shm_stat);

    processes = (int) (shm_size - (OSMP_MAX_SLOTS * sizeof(message)) - (sizeof(process))) ; //sizeof(process) = emptyslot
    processes /= (int) sizeof(process);

    //Mappe den erzeugten shared memory in den Prozessspeicher
    shm_start = mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    //Fehlerbehandlung für das Mapping
    if (shm_start == MAP_FAILED) {
        error("[OSMPLib.c] OSMP_Init Fehler beim Mapping");
        return OSMP_ERROR;
    }

    OSMP_Rank(&rank);
    debug("[OSMPLib.c] OSMP_Init - End");
    return OSMP_SUCCESS;
}


int OSMP_DataSize(OSMP_Datatype datatype){

    if(datatype == 0) return sizeof(int);
    else if(datatype == 1) return sizeof(short);
    else if(datatype == 2) return sizeof(long);
    else if(datatype == 3) return sizeof(char);
    else if(datatype == 4) return sizeof(unsigned char);
    else if(datatype == 5) return sizeof(unsigned short);
    else if(datatype == 6) return sizeof(unsigned);
    else if(datatype == 7) return sizeof(float);
    else if(datatype == 8) return sizeof(double);

    return OSMP_ERROR;
}


int OSMP_Size(int *size){
    debug("[OSMPLib.c] OSMP_Size - Start");

    if(shm_start == NULL){
        error("[OSMPLib.c] OSMP_Size SHM not initialized");
        return OSMP_ERROR;
    }

    *size = shm_start->processAmount;
    debug("[OSMPLib.c] OSMP_Size - End");
    return OSMP_SUCCESS;
}


int OSMP_Rank(int *rank){

    //debug("[OSMPLib.c] OSMP_Rank - Start");

    if(shm_start == NULL){
        error("[OSMPLib.c] OSMP_Rank SHM not initialized");
        return OSMP_ERROR;
    }

    //Über alle Prozesse iterieren, mit dem aktuellen die PID matchen
    for (int i = 0; i <= processes; i++) {
        if (shm_start->p[i].pid == getpid()) *rank = i;
    }

    //debug("[OSMPLib.c] OSMP_Rank - End");
    return OSMP_SUCCESS;
}


int OSMP_Send(const void *buf, int count, OSMP_Datatype datatype, int dest) {

    debug("[OSMPLib.c] OSMP_Send - Start");

    if (shm_start == NULL) {
        error("[OSMPLib.c] OSMP_Send SHM not initialized");
        return OSMP_ERROR;
    }

    if(buf == NULL){
        error("[OSMPLib.c] OSMP_Send buf null");
        return OSMP_ERROR;
    }
    if(dest >= processes){
        debug("[OSMPLib.c] OSMP_Send Destination unreachable. %d not a valid OSMP_Process no.",dest);
        return OSMP_ERROR;
    }
 
    if (sem_wait(&shm_start->p[dest].freeslots) == -1) {
        error("[OSMPLib.c] OSMP_Send sem_wait Error");
        return OSMP_ERROR;
    }
    
    if (sem_wait(&shm_start->emptymsg.freeslots) == -1){
        error("[OSMPLib.c] OSMP_Send sem_wait Error");
        return OSMP_ERROR;
    }
    if(sem_wait(&shm_start->emptymsg.mutex) == -1) {
        error("[OSMPLib.c] OSMP_Send sem_wait Error");
        return OSMP_ERROR;
    }

    int first = shm_start->emptymsg.firstmsg;

    //Falls erste und letzte leere nachricht die gleiche sind -> es also nur noch eine leere msg gibt
    if(shm_start->emptymsg.firstmsg == shm_start->emptymsg.lastmsg){
        shm_start->emptymsg.firstmsg = -1;
        shm_start->emptymsg.lastmsg = -1;
    }
    else{ //Normalfall
        shm_start->emptymsg.firstmsg = shm_start->msg[first].nextmsg;
    }
    
    if(sem_post(&shm_start->emptymsg.mutex) == -1) {
        error("[OSMPLib.c] OSMP_Send sem_post Error");
        return OSMP_ERROR;
    }

    // Berechne Groesse des Datentypes
    int datasize = OSMP_DataSize(datatype);
    if(datasize==-1){
        error("[OSMPLib.c] OSMP_Send OSMP_DataSize returned -1");
        return OSMP_ERROR;
    }
    
    size_t actualLen = (size_t) (count*datasize);
    //Länge größer als maximal erlaubte Länge? Info an den Nutzer, dass Nachricht abgeschnitten wird.
    if((count*datasize) > OSMP_MAX_PAYLOAD_LENGTH){
        debug("[osmplib.c] OSMP_Send data bigger than OSMP_MAX_PAYLOAD_LENGTH. Message len: %d bytes. Cutting off after %d bytes.",(count*datasize),OSMP_MAX_PAYLOAD_LENGTH);
        actualLen = OSMP_MAX_PAYLOAD_LENGTH;
    } 

    shm_start->msg[first].len = actualLen;
    shm_start->msg[first].type = datatype;
    shm_start->msg[first].src = rank;
    shm_start->msg[first].nextmsg = -1;


    memcpy(shm_start->msg[first].data, buf, actualLen);

    if(sem_wait(&shm_start->p[dest].mutex) == -1) {
        error("[OSMPLib.c] OSMP_Send sem_wait Error");
        return OSMP_ERROR;
    }

    //Erneut Unterscheidung erste leere nachricht die letzte?
    if(shm_start->p[dest].firstmsg == -1){
        shm_start->p[dest].firstmsg = first;
    }else{
        int last = shm_start->p[dest].lastmsg;
        shm_start->msg[last].nextmsg = first;
    }

    shm_start->p[dest].lastmsg = first;

    if(sem_post(&shm_start->p[dest].mutex)) {
        error("[OSMPLib.c] OSMP_Send sem_post Error");
        return OSMP_ERROR;
    }

    if(sem_post(&shm_start->p[dest].fullslots)) {
        error("[OSMPLib.c] OSMP_Send sem_post Error");
        return OSMP_ERROR;
    }

    debug("[OSMPLib.c] OSMP_Send - End");
    return OSMP_SUCCESS;
}

int OSMP_Recv(void *buf, int count, OSMP_Datatype datatype,  int *source, int *len){

    debug("[OSMPLib.c] OSMP_Recv - Start");

    if(shm_start == NULL){
        error("[OSMPLib.c] OSMP_Recv SHM not initialized");
        return OSMP_ERROR;
    }

    if(buf == NULL){
        error("[OSMPLib.c] OSMP_Recv buf null");
        return OSMP_ERROR;
    }

    if(sem_wait(&shm_start->p[rank].fullslots) == -1) {
        error("[OSMPLib.c] OSMP_Recv sem_wait Error");
        return OSMP_ERROR;
    }

    if(sem_wait(&shm_start->p[rank].mutex) == -1) {
        error("[OSMPLib.c] OSMP_Recv sem_wait Error");
        return OSMP_ERROR;
    }

    int first = shm_start->p[rank].firstmsg;
    shm_start->p[rank].firstmsg = shm_start->msg[first].nextmsg;

    if(sem_post(&shm_start->p[rank].mutex) == -1) {
        error("[OSMPLib.c] OSMP_Recv sem_post Error");
        return OSMP_ERROR;
    }

    *source = shm_start->msg[first].src;
    *len = (int) shm_start->msg[first].len;
    int datasize = OSMP_DataSize(datatype);
    if(datasize==-1){
        error("[OSMPLib.c] OSMP_Send OSMP_DataSize returned -1");
        return OSMP_ERROR;
    }
    //Länge größer als maximal erlaubte Länge? Info an den Nutzer, dass Nachricht abgeschnitten wird.
    if(*len>OSMP_MAX_PAYLOAD_LENGTH ||*len > count*datatype ){
        debug("[osmplib.c] OSMP_Recv data bigger than OSMP_MAX_PAYLOAD_LENGTH OR accepted size of reciever (%d). Cutting off. Message len: %d bytes", count*datasize,*len);
        *len = ((count*datasize)<OSMP_MAX_PAYLOAD_LENGTH) ? (count*datasize) : OSMP_MAX_PAYLOAD_LENGTH;
    }

    memcpy(buf, shm_start->msg[first].data, (size_t)*len);

    if(sem_wait(&shm_start->emptymsg.mutex)==-1) {
        error("[OSMPLib.c] OSMP_Recv sem_wait Error");
        return OSMP_ERROR;
    }

    shm_start->msg[shm_start->emptymsg.lastmsg].nextmsg = first;
    shm_start->emptymsg.lastmsg = first;

    if(sem_post(&shm_start->emptymsg.mutex)==-1) {
        error("[OSMPLib.c] OSMP_Recv sem_post Error");
        return OSMP_ERROR;
    }

    if(sem_post(&shm_start->emptymsg.freeslots)==-1) {
        error("[OSMPLib.c] OSMP_Recv sem_post Error");
        return OSMP_ERROR;
    }

    if(sem_post(&shm_start->p[rank].freeslots)==-1) {
        error("[OSMPLib.c] OSMP_Recv sem_post Error");
        return OSMP_ERROR;
    }

    debug("[OSMPLib.c] OSMP_Recv - End");
    return OSMP_SUCCESS;
}

int OSMP_Finalize(void){

    debug("[OSMPLib.c] OSMP_Finalize - Start");

    if(shm_start == NULL){
        error("[OSMPLib.c] OSMP_Finalize SHM not initialized");
        return OSMP_ERROR;
    }

    if(shm_start->p[rank].pid == getpid()){
        shm_start->p[rank].pid = -1;
        shm_start->p[rank].firstmsg = -1;
        shm_start->p[rank].lastmsg = -1;
    }

    int i = 0;

    //Prüft, ob in der Menge der Prozesse noch welche vorhanden sind, welche nicht bereits auf -1 gesetzt wurden. Falls ja, break => i!= processes => Kein shm_unlink & Kein emptymsg auf -1
    while(i < processes){
        if(shm_start->p[i].pid != -1)
            break;
        i++;
    }

    if(i == processes){
        shm_start->emptymsg.firstmsg = -1;
        shm_start->emptymsg.lastmsg = -1;
    }

    if(munmap(shm_start, (sizeof(process) + OSMP_MAX_SLOTS * sizeof(message) + (unsigned int) processes * sizeof(process))) == OSMP_ERROR){
        error("[OSMPLib.c] OSMP_Finalize munmap failed");
        return OSMP_ERROR;
    }

    //printf("rechnung: %d", sizeof(process) + OSMP_MAX_SLOTS * sizeof(message) + processes * sizeof(process));

    if(i==processes){
        debug("[OSMPLib.c] OSMP_Finalize NULLing SHM with process of Rank: %d\n",rank);
        shm_start=NULL;    
    }
    debug("[OSMPLib.c] OSMP_Finalize - End");
    return OSMP_SUCCESS;
}

void* t_send(void *request){
    IRequest *req = (IRequest *) request;
    OSMP_Send(req->buffer,req->count,req->type,req->dest);
    if(sem_post(&req->mutex) == -1) {
        error("[OSMPLib.c] t_send sem_post");
        return 0;
    }
    return 0;
}

void* t_recv(void *request){
    IRequest *req = (IRequest *) request;
    OSMP_Recv(req->buffer,req->count,req->type,req->source,req->length);
    if(sem_post(&req->mutex) == -1) {
        error("[OSMPLib.c] t_recv sem_post");
        return 0;
    }
    return 0;
}


int OSMP_Isend(const void *buf, int count, OSMP_Datatype datatype, int dest, OSMP_Request request){

    debug("[OSMPLib.c] OSMP_iSend - Start");

    if(shm_start == NULL){
        error("[OSMPLib.c] OSMP_iSend SHM not initialized");
    }

    if(buf == NULL){
        error("[OSMPLib.c] OSMP_iSend buf null");
        return OSMP_ERROR;
    }
    if(request == NULL){
        error("[OSMPLib.c] OSMP_iSend request null");
        return OSMP_ERROR;
    }

    if(dest >= processes){
        debug("[OSMPLib.c] OSMP_Send Destination unreachable. %d not a valid OSMP_Process no.",dest);
        return OSMP_ERROR;
    }

    IRequest *req = (IRequest*) request;
    req->buffer = (char *) buf;
    req->type = datatype;
    req->count = count;
    req->dest = dest;

    fflush(stdout);
    if(sem_init(&req->mutex, 0, 0)){
        error("[OSMPLib.c] OSMP_iSend OSMP_Request sem_init");
        return OSMP_ERROR;
    }

    pthread_t pthread;

    if(pthread_create(&pthread,NULL,(*t_send),request) != OSMP_SUCCESS){
        error("[OSMPLib.c] OSMP_iSend pthread_create");
        return OSMP_ERROR;
    }

    if(pthread_detach(pthread) != OSMP_SUCCESS){
        error("[OSMPLib.c] OSMP_iSend pthread_detach");
        return OSMP_ERROR;
    }

    debug("[OSMPLib.c] OSMP_iSend - End");
    return OSMP_SUCCESS;

}

int OSMP_Irecv(void *buf, int count, OSMP_Datatype datatype, int *source, int *len, OSMP_Request request){

    debug("[OSMPLib.c] OSMP_iRecv - Start");

    if(shm_start == NULL){
        error("[OSMPLib.c] OSMP_IRecv SHM not initialized");
        return OSMP_ERROR;
    }

    if(buf == NULL){
        error("[OSMPLib.c] OSMP_IRecv buf null");
        return OSMP_ERROR;
    }
    if(request == NULL){
        error("[OSMPLib.c] OSMP_IRecv request null");
        return OSMP_ERROR;
    }

    IRequest *req = (IRequest *) request;

    req->buffer = buf;
    req->count = count;
    req->type = datatype;
    req->length = len;
    req->source = source;

    if(sem_init(&req->mutex, 0, 0)){
        error("[OSMPLib.c] OSMP_IRecv OSMP_Request sem_init");
        return OSMP_ERROR;
    }


    pthread_t pthread;

    if(pthread_create(&pthread,NULL,(*t_recv),request) != OSMP_SUCCESS){
        error("[OSMPLib.c] OSMP_iRecv pthread_create");
        return OSMP_ERROR;
    }

    if(pthread_detach(pthread) != OSMP_SUCCESS){
        error("[OSMPLib.c] OSMP_iRecv pthread_detach");
        return OSMP_ERROR;
    }
    
    debug("[OSMPLib.c] OSMP_iRecv - End");
    return OSMP_SUCCESS;
}

int OSMP_Test(OSMP_Request request, int *flag){

    debug("[OSMPLib.c] OSMP_Test - Start");

    if(shm_start == NULL){
        error("[OSMPLib.c] SHM not initialized");
        return OSMP_ERROR;
    }

    IRequest *req = (IRequest*) request;
    if(sem_getvalue(&req->mutex, flag)) {
        error("[OSMPLib.c] request running in sem_getvalue");
        return OSMP_ERROR;
    }

    debug("[OSMPLib.c] OSMP_Test - End");
    return OSMP_SUCCESS;

}

int OSMP_Wait (OSMP_Request request){

    debug("[OSMPLib.c] OSMP_Wait - Start");

    if(shm_start == NULL){
        error("[OSMPLib.c] SHM not initialized");
        return OSMP_ERROR;
    }

    if(request == NULL){
        error("[OSMPLib.c] OSMP_Request null");
        return OSMP_ERROR;
    }

    IRequest *req = (IRequest*) request;
    if(sem_wait(&req->mutex)){
        error("[OSMPLib.c] sem_wait");
        return OSMP_ERROR;
    }

    if(sem_post(&req->mutex)){
        error("[OSMPLib.c] sem_post");
        return OSMP_ERROR;
    }


    debug("[OSMPLib.c] OSMP_Wait - End");
    return OSMP_SUCCESS;
}

int OSMP_CreateRequest(OSMP_Request *request){
    debug("[OSMPLib.c] OSMP_CreateRequest - Start");

    if(shm_start == NULL){
        error("[OSMPLib.c] SHM not initialized");
        return OSMP_ERROR;
    }

    if(request == NULL){
        error("[OSMPLib.c] OSMP_Request null");
        return OSMP_ERROR;
    }

    *request = calloc(1, sizeof(IRequest));
    if(*request == NULL){
        error("[OSMPLib.c] OSMP_Request null");
        return OSMP_ERROR;
    }

    IRequest *req = (IRequest*) *request;

    req->buffer = NULL;
    req->length = NULL;
    req->source = NULL;
    req->dest = 0;
    req->count = 0;
    req->type = -1;

    *request = req;

    debug("[OSMPLib.c] OSMP_CreateRequest - End");
    return OSMP_SUCCESS;
}

int OSMP_RemoveRequest(OSMP_Request *request){

    debug("[OSMPLib.c] OSMP_RemoveRequest - Start");

    if(shm_start == NULL){
        error("[OSMPLib.c] SHM not initialized");
        return OSMP_ERROR;
    }

    if(request == NULL){
        error("[OSMPLib.c] OSMP_Request null");
        return OSMP_ERROR;
    }

    IRequest *req = (IRequest *) *request;
    int flag;

    if(sem_getvalue(&req->mutex, &flag)) {
        error("[OSMPLib.c] OSMP_RemoveRequest in sem_getvalue");
        return OSMP_ERROR;
    }

    if(flag==0){
        debug("[OSMPLib.c] OSMP_RemoveRequest req still in use. Returning..");
        return OSMP_ERROR;
    }

    if(sem_destroy(&req->mutex)){
        error("[OSMPLib.c] OSMP_RemoveRequest sem_destroy");
        return OSMP_ERROR;
    }

    free(req);
    //free(request);
    debug("[OSMPLib.c] OSMP_RemoveRequest - End");
    return OSMP_SUCCESS;
}

char* itos(int value) {
    char *string = malloc(12);

    if(string == NULL){
        error("[OSMPLib.c] itos malloc failed");
        return NULL;
    }

    if(sprintf(string, "%d", value) < 0){
        free(string);
        error("[OSMPLib.c] itos sprintf");
        return NULL;
    }
    return string;
}
