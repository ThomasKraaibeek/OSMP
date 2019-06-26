//
// Created by Gurur Öndarö and Thomas Kraaibeek on 24.04.19.
//

#include "OSMPStarter.h"

shm *shm_start;

int slots_init(unsigned int processAmount) {
    //EMPTYMESSAGE, setze first und last als INIT
    shm_start->emptymsg.firstmsg = 0;
    shm_start->emptymsg.lastmsg = OSMP_MAX_SLOTS;

    //sem_init(sem,!=0: zwischen prozessen,initwert)
    if (sem_init(&shm_start->emptymsg.freeslots, 1, OSMP_MAX_SLOTS)) {
        error("[OSMPRun.c] sem_init Fehler");
        return OSMP_ERROR;
    }

    if (sem_init(&shm_start->emptymsg.mutex, 1, 1)) {
        error("[OSMPRun.c] sem_init Fehler");
        return OSMP_ERROR;
    }


    //struct process | Array aller Prozesse als Structs | Init all -1
    for (int i = 0; i < processAmount; i++) {
        shm_start->p[i].firstmsg = -1;
        shm_start->p[i].lastmsg = -1;
        shm_start->p[i].pid = -2;


        if (sem_init(&shm_start->p[i].freeslots, 1, OSMP_MAX_MESSAGES_PROC)) {
            error("[OSMPRun.c] sem_init Fehler");
            return OSMP_ERROR;
        }

        if (sem_init(&shm_start->p[i].fullslots, 1, 0)) {
            error("[OSMPRun.c] sem_init Fehler");
            return OSMP_ERROR;
        }

        if (sem_init(&shm_start->p[i].mutex, 1, 1)) { //Mutex init value 1, siehe Verbraucher Erzeuger Problem VL
            error("[OSMPRun.c] sem_init Fehler");
            return OSMP_ERROR;
        }
    }

    for (int i = 0; i < OSMP_MAX_SLOTS; i++) {
        shm_start->msg[i].src = -1;
        shm_start->msg[i].len = 0; // besser 0 als -1. sonst inkompatibel mit dem Typ size_t
        memcpy(shm_start->msg[i].data, "\0", 1);
        //shm_start->msg[i].type = OSMP_INT;

        if (i == OSMP_MAX_SLOTS - 1) {
            shm_start->msg[i].nextmsg = -1;
        } else {
            shm_start->msg[i].nextmsg = i + 1;
        }
        //printf("shm_start->msg[%d].next: %d\n", i, shm_start->msg[i].nextmsg);
    }

    return OSMP_SUCCESS;
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Syntax: osmprun [int] [./name]    , [int]: Zahl der Prozesse, die erzeugt werden sollen, [./name]: Name des Executables, welches ausgeführt werden soll\n");
        return 1;
    }

    //konvertiere string in int
    int inputprocessAmount = atoi(argv[1]);
    if (inputprocessAmount > 150 || inputprocessAmount < 0) {
        error("[OSMPStarter.c] Anzahl der Prozesse muss zwischen 1 und 300 liegen\n");
        return OSMP_ERROR;
    }

    //Sonst warnung wegen Vorzeichen
    unsigned int processAmount = (unsigned int) inputprocessAmount;

    //shmname mit PID verbinden um mehrere Starter mit eigenen Kindern gleichzeitig in verschiedneen SHM's arbeiten zu lassen
    pid_t pid = getpid();
    char* itospid = itos(pid);

    if(itospid == NULL){
        error("[OSMPStarter.c] itos failed");
        return OSMP_ERROR;
    }

    char* shmname = calloc(1, strlen("myshm_") + strlen(itospid) + 1);
    if(snprintf(shmname, strlen("myshm_") + strlen(itospid) + 1, "myshm_%s", itospid) < 0){
        error("[OSMPStarter.c] snprintf shm_name");
        free(itospid);
        free(shmname);
        return OSMP_ERROR;
    }
    //printf("Sizeof: %d, strlen: %d, calloc: %d, pid: %d, shmname> %s",sizeof(shmname),strlen(shmname),strlen("myshm_")+sizeof(pid_t),strlen(itospid), shmname);
    free(itospid);

    //Erzeuge shared memory. 2. Argument: Zugriffsrechte der Prozesse, 3. Argument: Zugriffsrechte der Benutzer
    int fd = shm_open(shmname, O_CREAT | O_RDWR, 0640);
    if (fd == -1) {
        error("[OSMPStarter.c] Fehler bei shm_open");
        free(shmname);
        return OSMP_ERROR;
    }

    //Konfiguriere die Größe des Speichers
    int ftrunc = ftruncate(fd, (unsigned int) (sizeof(process) + OSMP_MAX_SLOTS * sizeof(message) + processAmount * sizeof(process)));
    //Fehlerbehandlung, falls ftruncate nicht funktioniert hat
    if (ftrunc == -1) {
        error("[OSMPStarter.c] Fehler bei ftruncate");
        free(shmname);
        return OSMP_ERROR;
    }

    //printf("selbst ausgerechnet: %d\n", (unsigned int)(sizeof(emptyslot) + OSMP_MAX_SLOTS * sizeof(message) + processAmount * sizeof(process)));

    //Mappe den erzeugten shared memory in den Prozessspeicher
    shm_start = mmap(0, (unsigned int) (sizeof(process) + OSMP_MAX_SLOTS * sizeof(message) + processAmount * sizeof(process)), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    //Fehlerbehandlung für das Mapping
    if (shm_start == MAP_FAILED) {
        error("[OSMPStarter.c] Fehler beim Mapping");
        free(shmname);
        return OSMP_ERROR;
    }

    if(slots_init(processAmount) == OSMP_ERROR){
        error("[OSMPStarter.c] Fehler beim slots_init");
        free(shmname);
        return OSMP_ERROR;
    }

    //Forke "processAmount" mal
    for (int i = 0; i < processAmount; i++) {
        shm_start->processAmount++;
        pid = fork();

        //Fehlerbehandlung
        if (pid < 0) {
            error("Fehler bei der Prozessaufteilung");
            free(shmname);
            return OSMP_ERROR;
        }

        //Child
        if (pid == 0) {
            shm_start->p[i].pid = getpid();

            //Name der auszuführenden Datei und alle Argumente
            //execlp(argv[2], argv[2], argv[3], NULL);
            execvp(argv[2],argv);
            //execlp(argv[2], *argv);
            free(shmname);
            debug("Fehler bei execvp");

            return OSMP_ERROR;
        }
        //Parent
        else if (pid > 0) {
            if (i + 1 < processAmount) {
                continue;
            }
        }
    }

    //Warte auf alle einzelnen Kinder. -1 = Warte nicht hintereinander sondern parallel
    for (int i = 0; i < processAmount; i++) {
        waitpid(-1, NULL, 0);
        //shm_start->processAmount--;
    }


    if(shm_unlink(shmname) == OSMP_ERROR){
        error("[OSMPLib.c] Unlinking SHM failed");
        free(shmname);
        return OSMP_ERROR;
    }
    //shm_start = NULL;
    debug("[OSMPStarter.c] Unlinking SHM successful");

    free(shmname);
    return 0;
}

