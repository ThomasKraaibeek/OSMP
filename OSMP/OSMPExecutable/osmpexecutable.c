//
// Created by Gurur Öndarö and Thomas Kraaibeek on 12.06.19.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../OSMPLib/OSMPLib.h"
#include "osmpexecutable.h"



int main(int argc, char** argv) {
    int rv = OSMP_ERROR;

    //Überprüfe Testnr vom Benutzer
    for(int i = 0; i<strlen(argv[3]); i++){
        //printf("argv:%s\n", (argv[1])+i);
        if(*((argv[3])+i) < 48 || *((argv[3])+i) > 57){//Ascii für Zahlen zwischen 0 und 9
            error("Not a valid test no. Choose between 0 and 12.");
            return rv;
        }
    }

    int testnr = atoi(argv[3]);

    if(testnr==0) rv = Send_Recv(argc,argv);
    else if(testnr==1) rv = Send_Irecv(argc,argv);
    else if(testnr==2) rv = Isend_Irecv(argc,argv);
    else if(testnr==3) rv = Isend_Recv(argc,argv);
    else if(testnr==4) rv = test01(argc,argv);
    else if(testnr==5) rv = test02(argc,argv);
    else if(testnr==6) rv = test03(argc,argv);
    else if(testnr==7) rv = test04(argc,argv);
    else if(testnr==8) rv = test05(argc,argv);
    else if(testnr==9) rv = test06(argc,argv);
    else if(testnr==10) rv = test07(argc,argv);
    else if(testnr==11) rv = test08(argc,argv);
    else if(testnr==12) rv = test09(argc,argv);
    else if(testnr==13) rv = test10(argc,argv);
    else if(testnr==14) rv = test11(argc,argv);
    else if(testnr==15) rv = test12(argc,argv);
    else if(testnr==16) rv = test13(argc,argv);
    else{
        error("Not a valid test no. Choose between 0 and 12.");
    }

    if(rv==OSMP_ERROR) error("Failed Test");    

    debug("Tests finished. Returning with Code: %d",rv);

    return rv;
}

//Test-Nr 0, 2 prozesse
int Send_Recv(int argc, char **argv)
{
    int size, rank, source;
    int bufin[2], bufout[2], len;
    
    if(OSMP_Init( &argc, &argv ) == OSMP_ERROR) {
        error("[osmpexecutable.c] OSMP_Init");
        return OSMP_ERROR;
    }
    if(OSMP_Size( &size ) == OSMP_ERROR) {
        error("[osmpexecutable.c] OSMP_Size");
        return OSMP_ERROR;
    }
    if(OSMP_Rank( &rank ) == OSMP_ERROR) {
        error("[osmpexecutable.c] OSMP_Rank");
        return OSMP_ERROR;
    }

    if(size != 2){
        error("Invalid number of processes. 2 processes required.");
        return OSMP_ERROR;
    }

    if( rank == 0 )
    { // OSMP process 0
        bufin[0] = 4711;
        bufin[1] = 4712;
        if(OSMP_Send( bufin, 2, OSMP_INT, 1 ) == OSMP_ERROR) {
            error("[osmpexecutable.c] Test 00OSMP_Send");
            return OSMP_ERROR;
        }
    }
    else
    { // OSMP process 1
        if(OSMP_Recv( bufout, 2, OSMP_INT, &source, &len )==OSMP_ERROR){
            error("[osmpexecutable.c] Test 00 OSMP_Recv");
            return OSMP_ERROR;
        }
        printf("OSMP process %d received %d byte from %d [%d:%d] \n",rank, len, source, bufout[0], bufout[1]);
    }
    if(OSMP_Finalize()==OSMP_ERROR){
        error("[osmpexecutable.c] Test 00 OSMP_Finalize");
        return OSMP_ERROR;
    }

    return OSMP_SUCCESS;
}

//Test-Nr 1, 2 prozesse
int Send_Irecv(int argc, char **argv)
{
    int size, rank, source, len;
    char *bufin, *bufout;
    OSMP_Request myrequest;

    if(OSMP_Init( &argc, &argv ) == OSMP_ERROR) {
        error("[osmpexecutable.c] OSMP_Init");
        return OSMP_ERROR;
    }
    if(OSMP_Size( &size ) == OSMP_ERROR) {
        error("[osmpexecutable.c] OSMP_Size");
        return OSMP_ERROR;
    }
    if(OSMP_Rank( &rank ) == OSMP_ERROR) {
        error("[osmpexecutable.c] OSMP_Rank");
        return OSMP_ERROR;
    }

    if(size != 2){
        error("Invalid number of processes. 2 processes required.");
        return OSMP_ERROR;
    }

    if( rank == 0 )
    { // OSMP process 0
        bufin = malloc(strlen("hello world") + 1); // check for != NULL
        if(bufin==NULL) {
            error("[osmpexecutable.c] Test 1 malloc");
            return OSMP_ERROR;
        }
        strncpy(bufin, "hello world",strlen("hello world")+1);
        if(OSMP_Send( bufin, strlen("hello world")+1, OSMP_BYTE, 1 )==OSMP_ERROR){
            error("[osmpexecutable.c] Test 1 OSMP_Send");
            free(bufin);
            return OSMP_ERROR;
        }
        free(bufin);
    }
    else
    { // OSMP process 1
        bufout = malloc(strlen("hello world")+1); // check for != NULL
        if(bufout==NULL){
            error("[osmpexecutable.c] Test 1 malloc");
            return OSMP_ERROR;
        }
        if(OSMP_CreateRequest( &myrequest )==OSMP_ERROR){
            free(bufout);
            error("[osmpexecutable.c] Test 1 OSMP_CreateRequest");
            return OSMP_ERROR;
        }
        if(OSMP_Irecv( bufout, strlen("hello world")+1, OSMP_BYTE, &source, &len, myrequest )==OSMP_ERROR){
            free(bufout);
            error("[osmpexecutable.c] Test 1 OSMP_ISend");
            return OSMP_ERROR;
        }

        if(OSMP_Wait( myrequest )==OSMP_ERROR){
            free(bufout);
            error("[osmpexecutable.c] Test 1 OSMP_Wait");
            return OSMP_ERROR;
        }
        printf("OSMP process %d received message: %s \n", rank, bufout);
        if(OSMP_RemoveRequest( &myrequest )==OSMP_ERROR){
            free(bufout);
            error("[osmpexecutable.c] Test 1 OSMP_RemoveRequest");
            return OSMP_ERROR;
        }
        free(bufout);
    }
    if(OSMP_Finalize()==OSMP_ERROR){
        error("[osmpexecutable.c] Test 1 OSMP_Finalize");
        return OSMP_ERROR;
    };

    return OSMP_SUCCESS;
}


