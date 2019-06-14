//
// Created by Gurur Öndarö and Thomas Kraaibeek on 12.06.19.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../OSMPLib/OSMPLib.h"
#include "osmpexecutable2.h"


int main(int argc, char** argv) {

    int rv = OSMP_ERROR;

    //rv = Send_Recv(argc,argv);

    //rv = Send_Irecv(argc,argv);

    //rv = Isend_Irecv(argc,argv);

    //rv = Isend_Recv(argc,argv);

    //rv = test01(argc, argv);

    //rv = test02(argc,argv);

    rv = test03(argc,argv);

    debug("Tests finished with Code: %d",rv);

    return rv;
}

int Send_Recv(int argc, char *argv[])
{
    printf("Send_Recv..\n");

    int rv, size, rank, source;
    int bufin[2], bufout[2], len;

    rv = OSMP_Init( &argc, &argv );
    rv = OSMP_Size( &size );
    rv = OSMP_Rank( &rank );

    if( size != 2 ) { /* Fehlerbehandlung */ }

    if( rank == 0 )
    { // OSMP process 0
        bufin[0] = 4711;
        bufin[1] = 4712;
        rv = OSMP_Send( bufin, 2, &osmp_int, 1 );
    }
    else
    { // OSMP process 1
        rv = OSMP_Recv( bufout, 2, &osmp_int, &source, &len );
        printf("OSMP process %d received %d byte from %d [%d:%d] \n",rank, len, source, bufout[0], bufout[1]);
    }
    rv = OSMP_Finalize();

    return rv;
}


int Send_Irecv(int argc, char *argv[])
{

    printf("Send_Irecv..\n");

    int rv, size, rank, source, len;
    char *bufin, *bufout;
    OSMP_Request myrequest;

    rv = OSMP_Init( &argc, &argv );
    rv = OSMP_Size( &size );
    rv = OSMP_Rank( &rank );

    if( size != 2 ) { /* Fehlerbehandlung */ }
    if( rank == 0 )
    { // OSMP process 0
        bufin = malloc(1*strlen("hello world")); // check for != NULL
        if(bufin==NULL) {

        }
        strncpy(bufin, "hello world",strlen("hello world")+1);
        rv = OSMP_Send( bufin, strlen("hello world"), &osmp_char, 1 );
    }
    else
    { // OSMP process 1
        bufout = malloc(1*strlen("hello world")); // check for != NULL
        rv = OSMP_CreateRequest( &myrequest );
        rv = OSMP_Irecv( bufout, strlen("hello world"), &osmp_char, &source, &len, myrequest );
        //
        // do something important...
        //
        //check if operation is completed and wait if not
        rv = OSMP_Wait( myrequest );
        //...
        printf("OSMP process %d received message: %s \n", rank, bufout);
        rv = OSMP_RemoveRequest( &myrequest );
    }
    rv = OSMP_Finalize();

    return rv;
}

int Isend_Irecv(int argc, char *argv[])
{
    printf("Isend_Irecv..\n");

    int rv, size, rank, source, len, flag;
    char *bufin, *bufout;
    OSMP_Request myrequest;

    rv = OSMP_Init( &argc, &argv );
    rv = OSMP_Size( &size );
    rv = OSMP_Rank( &rank );

    if( size != 2 ) { /* Fehlerbehandlung */ }
    if( rank == 0 )
    { // OSMP process 0
        bufin = malloc(1*strlen("hello world")); // check for != NULL
        if(bufin==NULL) {

        }
        strncpy(bufin, "hello world",strlen("hello world")+1);

        rv = OSMP_CreateRequest( &myrequest );
        rv = OSMP_Isend( bufin, strlen("hello world"), &osmp_char, 1, myrequest );

        OSMP_Test(myrequest, &flag);
        printf("flag: %d\n", flag);
        if(flag == 0){
            rv = OSMP_Wait(myrequest);
        }
        rv = OSMP_RemoveRequest( &myrequest );
    }
    else
    { // OSMP process 1
        bufout = malloc(1*strlen("hello world")); // check for != NULL
        rv = OSMP_CreateRequest( &myrequest );
        rv = OSMP_Irecv( bufout, strlen("hello world"), &osmp_char, &source, &len, myrequest );

        rv = OSMP_Wait( myrequest );
        //...
        printf("OSMP process %d received message from %d: %s \n", rank, source, bufout);
        rv = OSMP_RemoveRequest( &myrequest );
    }
    rv = OSMP_Finalize();


    return rv;

}

