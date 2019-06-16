//
// Created by Gurur Öndarö and Thomas Kraaibeek on 12.06.19.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../OSMPLib/OSMPLib.h"
#include "osmpexecutable2.h"


int main(int argc, char** argv) {
    int testnr = atoi(argv[1]);
    int rv = OSMP_ERROR;

    if(testnr==0) rv = Send_Recv(argc,argv);
    else if(testnr==1) rv = Send_Irecv(argc,argv);
    else if(testnr==2) rv = Isend_Irecv(argc,argv);
    else if(testnr==3) rv = Isend_Recv(argc,argv);
    else if(testnr==4) rv = test01(argc, argv);
    else if(testnr==5) rv = test02(argc,argv);
    else if(testnr==6) rv = test03(argc,argv);
    else if(testnr==7) rv = test04(argc,argv);
    else if(testnr==8) rv = test05(argc,argv);
    else if(testnr==9) rv = test06(argc,argv);
    else if(testnr==10) rv = test07(argc,argv);
    else{
        error("Not a valid test no.");
    }

    if(rv==OSMP_ERROR) error("Failed Test");    

    debug("Tests finished. Returning with Code: %d",rv);

    return rv;
}

//Test-Nr 0
int Send_Recv(int argc, char *argv[])
{
    printf("Send_Recv..\n");

    int rv, size, rank, source;
    int bufin[2], bufout[2], len;
    sem_t mutex;
    if(sem_init(&mutex,1,1)==-1) error("[osmpexecutable2.c] sem_init");
    
    rv = OSMP_Init( &argc, &argv );
    rv = OSMP_Size( &size );
    rv = OSMP_Rank( &rank );

    /*while( size != 2 ) {
        rv = OSMP_Size( &size );
        printf("RV: %d, SIZE: %d\n",rv,size);
        error("Test ERROR MESSAGE");
    }*/

    if( rank == 0 )
    { // OSMP process 0
        bufin[0] = 4711;
        bufin[1] = 4712;
        rv = OSMP_Send( bufin, 2, &osmp_int, 1 );
        //rv = sem_post(&mutex);
        rv=sem_wait(&mutex);
    }
    else
    { // OSMP process 1
        //rv=sem_wait(&mutex);
        rv = OSMP_Recv( bufout, 2, &osmp_int, &source, &len );
        rv=sem_post(&mutex);
        printf("OSMP process %d received %d byte from %d [%d:%d] \n",rank, len, source, bufout[0], bufout[1]);
    }
    rv = OSMP_Finalize();

    return rv;
}

//Test-Nr 1
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


//Test-Nr 2
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

//Test-Nr 3
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
//Test-Nr 4
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

//Test-Nr 5
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

//Test-Nr 6
int test03(int argc, char** argv) {

    int size, rank, source, len;
    char *bufin = "Test Message", *bufout;

    if (OSMP_Init(&argc, &argv) == OSMP_ERROR) error("[OSMPExecutable.c] Test03 OSMP_Init");
    if (OSMP_Size(&size) == OSMP_ERROR) error("[osmpexecutable2.c] Test03 OSMP_Size");
    if (OSMP_Rank(&rank) == OSMP_ERROR) error("[osmpexecutable2.c] Test03 OSMP_Rank");

    if (size != 2) error("[osmpexecutable2.c] Test03 Size!=2");

    if(rank==0){
        for(int i = 0;i<18;i++){
            if(OSMP_Send(bufin, OSMP_MAX_PAYLOAD_LENGTH,&osmp_char,1)==OSMP_ERROR) error("[osmpexecutable2.c] Test03 OSMP_Send");
        }
    }else{
        if((bufout = calloc(1,OSMP_MAX_PAYLOAD_LENGTH))==NULL)error("[osmpexecutable2.c] Calloc Fail");
        if(OSMP_Recv(bufout,OSMP_MAX_PAYLOAD_LENGTH,&osmp_char,&source,&len)==OSMP_ERROR) error("[osmpexecutable2.c] Test03 OSMP_Recv");
    }
    if(OSMP_Finalize()==OSMP_ERROR) error("[osmpexecutable2.c] OSMP_Finalize");

    return OSMP_SUCCESS;
}

