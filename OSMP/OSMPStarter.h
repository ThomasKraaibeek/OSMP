//
// Created by gurur on 24.04.19.
//

#ifndef OSMP_OSMPSTARTER_H
#define OSMP_OSMPSTARTER_H


#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>
#include <sys/types.h>

#define SHMSIZE 100
#define SHMNAME "/myshm"

char* itos(int value);

#endif //OSMP_OSMPSTARTER_H
