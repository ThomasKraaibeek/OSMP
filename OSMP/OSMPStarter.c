//
// Created by gurur on 24.04.19.
//

#include "OSMPStarter.h"

int main(int argc,char** argv) {
    if(argc<2){
	    printf("Syntax: osmprun [int] 	, [int]: Zahl der Prozesse, die erzeugt werden sollen\n");
	    return 1;
    }
    //konvertiere string in int
    int iter = atoi(argv[1]);

    //Ausgabe der Argumente
    //printf("#args %d\n",argc);
    for (int i = 0; i < argc; i++)
    {
        //printf("argv[%d]: %s\n", i, argv[i]);
    }

    //Array für die PID der Kinder
    //pid_t pids[iter];
    pid_t pid;




    int fd = shm_open(SHMNAME, O_CREAT | O_RDWR, 0640);

    int ftrunc = ftruncate(fd, SHMSIZE);

    if(ftrunc == -1){
        printf("Fehler bei ftruncate %s\n",strerror(errno));
        return 1;
    }

    void* map = mmap(0, SHMSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if(map == MAP_FAILED){
        printf("Fehler beim Mapping: %s\n",strerror(errno));
        return 1;
    }



    for(int i=0; i<iter; i++){
        pid = fork();

        //Fehlerbehandlung
        if(pid<0){
            printf("Fehler bei der Prozessaufteilung: %s\n",strerror(errno));
            return 1;
        }

        //Child
        if(pid==0){
            //konvertiere int in string. Anzahl der zu erzeugenden Kindprozesse
            //char* istring = itos(i);
            //execlp("./osmpexecutable", istring, NULL);
            //printf("Fehler bei execlp ./osmpexecutable : %s\n",strerror(errno));


            printf("%s",(char*)map);
            sleep(5);
            printf("%s",(char*)map);

            shm_unlink(SHMNAME);
            return 1;
        }

        //Parent
        else if(pid>0) {
            sleep(4);
            sprintf( map, "%s", "Hallo Welt\n");
            //speichere pid der einzelnen Kinder
            //pids[i] = pid;
        }

    }

    //Warte auf alle einzelnen Kinder. -1 = Warte nicht hintereinander sondern parallel
    for(int i=0; i < iter; i++){

        waitpid(-1, NULL, 0);
    }

    return 0;
}

/**
 * @brief Konvertiert Interger zum String
 * Diese Funktion erhält als Übergabeparameter den Integer den er zum String konvertieren soll. Der konvertierte Wert wird dann zurückgegeben.
 *
 * @param value Intergerwert, welcher konvertiert werden soll
 * @return konvertierter Wert als String (char*)
 */
char* itos(int value){

    char* string = malloc(sizeof(int));
    sprintf(string, "%d", value);
    return string;
}