//Test-Nr 7
int test04(int argc, char** argv) {

    int size, rank, source, len;
    //char bufin[] = "oifhaowifhwihseoifhsdligepihsep8hfpseiohgoseihtoösemoiriuhgprhsgpoiehsp98ghpeuhiwgupeiuhfpsiodhpfegpoihdrpgodrhüghedprg5hsepihp5gueshpgiresfuherfpoipreogish4hhddddddddddddddddddddddddddffffffffffffffffffffffffffffqeqw qweqweqweqweqweqweqweqweasfsdghtrhztu5twessowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgosei\0", *bufout;
    int intbufin[260];
    int intbufout[256];
    for(int i=0;i<260;i++){
        intbufin[i] = i;
    }
    if (OSMP_Init(&argc, &argv) == OSMP_ERROR) error("[OSMPExecutable.c] Test04 OSMP_Init");
    if (OSMP_Size(&size) == OSMP_ERROR) error("[osmpexecutable2.c] Test04 OSMP_Size");
    if (OSMP_Rank(&rank) == OSMP_ERROR) error("[osmpexecutable2.c] Test04 OSMP_Rank");

    //if (size != 2) error("[osmpexecutable2.c] Test03 Size!=2");

    if(rank==0){
        //if(OSMP_Send(bufin, OSMP_MAX_PAYLOAD_LENGTH,&osmp_char,1)==OSMP_ERROR) error("[osmpexecutable2.c] Test03 OSMP_Send");
        if(OSMP_Send(intbufin, 250,&osmp_int,1)==OSMP_ERROR) error("[osmpexecutable2.c] Test04 OSMP_Send");

    }else{
        //if((bufout = calloc(1,OSMP_MAX_PAYLOAD_LENGTH-4))==NULL)error("[osmpexecutable2.c] Calloc Fail");
        //if(OSMP_Recv(bufout,OSMP_MAX_PAYLOAD_LENGTH-4,&osmp_char,&source,&len)==OSMP_ERROR) error("[osmpexecutable2.c] Test03 OSMP_Recv");
        if(OSMP_Recv(intbufout,256,&osmp_int,&source,&len)==OSMP_ERROR) error("[osmpexecutable2.c] Test04 OSMP_Recv");
        //printf("OSMP process %d received message: %s \n", rank, bufout);
        //OSMP_SEND 250 kommt rein, OSMP_RECV 256 akzeptiert, array ausgabe zu lang TRASH DATA
        for(int i = 0;i<256;i++){
            //printf("%d\n",intbufout[i]);
        }
    }
    if(OSMP_Finalize()==OSMP_ERROR) error("[osmpexecutable2.c] OSMP_Finalize");

    return OSMP_SUCCESS;
}

//Test-Nr 8
int test05(int argc, char** argv) {

    int size, rank;
    char *bufin = "Test Message";

    if (OSMP_Init(&argc, &argv) == OSMP_ERROR) error("[OSMPExecutable.c] Test05 OSMP_Init");
    if (OSMP_Size(&size) == OSMP_ERROR) error("[osmpexecutable2.c] Test05 OSMP_Size");
    if (OSMP_Rank(&rank) == OSMP_ERROR) error("[osmpexecutable2.c] Test05 OSMP_Rank");

    if (size != 2) error("[osmpexecutable2.c] Test05 Size!=2");

    if(rank==0){
        for(int i = 0;i<OSMP_MAX_MESSAGES_PROC+1;i++){
            if(OSMP_Send(bufin, OSMP_MAX_PAYLOAD_LENGTH,&osmp_char,rank)==OSMP_ERROR) error("[osmpexecutable2.c] Test05 OSMP_Send");
        }
    }
    if(OSMP_Finalize()==OSMP_ERROR) error("[osmpexecutable2.c] OSMP_Finalize");

    return OSMP_SUCCESS;
}

