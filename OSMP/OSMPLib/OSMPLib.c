//
// Created by gurur on 24.04.19.
//


#include "OSMPLib.h"
#include <stdio.h>

//SHM Struct
shm* shm_start;

void error(char* msg, ...){
    printf("%s | %s\n", msg, strerror(errno));
    exit(0);
}

int OSMP_Init(int *argc, char ***argv){
    int fd = shm_open(SHMNAME, O_CREAT | O_RDWR, 0640);
    if(fd==-1){
        error("[OSMPLib.c] Fehler bei shm_open");
    }

    //Konfiguriere die Größe des Speichers
    int ftrunc = ftruncate(fd, OSMP_MAX_SLOTS); //TODO OSMP_MAX_SLOTS richtig?
    //Fehlerbehandlung, falls ftruncate nicht funktioniert hat
    if (ftrunc == -1) {
        error("[OSMPLib.c] Fehler bei ftruncate");
    }

    //Mappe den erzeugten shared memory in den Prozessspeicher
    shm_start = mmap(0, OSMP_MAX_SLOTS, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); //TODO Rechte?
    //Fehlerbehandlung für das Mapping
    if (shm_start == MAP_FAILED) {
        error("[OSMPLib.c] Fehler beim Mapping");
    }





    return OSMP_SUCCESS;
}

/**
 * Schreibe Anzahl von aktuell präsenten Prozessen in *size aus dem OSMPExecutable
 * @param size
 * @return OSMP_SUCCESS
 */
int OSMP_Size(int *size){
    // & -> Keine Warning mehr: Assignment make Integer from pointer without a cast
    *size = shm_start->processsAmount;

    return OSMP_SUCCESS;

}
/**
 * Schreibe Rank (ID) des aktuellen Prozesses in *rank aus dem OSMPExecutable
 * @param rank
 * @return OSMP_SUCCESS
 */
int OSMP_Rank(int *rank){

    //Über alle Prozesse iterieren, mit dem aktuellen die PID matchen
    for (int i = 0; i < shm_start->processsAmount; i++) {
        if (shm_start->p[i].pid == getpid()) *rank = i;
    }

    return OSMP_SUCCESS;

}

/**
 *
 * @param buf
 * @param count
 * @param datatype
 * @param dest
 * @return
 */
int OSMP_Send(const void *buf, int count, OSMP_Datatype datatype, int dest){

    printf("Hello World4\n");
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

    shm_unlink(SHMNAME);
    return OSMP_SUCCESS;

}