//Test-Nr 2, 2 prozesse
int Isend_Irecv(int argc, char **argv)
{
    int size, rank, source, len, flag;
    char *bufin, *bufout;
    OSMP_Request myrequest;

    if(OSMP_Init( &argc, &argv ) == OSMP_ERROR) {
        error("[osmpexecutable.c] OSMP_Init");
        return OSMP_ERROR;
    }
    if(OSMP_Size( &size ) == OSMP_ERROR) {
        error("[osmpexecutable.c] OSMP_Size");
        return OSMP_ERROR;
    }
    if(OSMP_Rank( &rank ) == OSMP_ERROR) {
        error("[osmpexecutable.c] OSMP_Rank");
        return OSMP_ERROR;
    }

    if(size != 2){
        error("Invalid number of processes. 2 processes required.");
        return OSMP_ERROR;
    }

    if( rank == 0 ){ // OSMP process 0
        bufin = malloc(strlen("hello world") + 1); // check for != NULL
        if(bufin == NULL) {
            error("[osmpexecutable.c] Test 2 Malloc");
            return OSMP_ERROR;
        }
        strncpy(bufin, "hello world", strlen("hello world") + 1);

        if(OSMP_CreateRequest( &myrequest ) == OSMP_ERROR){
            error("[osmpexecutable.c] Test 2 OSMP_CreateRequest");
            free(bufin);
            return OSMP_ERROR;
        }
        if(OSMP_Isend( bufin, strlen("hello world") + 1, OSMP_BYTE, 1, myrequest )){
            error("[osmpexecutable.c] Test 2 OSMP_ISend");
            free(bufin);
            return OSMP_ERROR;
        }

        if(OSMP_Test(myrequest, &flag)==OSMP_ERROR){
            error("[osmpexecutable.c] Test 2 OSMP_Test");
            free(bufin);
            return OSMP_ERROR;
        }
        //printf("flag: %d\n", flag);
        if(flag == 0){
            if(OSMP_Wait(myrequest)==OSMP_ERROR){
                free(myrequest);
                free(bufin);
                error("[osmpexecutable.c] Test 2 OSMP_Wait");
                return OSMP_ERROR;
            }
        }
        if(OSMP_RemoveRequest( &myrequest )==OSMP_ERROR){
            free(bufin);
            free(myrequest);
            error("[osmpexecutable.c] Test 2 OSMP_RemoveRequest");
            return OSMP_ERROR;
        }
        free(bufin);
    }
    else
    { // OSMP process 1
        bufout = malloc(strlen("hello world") + 1);
        if(bufout==NULL){
            error("[osmpexecutable.c] Test 2 malloc");
            return OSMP_ERROR;
        }
        if(OSMP_CreateRequest( &myrequest )==OSMP_ERROR){
            free(bufout);
            error("[osmpexecutable.c] Test 2 OSMP_CreateRequest");
            return OSMP_ERROR;
        }
        if(OSMP_Irecv( bufout, strlen("hello world")+1, OSMP_BYTE, &source, &len, myrequest )==OSMP_ERROR){
            free(bufout);
            free(myrequest);
            error("[osmpexecutable.c] Test 2 OSMP_IRecv");
            return OSMP_ERROR;
        }

        if(OSMP_Wait( myrequest )==OSMP_ERROR){
            free(bufout);
            free(myrequest);
            error("[osmpexecutable.c] Test 2 Wait");
            return OSMP_ERROR;
        };

        printf("OSMP process %d received message from %d: %s \n", rank, source, bufout);
        if(OSMP_RemoveRequest( &myrequest )==OSMP_ERROR){
            free(bufout);
            free(myrequest);
            error("[osmpexecutable.c] Test 2 OSMP_RemoveRequest");
            return OSMP_ERROR;
        }
        free(bufout);
    }
    if(OSMP_Finalize()==OSMP_ERROR){
        error("[osmpexecutable.c] Test 2 OSMP_Finalize");
        return OSMP_ERROR;
    }
    return OSMP_SUCCESS;
}