int Isend_Recv(int argc, char *argv[])
{
    printf("Isend_Recv..\n");

    int rv, size, rank, source, len, flag;
    char *bufin, *bufout;
    OSMP_Request myrequest;

    rv = OSMP_Init( &argc, &argv );
    rv = OSMP_Size( &size );
    rv = OSMP_Rank( &rank );

    if( size != 2 ) { /* Fehlerbehandlung */ }
    if( rank == 0 )
    { // OSMP process 0
        bufin = malloc(1*strlen("hello world")); // check for != NULL
        if(bufin==NULL) {

        }
        strncpy(bufin, "hello world",strlen("hello world")+1);

        rv = OSMP_CreateRequest( &myrequest );
        rv = OSMP_Isend( bufin, strlen("hello world"), &osmp_char, 1, myrequest );

        OSMP_Test(myrequest, &flag);
        printf("flag: %d\n", flag);
        if(flag == 0){
            rv = OSMP_Wait(myrequest);
        }
        rv = OSMP_RemoveRequest( &myrequest );
    }
    else
    { // OSMP process 1
        bufout = malloc(1*strlen("hello world")); // check for != NULL

        rv = OSMP_Recv( bufout, strlen("hello world"), &osmp_char, &source, &len);

        printf("OSMP process %d received message from %d: %s \n", rank, source, bufout);

    }
    rv = OSMP_Finalize();

    return rv;

}

int Isend_Recv_proce(int argc, char *argv[])
{
    printf("Isend_Recv..\n");

    int rv, size, rank, source, len, flag;
    char *bufin, *bufout;
    OSMP_Request myrequest;

    rv = OSMP_Init( &argc, &argv );
    rv = OSMP_Size( &size );
    rv = OSMP_Rank( &rank );

    if( size != 2 ) { /* Fehlerbehandlung */ }
    if( rank == 0 )
    { // OSMP process 0
        bufin = malloc(1*strlen("hello world")); // check for != NULL
        if(bufin==NULL) {

        }
        strncpy(bufin, "hello world",strlen("hello world")+1);

        rv = OSMP_CreateRequest( &myrequest );
        rv = OSMP_Isend( bufin, strlen("hello world"), &osmp_char, 1, myrequest );

        OSMP_Test(myrequest, &flag);
        printf("flag: %d\n", flag);
        if(flag == 0){
            rv = OSMP_Wait(myrequest);
        }
        rv = OSMP_RemoveRequest( &myrequest );
    }
    else
    { // OSMP process 1
        bufout = malloc(1*strlen("hello world")); // check for != NULL

        rv = OSMP_Recv( bufout, strlen("hello world"), &osmp_char, &source, &len);

        printf("OSMP process %d received message from %d: %s \n", rank, source, bufout);

    }
    rv = OSMP_Finalize();

    return rv;

}


