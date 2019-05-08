#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../OSMPLib/OSMPLib.h"

int main(int argc, char** argv){

    int rv,size=0,rank;

    rv = OSMP_Init(&argc,&argv);
    rv = OSMP_Size(&size);
    rv = OSMP_Rank(&rank);
    rv = OSMP_Finalize();

    //rv = OSMP_Send();
    //rv = OSMP_Recv();
    printf("Size: %d\n",(int)size);

	return 1;
}