//Test-Nr 3, 2 prozesse
int Isend_Recv(int argc, char **argv)
{
    int size, rank, source, len, flag;
    char *bufin, *bufout;
    OSMP_Request myrequest;

    if(OSMP_Init( &argc, &argv ) == OSMP_ERROR) {
        error("[osmpexecutable.c] OSMP_Init");
        return OSMP_ERROR;
    }
    if(OSMP_Size( &size ) == OSMP_ERROR) {
        error("[osmpexecutable.c] OSMP_Size");
        return OSMP_ERROR;
    }
    if(OSMP_Rank( &rank ) == OSMP_ERROR) {
        error("[osmpexecutable.c] OSMP_Rank");
        return OSMP_ERROR;
    }

    if(size != 2){
        error("Invalid number of processes. 2 processes required.");
        return OSMP_ERROR;
    }

    if( rank == 0 )
    { // OSMP process 0
        bufin = malloc(strlen("hello world") + 1); // check for != NULL
        if(bufin == NULL) {
            error("[osmpexecutable.c] Test 3 malloc");
            return OSMP_ERROR;
        }
        strncpy(bufin, "hello world", strlen("hello world") + 1);

        if(OSMP_CreateRequest( &myrequest ) == OSMP_ERROR){
            error("[osmpexecutable.c] Test 3 OSMP_CreateRequest");
            free(bufin);
            return OSMP_ERROR;
        }
        if(OSMP_Isend( bufin, strlen("hello world") + 1, OSMP_BYTE, 1, myrequest ) == OSMP_ERROR){
            error("[osmpexecutable.c] Test 3 OSMP_ISend");
            free(myrequest);
            free(bufin);
            return OSMP_ERROR;
        }

        if(OSMP_Test(myrequest, &flag) == OSMP_ERROR){
            error("[osmpexecutable.c] Test 3 OSMP_Test");
            free(myrequest);
            free(bufin);
            return OSMP_ERROR;
        }
        printf("flag: %d\n", flag);
        if(flag == 0){
            if(OSMP_Wait(myrequest) == OSMP_ERROR){
                error("[osmpexecutable.c] Test 3 OSMP_Wait");
                free(myrequest);
                free(bufin);
                return OSMP_ERROR;
            }
        }
        if(OSMP_RemoveRequest( &myrequest ) == OSMP_ERROR){
            error("[osmpexecutable.c] Test 3 OSMP_RemoveRequest");
            free(myrequest);
            free(bufin);
            return OSMP_ERROR;
        }
        //free(myrequest);
        free(bufin);
    }
    else
    { // OSMP process 1
        bufout = malloc(strlen("hello world") + 1); // check for != NULL
        if(bufout == NULL){
            error("[osmpexecutable.c] Test 3 malloc");
            return OSMP_ERROR;
        }
        if(OSMP_Recv( bufout, strlen("hello world") + 1, OSMP_BYTE, &source, &len)==OSMP_ERROR){
            error("[osmpexecutable.c] Test 3 OSMP_Recv");
            free(bufout);
            return OSMP_ERROR;
        }

        printf("OSMP process %d received message from %d: %s \n", rank, source, bufout);
        free(bufout);
    }

    if(OSMP_Finalize() == OSMP_ERROR){
        error("[osmpexecutable.c] Test 3 OSMP_Finalize");
        return OSMP_ERROR;
    }
    return OSMP_SUCCESS;
}

//Test-Nr 4, 1 prozess
int test01(int argc, char** argv){

    int size = 0, rank = 0, source, len;
    char* bufin="Test Message";
    char* bufout;
    //*in = 5;
    //printf("im exec: %d\n", atoi(argv[2]));
    fflush(stdout);


    if(OSMP_Size(&size)==OSMP_ERROR) debug("[osmpexecutable.c] OSMP_Size");
    if(OSMP_Rank(&rank)==OSMP_ERROR) debug("[osmpexecutable.c] OSMP_Rank");

    if(OSMP_Send(bufin, OSMP_MAX_PAYLOAD_LENGTH, OSMP_INT, rank)==OSMP_ERROR) error("[osmpexecutable.c] OSMP_Send");

    bufout = calloc(1,OSMP_MAX_PAYLOAD_LENGTH);
    if(bufout==NULL) error("[osmpexecutable.c] Calloc fail");

    if(OSMP_Recv(bufout, OSMP_MAX_PAYLOAD_LENGTH, OSMP_INT, &source, &len)==OSMP_ERROR) error("[osmpexecutable.c] OSMP_Recv");

    //printf("Recieved from %d : %s\n",source,bufout);


    if(OSMP_Finalize()==OSMP_ERROR) error("[osmpexecutable.c] OSMP_Finalize");
    fflush(stdout);

    printf("Part 2\n");
    //Part2
    if(OSMP_Init(&argc,&argv)==OSMP_ERROR) error("[osmpexecutable.c] OSMP_Init");
    if(OSMP_Finalize()==OSMP_ERROR) error("[osmpexecutable.c] OSMP_Finalize");


    if(OSMP_Size(&size)==OSMP_ERROR) error("[osmpexecutable.c] OSMP_Size");
    if(OSMP_Rank(&rank)==OSMP_ERROR) error("[osmpexecutable.c] OSMP_Rank");

    if(OSMP_Send(bufin, OSMP_MAX_PAYLOAD_LENGTH, OSMP_INT, rank)==OSMP_ERROR) error("[osmpexecutable.c] OSMP_Send");

    
    if(OSMP_Recv(bufout, OSMP_MAX_PAYLOAD_LENGTH, OSMP_INT, &source, &len)==OSMP_ERROR) error("[osmpexecutable.c] OSMP_Recv");
    //printf("Recieved from %d : %s\n",source,bufout);
    fflush(stdout);

    free(bufout);
    return OSMP_SUCCESS;
}

