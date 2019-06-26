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
