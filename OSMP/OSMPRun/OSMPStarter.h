//
// Created by gurur on 24.04.19.
//

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

#ifndef OSMP_OSMPSTARTER_H
#define OSMP_OSMPSTARTER_H


#define SHMSIZE 100

char* itos(int value);

#endif //OSMP_OSMPSTARTER_H