int test01(int argc, char** argv){

    printf("Test01..\n");

    int rv = OSMP_SUCCESS, size = 0, rank = 0, source, len;
    char* bufin="Test Message";
    char* bufout;
    //*in = 5;
    //printf("im exec: %d\n", atoi(argv[2]));
    fflush(stdout);


    if(OSMP_Size(&size)==OSMP_ERROR) debug("[osmpexecutable2.c] OSMP_Size");
    if(OSMP_Rank(&rank)==OSMP_ERROR) debug("[osmpexecutable2.c] OSMP_Rank");

    if(OSMP_Send(bufin, OSMP_MAX_PAYLOAD_LENGTH, &osmp_int, rank)==OSMP_ERROR) error("[osmpexecutable2.c] OSMP_Send");

    bufout = calloc(1,OSMP_MAX_PAYLOAD_LENGTH);
    if(bufout==NULL) error("[osmpexecutable2.c] Calloc fail");

    if(OSMP_Recv(bufout, OSMP_MAX_PAYLOAD_LENGTH, &osmp_int, &source, &len)==OSMP_ERROR) error("[osmpexecutable2.c] OSMP_Recv");

    //printf("Recieved from %d : %s\n",source,bufout);


    if(OSMP_Finalize()==OSMP_ERROR) error("[osmpexecutable2.c] OSMP_Finalize");
    fflush(stdout);


    //Part2
    if(OSMP_Init(&argc,&argv)==OSMP_ERROR) error("[osmpexecutable2.c] OSMP_Init");
    if(OSMP_Finalize()==OSMP_ERROR) error("[osmpexecutable2.c] OSMP_Finalize");


    if(OSMP_Size(&size)==OSMP_ERROR) error("[osmpexecutable2.c] OSMP_Size");
    if(OSMP_Rank(&rank)==OSMP_ERROR) error("[osmpexecutable2.c] OSMP_Rank");

    if(OSMP_Send(bufin, OSMP_MAX_PAYLOAD_LENGTH, &osmp_int, rank)==OSMP_ERROR) error("[osmpexecutable2.c] OSMP_Send");

    bufout = calloc(1,OSMP_MAX_PAYLOAD_LENGTH);
    if(bufout==NULL) error("[osmpexecutable2.c] Calloc fail");

    if(OSMP_Recv(bufout, OSMP_MAX_PAYLOAD_LENGTH, &osmp_int, &source, &len)==OSMP_ERROR) error("[osmpexecutable2.c] OSMP_Recv");
    //printf("Recieved from %d : %s\n",source,bufout);
    fflush(stdout);


    return rv;
}

int test02(int argc,char** argv){

    printf("Test02\n");

    int rv, size, rank, source, len;
    char *bufout;
    OSMP_Request myrequest;
    rv = OSMP_Init(&argc, &argv);
    if(rv==OSMP_ERROR){
        debug("ERROR OSMP_INIT");
    }
    rv = OSMP_Size( &size );
    rv = OSMP_Rank( &rank );
    if( size != 2 ) error("[osmpexecutable2.c] size!=2 ERROR");
    if( rank == 0 )
    { // OSMP process 0

    }
    else
    { // OSMP process 1
        bufout = malloc(OSMP_MAX_PAYLOAD_LENGTH); // check for != NULL
        if(bufout==NULL){
            error("bufout null");
            return OSMP_ERROR;
        }
        rv = OSMP_CreateRequest( &myrequest );
        rv = OSMP_Irecv( bufout, OSMP_MAX_PAYLOAD_LENGTH, &osmp_char, &source, &len, myrequest );
        if(rv==OSMP_ERROR) debug("[osmpexecutable2.c] OSMP_IRecv error");
        //
        // do something important...
        //
        //check if operation is completed and wait if not
        rv = OSMP_Wait( myrequest );
        //...
        printf("OSMP process %d received message: %s \n", rank, bufout);
        rv = OSMP_RemoveRequest( &myrequest );
    }
    rv = OSMP_Finalize();

    return rv;
}

int test03(int argc, char** argv) {

    int size, rank, source, len;
    char *bufin = "Test Message", *bufout;
    OSMP_Request myrequest;

    if (OSMP_Init(&argc, &argv) == OSMP_ERROR) error("[OSMPExecutable.c] Test03 OSMP_Init");
    if (OSMP_Size(&size) == OSMP_ERROR) error("[osmpexecutable2.c] Test03 OSMP_Size");
    if (OSMP_Rank(&rank) == OSMP_ERROR) error("[osmpexecutable2.c] Test03 OSMP_Rank");

    if (size != 2) error("[osmpexecutable2.c] Test03 Size!=2");

    if(rank==0){
        for(int i = 0;i<18;i++){
            if(OSMP_Send(bufin, OSMP_MAX_PAYLOAD_LENGTH,OSMP_BYTE,1)==OSMP_ERROR) error("[osmpexecutable2.c] Test03 OSMP_Send");
        }
    }else{
        if(bufout = calloc(1,OSMP_MAX_PAYLOAD_LENGTH)==NULL) error("[osmpexecutable2.c] Calloc Fail");
        if(OSMP_Recv(bufout,OSMP_MAX_PAYLOAD_LENGTH,OSMP_BYTE,&source,&len)==OSMP_ERROR) error("[osmpexecutable2.c] Test03 OSMP_Recv");
        wait(2);
    }

    return OSMP_SUCCESS;
}
