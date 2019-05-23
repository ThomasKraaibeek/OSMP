//
// Created by gurur on 24.04.19.
//

#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* for O_* constans*/
#include <sys/types.h>
#include <semaphore.h>

#ifndef OSMP_OSMPLIB_H
#define OSMP_OSMPLIB_H

#define SHMNAME "/myshm"


#define OSMP_SUCCESS 0
#define OSMP_ERROR -1


typedef void* OSMP_Datatype;

typedef void* OSMP_Request;
#define OSMP_MAX_MESSAGES_PROC   16   // maximale Zahl der Nachrichten pro Prozess
#define OSMP_MAX_SLOTS           256  // maximale Anzahl der Nachrichten, die insgesamt vorhanden sein dürfen
#define OSMP_MAX_PAYLOAD_LENGTH  1024 // maximale Länge der Nutzlast einer Nachricht

typedef short int OSMP_SHORT;
typedef int OSMP_INT;
typedef long int OSMP_LONG;
typedef unsigned char OSMP_UNSIGNED_CHAR;
typedef unsigned short int OSMP_UNSIGNED_SHORT;
typedef unsigned int OSMP_UNSIGNED;
typedef unsigned long int OSMP_UNSIGNED_LONG;
typedef float OSMP_FLOAT;
typedef double OSMP_DOUBLE;
typedef char OSMP_BYTE;

typedef struct{
    int src;
    char data[OSMP_MAX_PAYLOAD_LENGTH];
    OSMP_Datatype type;
    size_t len;
    int nextmsg;
    
} message;

typedef struct{
    int firstmsg;
    int lastmsg;
    pid_t pid;
    sem_t freeslots;
    sem_t fullslots;
    sem_t mutex;
} process;

typedef struct{
    int firstempty;
    int lastempty;
    sem_t free_slot;
    sem_t mutex;
} emptyslot;

typedef struct{
    process emptymsg;
    message msg[OSMP_MAX_SLOTS];
    int processAmount;
    process p[];

}shm;


int OSMP_Init(int *argc, char ***argv);
int OSMP_Size(int *size);
int OSMP_Rank(int *rank);
int OSMP_Send(const void *buf, int count, OSMP_Datatype datatype, int dest);
int OSMP_Recv(void *buf, int count, OSMP_Datatype datatype,  int *source, int *len);
int OSMP_Finalize(void);


int OSMP_Isend(const void *buf, int count, OSMP_Datatype datatype, int dest, OSMP_Request request);
int OSMP_Irecv(void *buf, int count, OSMP_Datatype datatype, int *source, int *len, OSMP_Request request);
int OSMP_Test(OSMP_Request request, int *flag);
int OSMP_Wait (OSMP_Request request);
int OSMP_CreateRequest(OSMP_Request *request);
int OSMP_RemoveRequest(OSMP_Request *request);


void error(char* msg, ...);


#endif //OSMP_OSMPLIB_H