//Test-Nr 5, 2 prozesse
int test02(int argc,char** argv){


    int size, rank, source, len;
    char *bufout;
    OSMP_Request myrequest;
    if(OSMP_Init(&argc, &argv)==OSMP_ERROR) {
        error("[osmpexecutable.c] Test02 OSMP_Init");
        return OSMP_ERROR;
    }

    if(OSMP_Size( &size ) == OSMP_ERROR) {
        error("[osmpexecutable.c] Test02 OSMP_Size");
        return OSMP_ERROR;
    }
    if(OSMP_Rank( &rank ) == OSMP_ERROR) {
        error("[osmpexecutable.c] Test02 OSMP_Rank");
        return OSMP_ERROR;
    }
    if(size != 2){
        error("Invalid number of processes. 2 processes required.");
        return OSMP_ERROR;
    }

    if( rank == 0 )
    { 
        // Do nothing
    }
    else
    { // OSMP process 1
        bufout = malloc(OSMP_MAX_PAYLOAD_LENGTH); // check for != NULL
        if(bufout==NULL){
            error("bufout null");
            return OSMP_ERROR;
        }
        if(OSMP_CreateRequest( &myrequest )==OSMP_ERROR){
            error("[osmpexecutable.c] Test02 OSMP_CreateRequest");
            free(bufout);
            return OSMP_ERROR;
        }
        if(OSMP_Irecv( bufout, OSMP_MAX_PAYLOAD_LENGTH, OSMP_BYTE, &source, &len, myrequest )==OSMP_ERROR){
            free(bufout);
            free(myrequest);
            error("[osmpexecutable.c] Test02 OSMP_IRecv");
            return OSMP_ERROR;
        }
  
        //check if operation is completed and wait if not
        if(OSMP_Wait( myrequest )==OSMP_ERROR){
            free(bufout);
            free(myrequest);
            error("[osmpexecutable.c] Test02 OSMP_Wait");
            return OSMP_ERROR;
        }
        printf("OSMP process %d received message: %s \n", rank, bufout);
        if(OSMP_RemoveRequest( &myrequest )==OSMP_ERROR){
            free(bufout);
            free(myrequest);
            error("[osmpexecutable.c] Test02 OSMP_RemoveRequest");
            return OSMP_ERROR;
        }
        free(bufout);
    }
    if(OSMP_Finalize()==OSMP_ERROR){
        error("[osmpexecutable.c] Test02 OSMP_Size");
        return OSMP_ERROR;
    }
    return OSMP_SUCCESS;
}

//Test-Nr 6, 2 prozesse
int test03(int argc, char** argv) {

    int size, rank, source, len;
    char *bufin = "Test Message", *bufout;

    if(OSMP_Init(&argc, &argv)==OSMP_ERROR) {
        error("[osmpexecutable.c] Test03 OSMP_Init");
        return OSMP_ERROR;
    }
    if(OSMP_Size( &size ) == OSMP_ERROR) {
        error("[osmpexecutable.c] Test03 OSMP_Size");
        return OSMP_ERROR;
    }
    if(OSMP_Rank( &rank ) == OSMP_ERROR) {
        error("[osmpexecutable.c] Test03 OSMP_Rank");
        return OSMP_ERROR;
    }

    if(size != 2){
        error("Invalid number of processes. 2 processes required.");
        return OSMP_ERROR;
    }

    if(rank==0){
        for(int i = 0;i<18;i++){
            if(OSMP_Send(bufin, OSMP_MAX_PAYLOAD_LENGTH,OSMP_BYTE,1)==OSMP_ERROR) {
                error("[osmpexecutable.c] Test03 OSMP_Send");
                return OSMP_ERROR;
            }
        }
    }else{
        if((bufout = calloc(1,OSMP_MAX_PAYLOAD_LENGTH))==NULL) {
            error("[osmpexecutable.c] Calloc Fail");
            return OSMP_ERROR;
        }
        if(OSMP_Recv(bufout,OSMP_MAX_PAYLOAD_LENGTH,OSMP_BYTE,&source,&len)==OSMP_ERROR) {
            free(bufout);
            error("[osmpexecutable.c] Test03 OSMP_Recv");
            return OSMP_ERROR;
        }
        printf("OSMP process %d received message: %s \n", rank, bufout);
        free(bufout);
    }
    if(OSMP_Finalize()==OSMP_ERROR) {
        error("[osmpexecutable.c] OSMP_Finalize");
        return OSMP_ERROR;
    }

    return OSMP_SUCCESS;
}

//Test-Nr 7, 2 prozesse
int test04(int argc, char** argv) {

    int size, rank, source, len;
    //char bufin[] = "oifhaowifhwihseoifhsdligepihsep8hfpseiohgoseihtoösemoiriuhgprhsgpoiehsp98ghpeuhiwgupeiuhfpsiodhpfegpoihdrpgodrhüghedprg5hsepihp5gueshpgiresfuherfpoipreogish4hhddddddddddddddddddddddddddffffffffffffffffffffffffffffqeqw qweqweqweqweqweqweqweqweasfsdghtrhztu5twessowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgoseiowifhwihseoifhsdligepihsep8hfpseiohgosei\0", *bufout;
    int intbufin[290];
    int intbufout[256];
    for(int i=0;i<290;i++){
        intbufin[i] = i;
    }
    if(OSMP_Init(&argc, &argv)==OSMP_ERROR) {
        error("[osmpexecutable.c] Test04 OSMP_Init");
        return OSMP_ERROR;
    }
    if(OSMP_Size( &size ) == OSMP_ERROR) {
        error("[osmpexecutable.c] Test04 OSMP_Size");
        return OSMP_ERROR;
    }
    if(OSMP_Rank( &rank ) == OSMP_ERROR) {
        error("[osmpexecutable.c] Test04 OSMP_Rank");
        return OSMP_ERROR;
    }
    if(size != 2){
        error("Invalid number of processes. 2 processes required.");
        return OSMP_ERROR;
    }

    if(rank==0){
        if(OSMP_Send(intbufin, OSMP_MAX_PAYLOAD_LENGTH+1,OSMP_INT,1)==OSMP_ERROR) {
            error("[osmpexecutable.c] Test03 OSMP_Send");
            return OSMP_ERROR;
        }
        if(OSMP_Send(intbufin, 250,OSMP_INT,1)==OSMP_ERROR){
            error("[osmpexecutable.c] Test04 OSMP_Send");
            return OSMP_ERROR;
        }

    }else{
        //if((bufout = calloc(1,OSMP_MAX_PAYLOAD_LENGTH-4))==NULL)error("[osmpexecutable.c] Calloc Fail");
        //if(OSMP_Recv(bufout,OSMP_MAX_PAYLOAD_LENGTH-4,OSMP_BYTE,&source,&len)==OSMP_ERROR) error("[osmpexecutable.c] Test03 OSMP_Recv");
        if(OSMP_Recv(intbufout,200,OSMP_INT,&source,&len)==OSMP_ERROR) {
            error("[osmpexecutable.c] Test04 OSMP_Recv");
            return OSMP_ERROR;
        }
        //printf("OSMP process %d received message: %s \n", rank, bufout);
        //OSMP_SEND 250 kommt rein, OSMP_RECV 256 akzeptiert, array ausgabe zu lang TRASH DATA
        for(int i = 0;i<256;i++){
            //printf("%d\n",intbufout[i]);
        }
    }
    if(OSMP_Finalize()==OSMP_ERROR) {
        error("[osmpexecutable.c] OSMP_Finalize");
        return OSMP_ERROR;
    }
    return OSMP_SUCCESS;
}

