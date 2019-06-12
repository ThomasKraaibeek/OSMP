#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../OSMPLib/OSMPLib.h"

int main(int argc, char** argv){

    int rv = 0, size = 0, rank = 0, source, len;
    //int bufin[2], bufout[2];
    int out;
    int in = 5;
    //*in = 5;
    //printf("im exec: %d\n", atoi(argv[2]));
    fflush(stdout);

    rv = OSMP_Init(&argc,&argv);
    rv = OSMP_Size(&size);
    rv = OSMP_Rank(&rank);

    rv = OSMP_Send(&in, 4, OSMP_INT, rank);
    //rv = OSMP_Recv(&out, 4, OSMP_INT, &source, &len);

    OSMP_Request request;//= calloc(1, sizeof(IRequest));

    rv = OSMP_CreateRequest(&request);
    printf("rv: %d",rv);
    //rv = OSMP_Isend(&in, 1, OSMP_INT, rank, request);
    rv = OSMP_Irecv(&out, 4, OSMP_INT, &source, &len, request);
    rv = OSMP_Wait(request);

    printf("Recieved from %d : %d\n",source,out);

    //OSMP_INT a = 5;
    //OSMP_Test(a);

    /*if(rank==0){
        bufin[0] = 4711;
        bufin[1] = 4712;


        rv = OSMP_Send(&in, 2, OSMP_INT, 1);

    }else{
        rv = OSMP_Recv(&out, 2, OSMP_INT, &source, &len);
        printf("OUT: %d\n", out);
        //printf("OSMP process %d received %d byte from %d [%d:%d] \n",rank, len, source, bufout[0], bufout[1]);
    }*/

    rv = OSMP_Finalize();
    fflush(stdout);


	return rv;
}
