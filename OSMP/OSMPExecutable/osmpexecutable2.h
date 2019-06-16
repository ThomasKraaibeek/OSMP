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
 * @return rv
 */
int test01(int argc, char** argv);

/**
 * Test 02: Der Empfänger soll beim Empfangen einer Nachricht blockieren, wenn keine Nachricht vorhanden ist.
 * @param argc
 * @param argv
 * @return rv
 */
int test02(int argc, char** argv);

/**
 * Test 03: Der Sender sollte blockieren wenn der Empfänger keine Nachrichten empfangen kann.
 * @param argc
 * @param argv
 * @return rv
 */
int test03(int argc, char** argv);

/**
 * Test 04: Der Sender könnte größere Nachrichten senden als der Empfänger empfangen kann, oder sendet längere Nachrichten
    als die maximale erlaubte Länge. Was passiert jeweils?
 * @param argc
 * @param argv
 * @return rv
 */
int test04(int argc, char** argv);

/**
 * Test 05: Es werden mehr Nachrichten verschickt als die globalen Slots aufnehmen können.
 * @param argc
 * @param argv
 * @return rv
 */
int test05(int argc, char** argv);

/**
 * Test 06: Wie erfährt der Aufrufer, wenn beim Starten eines asynchronen Send oder Receive ein Fehler auftritt?
 * @param argc
 * @param argv
 * @return rv
 */
int test06(int argc, char** argv);


/**
 * Ausführbare Datei, die von allen Prozessen geladen wird. Die Prozesse nutzen OSMPLib-Routinen um miteinander zu kommunizieren
 * @param argc Name dieser Datei
 * @param argv Alle Argumente, die auch dem OSMPStarter übergeben wurden
 * @return Success oder Error
 */
int main(int argc, char** argv);



#endif //OSMP_OSMPEXECUTABLE2_H
