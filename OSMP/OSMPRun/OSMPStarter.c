//
// Created by gurur on 24.04.19.
//

#include "OSMPStarter.h"

shm* shm_start;

int slots_init(int processAmount){

    //dynamische Arraygröße, da erst in Laufzeit bekannt wie viele processstructs benötigt werden.
    shm_start = malloc(sizeof(shm) + processAmount * sizeof(process));
    //todo fehlerbahndlung

    //EMPTYMESSAGE, setze first und last als INIT
    shm_start->emptymsg.firstmsg = 0;
    shm_start->emptymsg.lastmsg = OSMP_MAX_SLOTS;


    /*
     * @TODO Semaphore Init
     * shm_start->emptymsg.mutex etc.
     * */


    //struct process | Array aller Prozesse als Structs | Init all -1
    for(int i=0;i<processAmount;i++){
        shm_start->p[i].firstmsg = -1;
        shm_start->p[i].lastmsg = -1;
        shm_start->p[i].pid = -1;

        //@TODO Semaphore Init
        /*shm_start->p[i].freeslots = x;
        shm_start->p[i].hasmsg = y;
        shm_start->p[i].mutex = z;*/
    }


    for(int i = 0;i<OSMP_MAX_SLOTS;i++){
        shm_start->msg[i].src = -1;
        shm_start->msg[i].len = -1;
        memcpy(shm_start->msg[i].data,"\0",1);

        if(i==OSMP_MAX_SLOTS-1){
            shm_start->msg[i].nextmsg = -1;
        }else{
            shm_start->msg[i].nextmsg = i+1;
        }
    }


}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Syntax: osmprun [int] 	, [int]: Zahl der Prozesse, die erzeugt werden sollen\n");
        return 1;
    }

    //konvertiere string in int
    int processAmount = atoi(argv[1]);
    if (processAmount >=  10000 || processAmount < 0) {
        error("[OSMPStarter.c] Anzahl der Prozesse muss zwischen 1 und %ld liegen\n", sizeof(int)/2);
    }

    //Ausgabe der Argumente
    //printf("#args %d\n",argc);
    for (int i = 0; i < argc; i++) {
        //printf("argv[%d]: %s\n", i, argv[i]);
    }

    pid_t pid;




    slots_init(processAmount);





    //Erzeuge shared memory. 2. Argument: Zugriffsrechte der Prozesse, 3. Argument: Zugriffsrechte der Benutzer
    //@TODO Fehlerbehandlung auslagern
    //@TODO Rechte?
    int fd = shm_open(SHMNAME, O_CREAT | O_RDWR, 0640);
    if(fd==-1){
        error("[OSMPStarter.c] Fehler bei shm_open");
    }

    //Konfiguriere die Größe des Speichers
    int ftrunc = ftruncate(fd, sizeof(emptyslot) + OSMP_MAX_SLOTS * sizeof(message) + processAmount * sizeof(process)); //TODO OSMP_MAX_SLOTS richtig?
    //Fehlerbehandlung, falls ftruncate nicht funktioniert hat
    if (ftrunc == -1) {
        error("[OSMPStarter.c] Fehler bei ftruncate");
    }

    //Mappe den erzeugten shared memory in den Prozessspeicher
    void *map = mmap(0, OSMP_MAX_SLOTS, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); //TODO Rechte?
    //Fehlerbehandlung für das Mapping
    if (map == MAP_FAILED) {
        error("[OSMPStarter.c] Fehler beim Mapping");
    }

    //Forke "processAmount" mal
    for (int i = 0; i < processAmount; i++) {
        pid = fork();

        //Fehlerbehandlung
        if (pid < 0) {
            error("Fehler bei der Prozessaufteilung");
        }

        //Child
        if (pid == 0) {
            //TODO direkt munmap?
            execlp(argv[2],*argv, NULL);
            error("Fehler bei execlp %s",argv[2]);

            //Parent braucht durch das Sleep einige Zeit um was in den Speicher zu schreiben. Hier wird direkt ausgelesen. Speicher muss leer sein.
            //strncmp(map, "", strlen(map)) == 0 ? printf("p(%d): Speicher leer!\n", i) : printf("p(%d): %s\n", i, (char *) map);
            return 1;
        }

        //Parent
        else if (pid > 0) {

            if (i + 1 < processAmount) {
                continue;
            }
            sprintf(map, "%s", "Hallo Welt");
        }

    }

    //Warte auf alle einzelnen Kinder. -1 = Warte nicht hintereinander sondern parallel
    for (int i = 0; i < processAmount; i++) {

        waitpid(-1, NULL, 0);
    }

    free(shm_start);

    return 0;
}
