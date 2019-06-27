//
// Created by gurur on 13.06.19.
//

#ifndef OSMP_OSMPEXECUTABLE2_H
#define OSMP_OSMPEXECUTABLE2_H


/** Test 0
 * Send-Recv-Kommunikation zwischen zwei Prozessen
 * @param argc
 * @param argv
 */
int Send_Recv(int argc, char *argv[]);

/** Test 1
 * Send-Irecv-Kommunikation zwischen zwei Prozessen
 * @param argc
 * @param argv
 */
int Send_Irecv(int argc, char **argv);

/** Test 2
 * Isend-Irecv-Kommunikation zwischen zwei Prozessen
 * @param argc
 * @param argv
 * @return
 */
int Isend_Irecv(int argc, char *argv[]);

/** Test 3
 * Isend-Recv-Kommunikation zwischen zwei Prozessen
 * @param argc
 * @param argv
 * @return
 */
int Isend_Recv(int argc, char *argv[]);

/** Test 4
 * Test 01: Keine der OSM OSMP-Funktionen sollte ohne OSMP_Init aufzurufen, oder nach OSMP_Finalize verwendbar sein.
 * @param argc Name dieser Datei
 * @param argv Alle Argumente, die auch dem OSMPStarter übergeben wurden
 * @return rv
 */
int test01(int argc, char** argv);

/** Test 5
 * Test 02: Der Empfänger soll beim Empfangen einer Nachricht blockieren, wenn keine Nachricht vorhanden ist.
 * @param argc
 * @param argv
 * @return rv
 */
int test02(int argc, char** argv);

/** Test 6
 * Test 03: Der Sender sollte blockieren wenn der Empfänger keine Nachrichten empfangen kann.
 * @param argc
 * @param argv
 * @return rv
 */
int test03(int argc, char** argv);

/** Test 7
 * Test 04: Der Sender könnte größere Nachrichten senden als der Empfänger empfangen kann, oder sendet längere Nachrichten
    als die maximale erlaubte Länge. Was passiert jeweils?
 * @param argc
 * @param argv
 * @return rv
 */
int test04(int argc, char** argv);

/** Test 8
 * Test 05: Es werden mehr Nachrichten verschickt als die globalen Slots aufnehmen können.
 * @param argc
 * @param argv
 * @return rv
 */
int test05(int argc, char** argv);

/** Test 9
 * Test 06: Wie erfährt der Aufrufer, wenn beim Starten eines asynchronen Send oder Receive ein Fehler auftritt?
 * @param argc
 * @param argv
 * @return rv
 */
int test06(int argc, char** argv);

/** Test 10
 * Test 07: OSMP_RemoveRequest könnte aufgerufen werden obwohl der Requester noch verwendet wird.
 * @param argc
 * @param argv
 * @return rv
 */
int test07(int argc, char** argv);

/** Test 11
 * Test 08: Fehlerhafte Parameter wie Nullpointer, ungültige Prozess-IDs o.ä. sollten nicht zum Absturz führen    
    Z.B. sendet der Sender an einen nicht vorhandenen Prozess.
 * @param argc
 * @param argv
 * @return rv
 */
int test08(int argc, char** argv);

/** Test 12
 * Test 09: Es werden z.B. OSMP_BYTES versendet und OSMP_DOUBLE empfangen, was passiert und was ist mit der Länge?
 * @param argc
 * @param argv
 * @return rv
 */
int test09(int argc, char** argv);

/** Test 13
 * Send-Recv-Kommunikation zwischen beliebig vielen Prozessen
 * @param argc
 * @param argv
 * @return
 */
int test10(int argc, char** argv);

/** Test 14
 * Isend-Irecv-Kommunikation zwischen beliebig vielen Prozessen
 * @param argc
 * @param argv
 * @return
 */
int test11(int argc, char** argv);

/** Test 15
 * Isend-Irecv-Kommunikation zwischen exotischen Datentypen
 * @param argc
 * @param argv
 * @return
 */
int test12(int argc, char** argv);

/** Test 16
 * Request wird mehrmals verwendet
 * @param argc
 * @param argv
 * @return
 */
int test13(int argc, char** argv);

/** Test 17
 * Request wird ohne Create_Request verwendet
 * @param argc
 * @param argv
 * @return
 */
int Send_Irecv_NotInit(int argc, char **argv);

int getRV(OSMP_Request *request);


/**
 * Ausführbare Datei, die von allen Prozessen geladen wird. Die Prozesse nutzen OSMPLib-Routinen um miteinander zu kommunizieren
 * @param argc Name dieser Datei
 * @param argv Alle Argumente, die auch dem OSMPStarter übergeben wurden
 * @return Success oder Error
 */
int main(int argc, char** argv);



#endif //OSMP_OSMPEXECUTABLE2_H
