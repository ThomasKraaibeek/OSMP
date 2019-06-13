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
        shm_start->msg[i].len = 0; // besser 0 als -1? sonst inkompatibel mit dem Typ size_t
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
    if (inputprocessAmount >= 10000 || inputprocessAmount < 0) {
        error("[OSMPStarter.c] Anzahl der Prozesse muss zwischen 1 und %ld liegen\n", sizeof(int) / 2);
        return OSMP_ERROR;
    }

    //Sonst warnung wegen Vorzeichen
    unsigned int processAmount = (unsigned int) inputprocessAmount;

    pid_t pid;

    //Erzeuge shared memory. 2. Argument: Zugriffsrechte der Prozesse, 3. Argument: Zugriffsrechte der Benutzer
    int fd = shm_open(SHMNAME, O_CREAT | O_RDWR, 0640);
    if (fd == -1) {
        error("[OSMPStarter.c] Fehler bei shm_open");
        return OSMP_ERROR;
    }

    //Konfiguriere die Größe des Speichers
    int ftrunc = ftruncate(fd, (unsigned int) (sizeof(process) + OSMP_MAX_SLOTS * sizeof(message) + processAmount * sizeof(process)));
    //Fehlerbehandlung, falls ftruncate nicht funktioniert hat
    if (ftrunc == -1) {
        error("[OSMPStarter.c] Fehler bei ftruncate");
        return OSMP_ERROR;
    }

    //printf("selbst ausgerechnet: %d\n", (unsigned int)(sizeof(emptyslot) + OSMP_MAX_SLOTS * sizeof(message) + processAmount * sizeof(process)));

    //Mappe den erzeugten shared memory in den Prozessspeicher
    shm_start = mmap(0, (unsigned int) (sizeof(process) + OSMP_MAX_SLOTS * sizeof(message) + processAmount * sizeof(process)), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); //TODO Rechte?
    //Fehlerbehandlung für das Mapping
    if (shm_start == MAP_FAILED) {
        error("[OSMPStarter.c] Fehler beim Mapping");
        return OSMP_ERROR;
    }

    slots_init(processAmount);
    //Forke "processAmount" mal
    for (int i = 0; i < processAmount; i++) {
        shm_start->processAmount++;
        pid = fork();

        //Fehlerbehandlung
        if (pid < 0) {
            error("Fehler bei der Prozessaufteilung");
            return OSMP_ERROR;
        }

        //Child
        if (pid == 0) {
            shm_start->p[i].pid = getpid();
            //printf("ss%s\n",*argv[1]);
            fflush(stdout);
            //Name der auszuführenden Datei und alle Argumente
            execlp(argv[2], *argv, NULL);
            error("Fehler bei execlp %s", argv[2]);

            //Parent braucht durch das Sleep einige Zeit um was in den Speicher zu schreiben. Hier wird direkt ausgelesen. Speicher muss leer sein.
            //strncmp(map, "", strlen(map)) == 0 ? printf("p(%d): Speicher leer!\n", i) : printf("p(%d): %s\n", i, (char *) map);
            return OSMP_ERROR;
        }

            //Parent
        else if (pid > 0) {

            if (i + 1 < processAmount) {
                continue;
            }

            //memcpy(shm_start->msg->data, "s", 1);
            //sprintf(shm_start, "%s", "Hallo Welt");
        }
    }

    //Warte auf alle einzelnen Kinder. -1 = Warte nicht hintereinander sondern parallel
    for (int i = 0; i < processAmount; i++) {

        waitpid(-1, NULL, 0);
        shm_start->processAmount--;
    }

    //shm_unlink(SHMNAME);
    //free(shm_start);

    return 0;
}
