#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../OSMPLib/OSMPLib.h"

int main(int argc, char** argv){

    int rv = 0, size = 0, rank = 0;
    //printf("im exec: %d\n", atoi(argv[2]));
    fflush(stdout);

    rv = OSMP_Init(&argc,&argv);
    rv = OSMP_Size(&size);
    rv = OSMP_Rank(&rank);
    //rv = OSMP_Finalize();

    //OSMP_INT a = 5;
    //OSMP_Test(a);

    //rv = OSMP_Send()
    //rv = OSMP_Recv();
    printf("Size: %d\n",(int)size);
    printf("Rank: %d\n", rank);


	return 1;
}
