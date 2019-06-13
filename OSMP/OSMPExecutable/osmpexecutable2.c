//
// Created by Gurur Öndarö and Thomas Kraaibeek on 12.06.19.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "../OSMPLib/OSMPLib.h"

/**
 *
 * @param argc
 * @param argv
 */
void test000(int argc, char *argv[]);
/**
 *
 * @param argc
 * @param argv
 */
void test00(int argc, char *argv[]);

/**
 * Test 01: Keine der OSM OSMP-Funktionen sollte ohne OSMP_Init aufzurufen, oder nach OSMP_Finalize verwendbar sein.
 * @param argc Name dieser Datei
 * @param argv Alle Argumente, die auch dem OSMPStarter übergeben wurden
 */
void test01(int argc, char** argv);

/**
 *
 * @param argc
 * @param argv
 */
void test02(int argc, char** argv);


/**
 * Ausführbare Datei, die von allen Prozessen geladen wird. Die Prozesse nutzen OSMPLib-Routinen um miteinander zu kommunizieren
 * @param argc Name dieser Datei
 * @param argv Alle Argumente, die auch dem OSMPStarter übergeben wurden
 * @return Success oder Error
 */
int main(int argc, char** argv) {

    printf("Test000..\n");
    //test000(argc,argv);
    printf("Test00..\n");
    //test00(argc,argv);
    printf("Test01..\n");
    //test01(argc, argv);
    printf("Test02\n");
    test02(argc,argv);
    printf("Test03\n");


    return 1;
}

void test000(int argc, char *argv[])
{
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
        rv = OSMP_Send( bufin, 2, OSMP_INT, 1 );
    }
    else
    { // OSMP process 1
        rv = OSMP_Recv( bufout, 2, OSMP_INT, &source, &len );
        printf("OSMP process %d received %d byte from %d [%d:%d] \n",rank, len, source, bufout[0], bufout[1]);
    }
    rv = OSMP_Finalize();
}


void test00(int argc, char *argv[])
{
    int rv, size, rank, source, len;
    char *bufin, *bufout;
    OSMP_Request myrequest;
    rv = OSMP_Init( &argc, &argv );
    rv = OSMP_Size( &size );
    rv = OSMP_Rank( &rank );
    if( size != 2 ) { /* Fehlerbehandlung */ }
    if( rank == 0 )
    { // OSMP process 0
        bufin = malloc(OSMP_MAX_PAYLOAD_LENGTH); // check for != NULL
        if(bufin==NULL) {

        }
        strncpy(bufin, "hello world",OSMP_MAX_PAYLOAD_LENGTH);
        rv = OSMP_Send( bufin, strlen(bufin), OSMP_CHAR, 1 );
    }
    else
    { // OSMP process 1
        bufout = malloc(OSMP_MAX_PAYLOAD_LENGTH); // check for != NULL
        rv = OSMP_CreateRequest( &myrequest );
        rv = OSMP_Irecv( bufout, OSMP_MAX_PAYLOAD_LENGTH, OSMP_CHAR, &source, &len, myrequest );
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

}


void test01(int argc, char** argv){


    int rv = 0, size = 0, rank = 0, source, len;
    char* bufin="Test Message";
    char* bufout;
    //*in = 5;
    //printf("im exec: %d\n", atoi(argv[2]));
    fflush(stdout);


    if(OSMP_Size(&size)==OSMP_ERROR) debug("[osmpexecutable2.c] OSMP_Size");
    if(OSMP_Rank(&rank)==OSMP_ERROR) debug("[osmpexecutable2.c] OSMP_Rank");

    if(OSMP_Send(bufin, OSMP_MAX_PAYLOAD_LENGTH, OSMP_INT, rank)==OSMP_ERROR) error("[osmpexecutable2.c] OSMP_Send");

    bufout = calloc(1,OSMP_MAX_PAYLOAD_LENGTH);
    if(bufout==NULL) error("[osmpexecutable2.c] Calloc fail");

    if(OSMP_Recv(bufout, OSMP_MAX_PAYLOAD_LENGTH, OSMP_INT, &source, &len)==OSMP_ERROR) error("[osmpexecutable2.c] OSMP_Recv");

    //printf("Recieved from %d : %s\n",source,bufout);


    if(OSMP_Finalize()==OSMP_ERROR) error("[osmpexecutable2.c] OSMP_Finalize");
    fflush(stdout);


    //Part2
    if(OSMP_Init(&argc,&argv)==OSMP_ERROR) error("[osmpexecutable2.c] OSMP_Init");
    if(OSMP_Finalize()==OSMP_ERROR) error("[osmpexecutable2.c] OSMP_Finalize");


    if(OSMP_Size(&size)==OSMP_ERROR) error("[osmpexecutable2.c] OSMP_Size");
    if(OSMP_Rank(&rank)==OSMP_ERROR) error("[osmpexecutable2.c] OSMP_Rank");

    if(OSMP_Send(bufin, OSMP_MAX_PAYLOAD_LENGTH, OSMP_INT, rank)==OSMP_ERROR) error("[osmpexecutable2.c] OSMP_Send");

    bufout = calloc(1,OSMP_MAX_PAYLOAD_LENGTH);
    if(bufout==NULL) error("[osmpexecutable2.c] Calloc fail");

    if(OSMP_Recv(bufout, OSMP_MAX_PAYLOAD_LENGTH, OSMP_INT, &source, &len)==OSMP_ERROR) error("[osmpexecutable2.c] OSMP_Recv");
    //printf("Recieved from %d : %s\n",source,bufout);
    fflush(stdout);
}

void test02(int argc,char** argv){

    int rv, size, rank, source, len;
    char *bufin, *bufout;
    OSMP_Request myrequest;
    rv = OSMP_Init(&argc, &argv);
    if(rv==OSMP_ERROR){
        debug("ERROR OSMP_INIT");
    }
    rv = OSMP_Size( &size );
    rv = OSMP_Rank( &rank );
    if( size != 2 ) { /* Fehlerbehandlung */ }
    if( rank == 0 )
    { // OSMP process 0

    }
    else
    { // OSMP process 1
        bufout = malloc(OSMP_MAX_PAYLOAD_LENGTH); // check for != NULL
        if(bufout==NULL){
            error("bufout null");
            return;
        }
        rv = OSMP_CreateRequest( &myrequest );
        rv = OSMP_Irecv( bufout, OSMP_MAX_PAYLOAD_LENGTH, OSMP_CHAR, &source, &len, myrequest );
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

}