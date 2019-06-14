//
// Created by gurur on 13.06.19.
//

#ifndef OSMP_OSMPEXECUTABLE2_H
#define OSMP_OSMPEXECUTABLE2_H

int osmp_int = OSMP_INT;
int osmp_char = OSMP_BYTE;

/**
 *
 * @param argc
 * @param argv
 */
int Send_Recv(int argc, char *argv[]);
/**
 *
 * @param argc
 * @param argv
 */
int Send_Irecv(int argc, char **argv);

int Isend_Irecv(int argc, char *argv[]);

int Isend_Recv(int argc, char *argv[]);

/**
 * Test 01: Keine der OSM OSMP-Funktionen sollte ohne OSMP_Init aufzurufen, oder nach OSMP_Finalize verwendbar sein.
 * @param argc Name dieser Datei
 * @param argv Alle Argumente, die auch dem OSMPStarter übergeben wurden
 */
int test01(int argc, char** argv);

/**
 *
 * @param argc
 * @param argv
 */
int test02(int argc, char** argv);




/**
 * Ausführbare Datei, die von allen Prozessen geladen wird. Die Prozesse nutzen OSMPLib-Routinen um miteinander zu kommunizieren
 * @param argc Name dieser Datei
 * @param argv Alle Argumente, die auch dem OSMPStarter übergeben wurden
 * @return Success oder Error
 */
int main(int argc, char** argv);



#endif //OSMP_OSMPEXECUTABLE2_H
