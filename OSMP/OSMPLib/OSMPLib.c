//
// Created by gurur on 24.04.19.
//

#include "OSMPLib.h"
#include <stdio.h>

//SHM Struct
shm* shm_start;
sem_t mutex;
sem_t emptyslots;

int processes = 0, rank = 0;

void error(char* msg, ...){
    printf("%s | %s\n", msg, strerror(errno));
    exit(0);
}

/**
 * Erzeugen des Shared Memory, zuschneiden und mappen.
 * @param 
 * @return 1=success, 0=fail
 */
int OSMP_Init(int *argc, char ***argv){

    //printf("anfang init. pamount: %d\n", argv[2]);

    int fd = shm_open(SHMNAME, O_CREAT | O_RDWR, 0640);

    //printf("fd: %d\n", fd);

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


    size_t shm_size = shm_stat->st_size;

    //printf("shm_size: %ld\n", shm_size);
    free(shm_stat);

    processes = shm_size - (OSMP_MAX_SLOTS * sizeof(message))- (sizeof(process)) ; //sizeof(process) = emptyslot
    processes /= sizeof(process);


    //printf("shmzise: %d, MaxSlots: %d, sizeof(message): %d, sizeof(process): %d, sizeof(emptyslot): %d, processes: %d ",shm_size, OSMP_MAX_SLOTS, sizeof(message),
    //       sizeof(process), sizeof(emptyslot), processes);
    printf("Processes: %d", processes);

    //Mappe den erzeugten shared memory in den Prozessspeicher
    shm_start = mmap(NULL, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); //TODO Rechte?
    //Fehlerbehandlung für das Mapping
    if (shm_start == MAP_FAILED) {
        printf("line: %d ", __LINE__ -3);
        error("[OSMPLib.c] Fehler beim Mapping");
    }
    OSMP_Rank(&rank);

    //printf("Printing len: %ld",shm_start->msg->len);

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

    //first=shm_start->emptymsg
    int first = shm_start->emptymsg.firstmsg;
    shm_start->emptymsg.firstmsg = shm_start->msg[first].nextmsg;

    shm_start->msg[first].len = count;
    shm_start->msg[first].type = datatype;
    shm_start->msg[first].src = rank;
    shm_start->msg[first].nextmsg = -1;

    memcpy(shm_start->msg[first].data,buf,count);

    if(shm_start->p[dest].firstmsg == -1){
        shm_start->p[dest].firstmsg = first;
    }else{
        int last = shm_start->p[dest].lastmsg;
        shm_start->msg[last].nextmsg = first;
    }
    shm_start->p[dest].lastmsg = first;

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

    printf("Hello World5\n");
    return OSMP_SUCCESS;

}

/**
 *
 * @return
 */
int OSMP_Finalize(void){

    if(shm_start->p[rank].pid==getpid()){
        //@TODO Semaphoren setzen
        shm_start->p[rank].pid = -1;
        shm_start->p[rank].firstmsg = -1;
        shm_start->p[rank].lastmsg = -1;
    }

    int i = 0;

    //Prüft, ob in der Menge der Prozesse noch welche vorhanden sind, welche nicht bereits auf -1 gesetzt wurden. Falls ja, break => i!= processes => Kein shm_unlink & Kein emptymsg auf -1
    while(i<processes){
        if(shm_start->p[i].pid!=-1)
            break;
        i++;
    }
    printf("\ti: %d\t rank: %d\n",i,rank);

    if(i==processes){
        //@TODO Semaphoren setzen
        shm_start->emptymsg.firstmsg = -1;
        shm_start->emptymsg.lastmsg = -1;
    }

    int result = munmap(shm_start, sizeof(emptyslot) + OSMP_MAX_SLOTS * sizeof(message) + processes * sizeof(process));

    if(i==processes){
        printf("Unlinking SHM of Rank: %d\n",rank);
        shm_start = shm_unlink(SHMNAME);
    }
    return OSMP_SUCCESS;
}