//Test-Nr 8 16 prozesse
int test05(int argc, char** argv) {

    int size, rank;
    char *bufin = "Test Message";

    if (OSMP_Init(&argc, &argv) == OSMP_ERROR) {
        error("[OSMPExecutable.c] Test05 OSMP_Init");
        return OSMP_ERROR;
    }
    if (OSMP_Size(&size) == OSMP_ERROR) {
        error("[osmpexecutable.c] Test05 OSMP_Size");
        return OSMP_ERROR;
    }
    if (OSMP_Rank(&rank) == OSMP_ERROR) {
        error("[osmpexecutable.c] Test05 OSMP_Rank");
        return OSMP_ERROR;
    }

    if(size != 16){
        error("Invalid number of processes. 16 processes required.");
        return OSMP_ERROR;
    }

    if(rank+1==size){
        //+1 um eine nachricht zu viel zu schicken. 256(16*16) Nachrichten insgesamt zugelassen
        //257 werden gesendet
        for(int i = 0;i<OSMP_MAX_MESSAGES_PROC+1;i++){
            if(OSMP_Send(bufin,(int)strlen(bufin),OSMP_BYTE,0)==OSMP_ERROR){
                error("[osmpexecutable.c] Tesst05 OSMP_Send");
                return OSMP_ERROR;
            }
        }
    }else{
        for(int i = 0;i<OSMP_MAX_MESSAGES_PROC;i++){
            if(OSMP_Send(bufin,(int)strlen(bufin),OSMP_BYTE,rank+1)==OSMP_ERROR){
                error("[osmpexecutable.c] Tesst05 OSMP_Send");
                return OSMP_ERROR;
            } 
        }
    }
    if(OSMP_Finalize()==OSMP_ERROR) error("[osmpexecutable.c] OSMP_Finalize");
    return OSMP_SUCCESS;
}

//Test-Nr 9, 2 prozess
int test06(int argc, char** argv){

    int size, rank, source, len, flag;
    //bufin extra uninitialisiert, damit bei Send ein fehler auftritt
    char* bufin;// = "Test Message";
    char* bufout;
    OSMP_Request myrequest;

    if (OSMP_Init(&argc, &argv) == OSMP_ERROR) {
        error("[OSMPExecutable.c] Test06 OSMP_Init");
        return OSMP_ERROR;
    }
    if (OSMP_Size(&size) == OSMP_ERROR) {
        error("[osmpexecutable.c] Test06 OSMP_Size");
        return OSMP_ERROR;
    }
    if (OSMP_Rank(&rank) == OSMP_ERROR) {
        error("[osmpexecutable.c] Test06 OSMP_Rank");
        return OSMP_ERROR;
    }

    if(rank==0){ 
        //if (OSMP_CreateRequest(&myrequest)==OSMP_ERROR) error("[osmpexecutable.c] Test06 OSMP_CreateRequest");
        if(OSMP_Isend(bufin,OSMP_MAX_PAYLOAD_LENGTH,OSMP_BYTE,rank,myrequest)==OSMP_ERROR){
             error("[osmpexecutable.c] Test06 OSMP_Send");
             return OSMP_ERROR;
        }

        //......... 
        OSMP_Test(myrequest, &flag);
        if(flag == 0){
            if(OSMP_Wait(myrequest)==OSMP_ERROR) error("[osmpexecutable.c] Test06 OSMP_Wait");
        }
        if(OSMP_RemoveRequest( &myrequest )==OSMP_ERROR) error("[osmpexecutable.c] Test06 OSMP_RemoveRequest");
    }else{
        //if(OSMP_CreateRequest(&myrequest)==OSMP_ERROR) error("[osmpexecutable.c] Test06 OSMP_CreateRequest");
        if(OSMP_Irecv(bufout, OSMP_MAX_PAYLOAD_LENGTH, OSMP_BYTE, &source, &len, myrequest)==OSMP_ERROR){
             error("[osmpexecutable.c] Test06 OSMP_IRecv");
             return OSMP_ERROR;
        }

        //.....
        printf("OSMP process %d received message: %s \n", rank, bufout);
        if(OSMP_RemoveRequest( &myrequest )==OSMP_ERROR) error("[osmpexecutable.c] Test06 OSMP_RemoveRequest");
    }
    if(OSMP_Finalize()==OSMP_ERROR) error("[osmpexecutable.c] OSMP_Finalize");

    return OSMP_SUCCESS;
}

