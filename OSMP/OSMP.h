//
// Created by go744355 on 25.04.19.
//

#ifndef OSMP_OSMP_H
#define OSMP_OSMP_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>



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



#define SHMNAME "/myshm"


#define OSMP_SUCCESS 0
#define OSMP_ERROR -1

typedef void* OSMP_Datatype;

typedef void* OSMP_Request; // maximale Zahl der Nachrichten pro Prozess
#define OSMP_MAX_MESSAGES_PROC   16   // maximale Anzahl der Nachrichten, die insgesamt vorhanden sein dürfen
#define OSMP_MAX_SLOTS           256 // maximale Länge der Nutzlast einer Nachricht
#define OSMP_MAX_PAYLOAD_LENGTH  1024

int OSMP_Init(int *argc, char ***argv);
int OSMP_Size(int *size);
int OSMP_Rank(int *rank);
int OSMP_Send(const void *buf, int count, OSMP_Datatype datatype, int dest);
int OSMP_Recv(void *buf, int count, OSMP_Datatype datatype,  int *source, int *len);
int OSMP_Finalize(void);





#endif //OSMP_OSMP_H