//Test-Nr 9
int test06(int argc, char** argv){

    int size, rank, source, len, flag;
    //bufin extra uninitialisiert, damit bei Send ein fehler auftritt
    char *bufin;// = "Test Message";
    char* bufout;
    OSMP_Request myrequest;

    if (OSMP_Init(&argc, &argv) == OSMP_ERROR) error("[OSMPExecutable.c] Test06 OSMP_Init");
    if (OSMP_Size(&size) == OSMP_ERROR) error("[osmpexecutable2.c] Test06 OSMP_Size");
    if (OSMP_Rank(&rank) == OSMP_ERROR) error("[osmpexecutable2.c] Test06 OSMP_Rank");


    if(rank==0){ 
        //if (OSMP_CreateRequest(&myrequest)==OSMP_ERROR) error("[osmpexecutable2.c] Test06 OSMP_CreateRequest");
        if(OSMP_Isend(bufin,OSMP_MAX_PAYLOAD_LENGTH,&osmp_char,rank,myrequest)==OSMP_ERROR){
             error("[osmpexecutable2.c] Test06 OSMP_Send");
             return OSMP_ERROR;
        }
        OSMP_Test(myrequest, &flag);
        if(flag == 0){
            if(OSMP_Wait(myrequest)==OSMP_ERROR) error("[osmpexecutable2.c] Test06 OSMP_Wait");
        }
        if(OSMP_RemoveRequest( &myrequest )==OSMP_ERROR) error("[osmpexecutable2.c] Test06 OSMP_RemoveRequest");
    }else{
        //if(OSMP_CreateRequest(&myrequest)==OSMP_ERROR) error("[osmpexecutable2.c] Test06 OSMP_CreateRequest");
        if(OSMP_Irecv(bufout, OSMP_MAX_PAYLOAD_LENGTH, &osmp_char, &source, &len, myrequest)==OSMP_ERROR){
             error("[osmpexecutable2.c] Test06 OSMP_IRecv");
             return OSMP_ERROR;
        }
        if(OSMP_RemoveRequest( &myrequest )==OSMP_ERROR) error("[osmpexecutable2.c] Test06 OSMP_RemoveRequest");

    }
    if(OSMP_Finalize()==OSMP_ERROR) error("[osmpexecutable2.c] OSMP_Finalize");

    return OSMP_SUCCESS;
}

//Test-Nr 10
int test07(int argc, char** argv){

    int size, rank, source, len, flag;
    char *bufin= "Test Message";
    char* bufout;
    OSMP_Request myrequest,myrequest2;

    if (OSMP_Init(&argc, &argv) == OSMP_ERROR) error("[OSMPExecutable.c] Test06 OSMP_Init");
    if (OSMP_Size(&size) == OSMP_ERROR) error("[osmpexecutable2.c] Test06 OSMP_Size");
    if (OSMP_Rank(&rank) == OSMP_ERROR) error("[osmpexecutable2.c] Test06 OSMP_Rank");


    if(rank==0){ 
        if (OSMP_CreateRequest(&myrequest)==OSMP_ERROR) error("[osmpexecutable2.c] Test06 OSMP_CreateRequest");
        for(int i = 0;i<OSMP_MAX_MESSAGES_PROC+2;i++){
            if(OSMP_Isend(bufin,OSMP_MAX_PAYLOAD_LENGTH,&osmp_char,rank,myrequest)==OSMP_ERROR){
                error("[osmpexecutable2.c] Test06 OSMP_Send");
                return OSMP_ERROR;
            }
            if(OSMP_RemoveRequest( &myrequest )==OSMP_ERROR){
                error("[osmpexecutable2.c] Test06 OSMP_RemoveRequest");
                return OSMP_ERROR;
            } 
        }
        

        OSMP_Test(myrequest, &flag);
        if(flag == 0){
            if(OSMP_Wait(myrequest)==OSMP_ERROR) error("[osmpexecutable2.c] Test06 OSMP_Wait");
        }
    }else{
        sleep(1);
        bufout = calloc(1,strlen("Test Message"));
        if(OSMP_CreateRequest(&myrequest2)==OSMP_ERROR) error("[osmpexecutable2.c] Test06 OSMP_CreateRequest");
        if(OSMP_Irecv(bufout, OSMP_MAX_PAYLOAD_LENGTH, &osmp_char, &source, &len, myrequest2)==OSMP_ERROR){
             error("[osmpexecutable2.c] Test06 OSMP_IRecv");
             return OSMP_ERROR;
        }
        if(OSMP_RemoveRequest( &myrequest2 )==OSMP_ERROR) error("[osmpexecutable2.c] Test06 OSMP_RemoveRequest");

    }
    if(OSMP_Finalize()==OSMP_ERROR) error("[osmpexecutable2.c] OSMP_Finalize");

    return OSMP_SUCCESS;
}