//Test-Nr 10, 1 prozess
int test07(int argc, char** argv){

    int size, rank, flag;
    char *bufin= "Test Message";
    OSMP_Request myrequest;

    if (OSMP_Init(&argc, &argv) == OSMP_ERROR) {
        error("[OSMPExecutable.c] Test07 OSMP_Init");
        return OSMP_ERROR;
    }
    if (OSMP_Size(&size) == OSMP_ERROR) {
        error("[osmpexecutable.c] Test07 OSMP_Size");
        return OSMP_ERROR;
    }
    if (OSMP_Rank(&rank) == OSMP_ERROR) {
        error("[osmpexecutable.c] Test07 OSMP_Rank");
        return OSMP_ERROR;
    }

    if(rank==0){ 
        if (OSMP_CreateRequest(&myrequest)==OSMP_ERROR){
            error("[osmpexecutable.c] Test07 OSMP_CreateRequest");
            return OSMP_ERROR;
        }
        for(int i = 0;i<OSMP_MAX_MESSAGES_PROC+2;i++){
            if(OSMP_Isend(bufin,OSMP_MAX_PAYLOAD_LENGTH,OSMP_BYTE,rank,myrequest)==OSMP_ERROR){
                error("[osmpexecutable.c] Test07 OSMP_Send");
                free(myrequest);
                return OSMP_ERROR;
            }
            if(OSMP_RemoveRequest( &myrequest )==OSMP_ERROR){
                error("[osmpexecutable.c] Test07 OSMP_RemoveRequest");
                return OSMP_ERROR;
            } 
        }

        if(OSMP_Test(myrequest, &flag)==OSMP_ERROR){
            error("[osmpexecutable.c] Test 07 OSMP_Test");
            return OSMP_ERROR;
        }
        if(flag == 0){
            if(OSMP_Wait(myrequest)==OSMP_ERROR){
                error("[osmpexecutable.c] Test07 OSMP_Wait");
                return OSMP_ERROR;
            }
        }
    }
    if(OSMP_Finalize()==OSMP_ERROR){
        error("[osmpexecutable.c] OSMP_Finalize");
        return OSMP_ERROR;
    }

    return OSMP_SUCCESS;
}

//Test-Nr 11, 1 prozess
int test08(int argc, char** argv){

    int size, rank, flag;
    char *bufin= "Test Message";
    OSMP_Request myrequest;

    if (OSMP_Init(&argc, &argv) == OSMP_ERROR) {
        error("[OSMPExecutable.c] Test08 OSMP_Init");
        return OSMP_ERROR;
    }
    if (OSMP_Size(&size) == OSMP_ERROR) {
        error("[osmpexecutable.c] Test08 OSMP_Size");
        return OSMP_ERROR;
    }
    if (OSMP_Rank(&rank) == OSMP_ERROR) {
        error("[osmpexecutable.c] Test08 OSMP_Rank");
        return OSMP_ERROR;
    }

    if(size != 1){
        error("Invalid number of processes. 1 processe required.");
        return OSMP_ERROR;
    }

    if(OSMP_CreateRequest( &myrequest ) == OSMP_ERROR){
        error("[osmpexecutable.c] Test 08 OSMP_CreateRequest");
        return OSMP_ERROR;
    }

    //sende an nicht existierenden Prozess 6
    if(OSMP_Isend(bufin,OSMP_MAX_PAYLOAD_LENGTH,OSMP_BYTE,6,myrequest)==OSMP_ERROR){
        error("[osmpexecutable.c] Test08 OSMP_Send");
        free(myrequest);
        return OSMP_ERROR;
    }

    if(OSMP_Test(myrequest, &flag)==OSMP_ERROR){
        error("[osmpexecutable.c] Test 08 OSMP_Test");
        free(myrequest);
        return OSMP_ERROR;
    }
    if(flag == 0){
        if(OSMP_Wait(myrequest)==OSMP_ERROR){
            error("[osmpexecutable.c] Test08 OSMP_Wait");
            free(myrequest);
            return OSMP_ERROR;
        }
    }
    if(OSMP_RemoveRequest(&myrequest)==OSMP_ERROR){
        error("[osmpexecitable2.c] Test08 OSMP_RemoveRequest");
        free(myrequest);
        return OSMP_ERROR;
    }

    if(OSMP_Finalize()==OSMP_ERROR){
        error("[osmpexecutable.c] Test08 OSMP_Finalize");
        return OSMP_ERROR;
    }
    return OSMP_SUCCESS;
}

//Test-Nr 12, 2 prozesse
int test09(int argc, char **argv)
{
    printf("Test-Nr 09..\n");

    int size, rank, source, len;
    double bufout[3];
    double doublebufin[] = {1.9, 2.7, 3.8};

    if (OSMP_Init(&argc, &argv) == OSMP_ERROR) {
        error("[OSMPExecutable.c] Test09 OSMP_Init");
        return OSMP_ERROR;
    }
    if (OSMP_Size(&size) == OSMP_ERROR) {
        error("[osmpexecutable.c] Test09 OSMP_Size");
        return OSMP_ERROR;
    }
    if (OSMP_Rank(&rank) == OSMP_ERROR) {
        error("[osmpexecutable.c] Test09 OSMP_Rank");
        return OSMP_ERROR;
    }
    if(size != 2){
        error("Invalid number of processes. 2 processes required.");
        return OSMP_ERROR;
    }

    if(rank==0){
        if(OSMP_Send(doublebufin, 3,OSMP_DOUBLE,1)==OSMP_ERROR){
            error("[osmpexecutable.c] Test09 OSMP_Send");
            return OSMP_ERROR;
        }

    }else{
        /*if((bufout = calloc(1,OSMP_MAX_PAYLOAD_LENGTH))==NULL){
            error("[osmpexecutable.c] Calloc Fail");
            return OSMP_ERROR;
        }*/
        if(OSMP_Recv(bufout,24,OSMP_BYTE,&source,&len)==OSMP_ERROR){
            error("[osmpexecutable.c] Test09 OSMP_Recv");
            //free(bufout);
            return OSMP_ERROR;
        }
        printf("OSMP process %d received message: %f, %f, %f \n", rank, bufout[0], bufout[1], bufout[2]);
        //free(bufout);
    }
    if(OSMP_Finalize()==OSMP_ERROR){
        error("[osmpexecutable.c] Test09 OSMP_Finalize");
        return OSMP_ERROR;
    }

    return OSMP_SUCCESS;
}

