//
// Created by Gurur Öndarö and Thomas Kraaibeek on 24.04.19.
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

#include "../OSMPLib/OSMPLib.h"


#define SHMSIZE 100
#define SHMNAME "/myshm"

#define OSMP_MAX_MESSAGES_PROC   16   // maximale Zahl der Nachrichten pro Prozess
#define OSMP_MAX_SLOTS           256  // maximale Anzahl der Nachrichten, die insgesamt vorhanden sein dürfen
#define OSMP_MAX_PAYLOAD_LENGTH  1024 // maximale Länge der Nutzlast einer Nachricht


/**
 * Startpunkt des OSMPStarters. Richtet den von allen Prozessen genutzten Shared-Memory ein und startet Prozesse.
 * @param argc Anzahl der übergebenen Parameter
 * @param argv Erster Parameter: Anzahl der zu erzeugenden Prozesse. Zweiter Paramter: Auszuführende Datei.
 * @return int Success oder Error
 */
int main(int argc, char **argv);


#endif //OSMP_OSMPSTARTER_H
