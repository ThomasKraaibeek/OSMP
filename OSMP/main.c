#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

char* itos(int value);

int main(int argc,char** argv) {

    //konvertiere string in int
    int iter = atoi(argv[1]);

    //Ausgabe der Argumente
    printf("#args %d\n",argc);
    for (int i = 0; i < argc; i++)
    {
        //printf("argv[%d]: %s\n", i, argv[i]);
    }

    //Array für die PID der Kinder
    //pid_t pids[iter];
    pid_t pid;

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
            char* istring = itos(i);
            execlp("./osmpexecutable", istring, NULL);

            printf("Fehler bei der Prozessaufteilung: %s\n",strerror(errno));
            return 1;
        }

        //Parent
        else if(pid>0) {

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
