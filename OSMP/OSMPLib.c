//
// Created by gurur on 24.04.19.
//


#include "OSMPLib.h"
#include <stdio.h>

int OSMP_Init(int *argc, char ***argv){

    printf("Hello World1\n");
}


int OSMP_Size(int *size){

    printf("Hello World1\n");
}


int OSMP_Rank(int *rank){

    printf("Hello World1\n");
}


int OSMP_Send(const void *buf, int count, OSMP_Datatype datatype, int dest){

    printf("Hello World1\n");
}


int OSMP_Recv(void *buf, int count, OSMP_Datatype datatype,  int *source, int *len){

    printf("Hello World1\n");
}


int OSMP_Finalize(void){

    printf("Hello World1\n");
}