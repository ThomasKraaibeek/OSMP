#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//argv[0] ist hier der Iterator der Schleife in main.c und gleichzeitig die von uns vergebene Prozessnummer
int main(int argc, char** argv){

    //Wenn es sich um den 2. Prozess handelt, dann warte 2 Sekunden.
	if(atoi(argv[0]) == 2){
            sleep(2);
	}
	
	printf("Hello World No. %s\n", argv[0]);
	return 1;
}
