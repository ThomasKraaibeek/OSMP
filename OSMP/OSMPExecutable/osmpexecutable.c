#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../OSMPLib/OSMPLib.h"

int main(int argc, char** argv){

    int rv = 0, size = 0, rank = 0, source,len;
    int bufin[2], bufout[2];
    //printf("im exec: %d\n", atoi(argv[2]));
    fflush(stdout);

    rv = OSMP_Init(&argc,&argv);
    rv = OSMP_Size(&size);
    rv = OSMP_Rank(&rank);

    //OSMP_INT a = 5;
    //OSMP_Test(a);

    if(rank==0){
        bufin[0] = 4711;
        bufin[1] = 4712;

        rv = OSMP_Send(bufin, 2, OSMP_INT,1);

    }else{
        rv = OSMP_Recv(bufout,2,OSMP_INT,&source, &len);
        printf("OSMP process %d received %d byte from %d [%d:%d] \n",rank, len, source, bufout[0], bufout[1]);
    }

    rv = OSMP_Finalize();
    fflush(stdout);


	return rv;
}
