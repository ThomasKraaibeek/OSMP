//
// Created by gurur on 24.04.19.
//

#include "OSMPStarter.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Syntax: osmprun [int] 	, [int]: Zahl der Prozesse, die erzeugt werden sollen\n");
        return 1;
    }

    //konvertiere string in int
    int iter = atoi(argv[1]);

    //Ausgabe der Argumente
    //printf("#args %d\n",argc);
    for (int i = 0; i < argc; i++) {
        //printf("argv[%d]: %s\n", i, argv[i]);
    }

    //Array für die PID der Kinder
    //pid_t pids[iter];
    pid_t pid;

    //Erzeuge shared memory. 2. Argument: Zugriffsrechte der Prozesse, 3. Argument: Zugriffsrechte der Benutzer
    //@TODO SHM_OPEN -> OSMP_INIT ?
    //@TODO Fehlerbehandlung auslagern
    //@TODO Rechte?
    int fd = shm_open(SHMNAME, O_CREAT | O_RDWR, 0640);
    if(fd==-1){
        printf("Fehler bei shm_open %s\n", strerror(errno));
        return 1;
    }

    //Konfiguriere die Größe des Speichers
    int ftrunc = ftruncate(fd, sizeof(emptyslot) + OSMP_MAX_SLOTS * sizeof(message) + iter * sizeof(process)); //TODO OSMP_MAX_SLOTS richtig?
    //Fehlerbehandlung, falls ftruncate nicht funktioniert hat
    if (ftrunc == -1) {
        printf("Fehler bei ftruncate %s\n", strerror(errno));
        return 1;
    }

    //Mappe den erzeugten shared memory in den Prozessspeicher
    void *map = mmap(0, OSMP_MAX_SLOTS, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0); //TODO Rechte?
    //Fehlerbehandlung für das Mapping
    if (map == MAP_FAILED) {
        printf("Fehler beim Mapping: %s\n", strerror(errno));
        return 1;
    }

    //Forke "iter" mal
    for (int i = 0; i < iter; i++) {
        pid = fork();

        //Fehlerbehandlung
        if (pid < 0) {
            printf("Fehler bei der Prozessaufteilung: %s\n", strerror(errno));
            return 1;
        }

        //Child
        if (pid == 0) {
            //TODO direkt munmap?
            execlp(argv[2],*argv, NULL);
            printf("Fehler bei execlp %s : %s\n",argv[2],strerror(errno));

            //Parent braucht durch das Sleep einige Zeit um was in den Speicher zu schreiben. Hier wird direkt ausgelesen. Speicher muss leer sein.
            //strncmp(map, "", strlen(map)) == 0 ? printf("p(%d): Speicher leer!\n", i) : printf("p(%d): %s\n", i, (char *) map);
            return 1;
        }

        //Parent
        else if (pid > 0) {

            if (i + 1 < iter) {
                continue;
            }
            sprintf(map, "%s", "Hallo Welt");
        }

    }

    //Warte auf alle einzelnen Kinder. -1 = Warte nicht hintereinander sondern parallel
    for (int i = 0; i < iter; i++) {

        waitpid(-1, NULL, 0);
    }

    return 0;
}
