//
// Created by Gurur Öndarö and Thomas Kraaibeek on 24.04.19.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../OSMPLib/OSMPLib.h"

/**
 * Ausführbare Datei, die von allen Prozessen geladen wird. Die Prozesse nutzen OSMPLib-Routinen um miteinander zu kommunizieren
 * @param argc Name dieser Datei
 * @param argv Alle Argumente, die auch dem OSMPStarter übergeben wurden
 * @return Success oder Error
 */
int main(int argc, char** argv){

    int rv = 0, size = 0, rank = 0, source, len;
    char* bufin="Test Message";
    char* bufout;
    int out;
    int in = 5;
    //*in = 5;

    int osmp_int = OSMP_INT;
    int osmp_char = OSMP_BYTE;
    int flag;

    rv = OSMP_Init(&argc,&argv);
    rv = OSMP_Size(&size);
    rv = OSMP_Rank(&rank);

    OSMP_Request request;//= calloc(1, sizeof(IRequest));
    rv = OSMP_CreateRequest(&request);

    //rv = OSMP_Isend(bufin, 12, &osmp_char, rank, request);
    rv = OSMP_Send(bufin, (int) strlen(bufin), &osmp_char, rank);
    //rv = OSMP_Send(in, 1, &osmp_int, rank);


    OSMP_Test(request, &flag);
    printf("flag: %d\n", flag);
    if(flag == 0){
        //rv = OSMP_Wait(request);
    }

    //rv = OSMP_RemoveRequest(request);


    //rv = OSMP_CreateRequest(&request);
    bufout = calloc(1,OSMP_MAX_PAYLOAD_LENGTH);
    //rv = OSMP_Irecv(bufout, 12, &osmp_char, &source, &len, request);
    rv = OSMP_Recv(bufout, 12, &osmp_char, &source, &len);
    rv = OSMP_RemoveRequest(request);

    printf("Recieved from %d : %s\n",source,bufout);

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
