//
// Created by gurur on 24.04.19.
//


#include "OSMPLib.h"
#include <stdio.h>

int counter = 0;

int OSMP_Init(int *argc, char ***argv){
    int fd = shm_open(SHMNAME, O_CREAT | O_RDWR, 0640);
    if(fd==-1){
        printf("Fehler bei shm_open %s\n", strerror(errno));
        return OSMP_ERROR;
    }

    //Konfiguriere die Größe des Speichers
    int ftrunc = ftruncate(fd, OSMP_MAX_SLOTS); //TODO OSMP_MAX_SLOTS richtig?
    //Fehlerbehandlung, falls ftruncate nicht funktioniert hat
    if (ftrunc == -1) {
        printf("Fehler bei ftruncate %s\n", strerror(errno));
        return OSMP_ERROR;
    }

    //Mappe den erzeugten shared memory in den Prozessspeicher
    void *map = mmap(0, OSMP_MAX_SLOTS, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); //TODO Rechte?
    //Fehlerbehandlung für das Mapping
    if (map == MAP_FAILED) {
        printf("Fehler beim Mapping: %s\n", strerror(errno));
        return OSMP_ERROR;
    }


    counter++;
    return OSMP_SUCCESS;
}

//TODO ausimplementieren
int OSMP_Size(int *size){
    printf("TEST");
    *size = counter;

    return OSMP_SUCCESS;

}

//TODO ausimplementieren
int OSMP_Rank(int *rank){

    printf("Hello World3\n");
    return OSMP_SUCCESS;

}


int OSMP_Send(const void *buf, int count, OSMP_Datatype datatype, int dest){

    printf("Hello World4\n");
    return OSMP_SUCCESS;

}


int OSMP_Recv(void *buf, int count, OSMP_Datatype datatype,  int *source, int *len){

    printf("Hello World5\n");
    return OSMP_SUCCESS;

}


int OSMP_Finalize(void){

    printf("Hello World6\n");
    return OSMP_SUCCESS;

}