//Test-Nr 13, n prozesse
int test10(int argc, char** argv) {

    int size, rank, source, len;
    int out = 0;

    if (OSMP_Init(&argc, &argv) == OSMP_ERROR) {
        error("[OSMPExecutable.c] Test10 OSMP_Init");
        return OSMP_ERROR;
    }
    if (OSMP_Size(&size) == OSMP_ERROR) {
        error("[osmpexecutable.c] Test10 OSMP_Size");
        return OSMP_ERROR;
    }
    if (OSMP_Rank(&rank) == OSMP_ERROR) {
        error("[osmpexecutable.c] Test10 OSMP_Rank");
        return OSMP_ERROR;
    }

    if(rank==size-1) {
        if(OSMP_Send(&rank, 1,OSMP_INT,0)==OSMP_ERROR){
            error("[osmpexecutable.c] Test10 OSMP_Send");
            return OSMP_ERROR;
        }
    }else {
        if(OSMP_Send(&rank, 1,OSMP_INT,rank+1)==OSMP_ERROR){
            error("[osmpexecutable.c] Test10 OSMP_Send");
            return OSMP_ERROR;
        }
    }

    if(OSMP_Recv(&out,1,OSMP_INT,&source,&len)==OSMP_ERROR){
        error("[osmpexecutable.c] Test10 OSMP_Send");
        return OSMP_ERROR;
    }
    printf("OSMP process %d received message: %d \n", rank, out);

    if(OSMP_Finalize()==OSMP_ERROR){
        error("[osmpexecutable.c] Test10 OSMP_Finalize");
        return OSMP_ERROR;
    }

    return OSMP_SUCCESS;
}

//Test-Nr 14, n prozesse
int test11(int argc, char** argv) {

    int size, rank, source, len;
    int out = 0;
    int flag = 0;

    OSMP_Request myrequest;

    if (OSMP_Init(&argc, &argv) == OSMP_ERROR) {
        error("[OSMPExecutable.c] Test11 OSMP_Init");
        return OSMP_ERROR;
    }
    if (OSMP_Size(&size) == OSMP_ERROR) {
        error("[osmpexecutable.c] Test11 OSMP_Size");
        return OSMP_ERROR;
    }
    if (OSMP_Rank(&rank) == OSMP_ERROR) {
        error("[osmpexecutable.c] Test11 OSMP_Rank");
        return OSMP_ERROR;
    }

    if (OSMP_CreateRequest(&myrequest)==OSMP_ERROR){
        error("[osmpexecutable.c] Test11 OSMP_CreateRequest");
        return OSMP_ERROR;
    }

    if(rank==size-1){
        if(OSMP_Isend(&rank,1,OSMP_INT,0,myrequest)==OSMP_ERROR) {
            free(myrequest);
            error("[osmpexecutable.c] Test11 OSMP_Send");
            return OSMP_ERROR;
        }
    }else{
        if(OSMP_Isend(&rank,1,OSMP_INT,rank+1,myrequest)==OSMP_ERROR) {
            free(myrequest);
            error("[osmpexecutable.c] Test11 OSMP_Send");
            return OSMP_ERROR;
        }
    }
    if(OSMP_Test(myrequest, &flag) == OSMP_ERROR){
        free(myrequest);
        error("[osmpexecutable.c] Test11 OSMP_Test");
        return OSMP_ERROR;
    }
    if(flag == 0){
        if(OSMP_Wait(myrequest)==OSMP_ERROR){
            free(myrequest);
            error("[osmpexecutable.c] Test11 OSMP_Wait");
            return OSMP_ERROR;
        }
    }
    if(OSMP_RemoveRequest(&myrequest)==OSMP_ERROR){
        error("[osmpexecitable.c] Test11 OSMP_RemoveRequest");
        free(myrequest);
        return OSMP_ERROR;
    }

    if(OSMP_CreateRequest(&myrequest)==OSMP_ERROR){
        error("[osmpexecutable.c] Test11 OSMP_CreateRequest");
        return OSMP_ERROR;
    }
    if(OSMP_Irecv(&out, 1, OSMP_INT, &source, &len, myrequest) == OSMP_ERROR){
        error("[osmpexecutable.c] Test11 OSMP_Irecv");
        free(myrequest);
        return OSMP_ERROR;
    }

    if(OSMP_Wait( myrequest )==OSMP_ERROR) {
        error("[osmpexecutable.c] Test11 OSMP_Wait");
        free(myrequest);
        return OSMP_ERROR;
    }

    printf("OSMP process %d received message: %d \n", rank, source);

    if(OSMP_RemoveRequest(&myrequest)==OSMP_ERROR){
        error("[osmpexecitable.c] Test11 OSMP_RemoveRequest");
        free(myrequest);
        return OSMP_ERROR;
    }

    if(OSMP_Finalize()==OSMP_ERROR){
        error("[osmpexecutable.c] Test11 OSMP_Finalize");
        return OSMP_ERROR;
    }

    return OSMP_SUCCESS;
}

//Test Nr. 15, 3 prozesse
int test12(int argc, char **argv)
{
    int size, rank, source, len;
    unsigned short bufin[8], bufout[8];
    
    if(OSMP_Init( &argc, &argv ) == OSMP_ERROR) {
        error("[osmpexecutable.c] OSMP_Init");
        return OSMP_ERROR;
    }
    if(OSMP_Size( &size ) == OSMP_ERROR) {
        error("[osmpexecutable.c] OSMP_Size");
        return OSMP_ERROR;
    }
    if(OSMP_Rank( &rank ) == OSMP_ERROR) {
        error("[osmpexecutable.c] OSMP_Rank");
        return OSMP_ERROR;
    }

    if(size != 3){
        error("Invalid number of processes. 2 processes required.");
        return OSMP_ERROR;
    }

    if( rank == 0 )
    { // OSMP process 0
        for(int i = 0;i<8;i++){
            bufin[i] = (unsigned short) i;
        }
        if(OSMP_Send( bufin, 8, OSMP_UNSIGNED_SHORT, 1 ) == OSMP_ERROR) {
            error("[osmpexecutable.c] Test 12 OSMP_Send");
            return OSMP_ERROR;
        }
    }
    else if(rank==2){
        for(int i = 7;i>=0;i--){
            bufin[7-i] = (unsigned short) i;
        }
        if(OSMP_Send( bufin, 8, OSMP_UNSIGNED_SHORT, 1 ) == OSMP_ERROR) {
            error("[osmpexecutable.c] Test 12 OSMP_Send");
            return OSMP_ERROR;
        }
    }
    else if(rank==1)
    { // OSMP process 1
        if(OSMP_Recv( bufout, 8, OSMP_UNSIGNED_SHORT, &source, &len )==OSMP_ERROR){
            error("[osmpexecutable.c] Test 12 OSMP_Recv");
            return OSMP_ERROR;
        }
        for(int i = 0;i<8;i++){
            printf("OSMP process %d received %d byte from %d [%hu] \n",rank, len, source, bufout[i]);
        }
        if(OSMP_Recv( bufout, 8, OSMP_UNSIGNED_SHORT, &source, &len )==OSMP_ERROR){
            error("[osmpexecutable.c] Test 12 OSMP_Recv");
            return OSMP_ERROR;
        }
        for(int i = 0;i<8;i++){
            printf("OSMP process %d received %d byte from %d [%hu] \n",rank, len, source, bufout[i]);
        }
    }
    if(OSMP_Finalize()==OSMP_ERROR){
        error("[osmpexecutable.c] Test 12 OSMP_Finalize");
        return OSMP_ERROR;
    }

    return OSMP_SUCCESS;
}

//Test-Nr 16, 2 prozesse
int test13(int argc, char **argv)
{
    int size, rank, source, len, flag;
    char *bufin, *bufout;
    OSMP_Request myrequest;

    if(OSMP_Init( &argc, &argv ) == OSMP_ERROR) {
        error("[osmpexecutable.c] OSMP_Init");
        return OSMP_ERROR;
    }
    if(OSMP_Size( &size ) == OSMP_ERROR) {
        error("[osmpexecutable.c] OSMP_Size");
        return OSMP_ERROR;
    }
    if(OSMP_Rank( &rank ) == OSMP_ERROR) {
        error("[osmpexecutable.c] OSMP_Rank");
        return OSMP_ERROR;
    }

    if(size != 2){
        error("Invalid number of processes. 2 processes required.");
        return OSMP_ERROR;
    }

    if( rank == 0 )
    { // OSMP process 0
        bufin = malloc(strlen("hello world") + 1); // check for != NULL
        if(bufin == NULL) {
            error("[osmpexecutable.c] Test 3 malloc");
            return OSMP_ERROR;
        }
        strncpy(bufin, "hello world", strlen("hello world") + 1);

        if(OSMP_CreateRequest( &myrequest ) == OSMP_ERROR){
            error("[osmpexecutable.c] Test 3 OSMP_CreateRequest");
            free(bufin);
            return OSMP_ERROR;
        }
        if(OSMP_Isend( bufin, strlen("hello world") + 1, OSMP_BYTE, 1, myrequest ) == OSMP_ERROR){
            error("[osmpexecutable.c] Test 3 OSMP_ISend");
            free(myrequest);
            free(bufin);
            return OSMP_ERROR;
        }

        if(OSMP_Test(myrequest, &flag) == OSMP_ERROR){
            error("[osmpexecutable.c] Test 3 OSMP_Test");
            free(myrequest);
            free(bufin);
            return OSMP_ERROR;
        }
        printf("flag: %d\n", flag);
        if(flag == 0){
            if(OSMP_Wait(myrequest) == OSMP_ERROR){
                error("[osmpexecutable.c] Test 3 OSMP_Wait");
                free(myrequest);
                free(bufin);
                return OSMP_ERROR;
            }
        }

        // Request Struktur wird zweimal benutzt
        strncpy(bufin, "hallo welt!", strlen("hallo welt!") + 1);

        if(OSMP_Isend( bufin, strlen("hallo welt!") + 1, OSMP_BYTE, 1, myrequest ) == OSMP_ERROR){
            error("[osmpexecutable.c] Test 3 OSMP_ISend");
            free(myrequest);
            free(bufin);
            return OSMP_ERROR;
        }

        if(OSMP_Test(myrequest, &flag) == OSMP_ERROR){
            error("[osmpexecutable.c] Test 3 OSMP_Test");
            free(myrequest);
            free(bufin);
            return OSMP_ERROR;
        }
        printf("flag: %d\n", flag);
        if(flag == 0){
            if(OSMP_Wait(myrequest) == OSMP_ERROR){
                error("[osmpexecutable.c] Test 3 OSMP_Wait");
                free(myrequest);
                free(bufin);
                return OSMP_ERROR;
            }
        }

        if(OSMP_RemoveRequest( &myrequest ) == OSMP_ERROR){
            error("[osmpexecutable.c] Test 3 OSMP_RemoveRequest");
            free(myrequest);
            free(bufin);
            return OSMP_ERROR;
        }
        //free(myrequest);
        free(bufin);
    }
    else
    { // OSMP process 1
        bufout = malloc(strlen("hello world") + 1); // check for != NULL
        if(bufout == NULL){
            error("[osmpexecutable.c] Test 3 malloc");
            return OSMP_ERROR;
        }
        if(OSMP_Recv( bufout, strlen("hello world") + 1, OSMP_BYTE, &source, &len)==OSMP_ERROR){
            error("[osmpexecutable.c] Test 3 OSMP_Recv");
            free(bufout);
            return OSMP_ERROR;
        }

        printf("OSMP process %d received message from %d: %s \n", rank, source, bufout);


        if(OSMP_Recv( bufout, strlen("hallo welt!") + 1, OSMP_BYTE, &source, &len)==OSMP_ERROR){
            error("[osmpexecutable.c] Test 3 OSMP_Recv");
            free(bufout);
            return OSMP_ERROR;
        }

        printf("OSMP process %d received message from %d: %s \n", rank, source, bufout);

        free(bufout);
    }

    if(OSMP_Finalize() == OSMP_ERROR){
        error("[osmpexecutable.c] Test 3 OSMP_Finalize");
        return OSMP_ERROR;
    }
    return OSMP_SUCCESS;
}