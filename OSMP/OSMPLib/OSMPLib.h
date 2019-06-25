//
// Created by Gurur Öndarö and Thomas Kraaibeek 24.04.19.
//

#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* for O_* constans*/
#include <sys/types.h>
#include <semaphore.h>
#include <pthread.h>

#ifndef OSMP_OSMPLIB_H
#define OSMP_OSMPLIB_H

#define OSMP_SUCCESS 0
#define OSMP_ERROR -1

#define DEBUG

typedef void* OSMP_Datatype;
typedef void* OSMP_Request;

#define OSMP_MAX_MESSAGES_PROC   16   // maximale Zahl der Nachrichten pro Prozess
#define OSMP_MAX_SLOTS           256  // maximale Anzahl der Nachrichten, die insgesamt vorhanden sein dürfen
#define OSMP_MAX_PAYLOAD_LENGTH  1024 // maximale Länge der Nutzlast einer Nachricht

enum  {OSMP_INT, OSMP_SHORT, OSMP_LONG, OSMP_BYTE, OSMP_UNSIGNED_CHAR, OSMP_UNSIGNED_SHORT, OSMP_UNSIGNED, OSMP_FLOAT, OSMP_DOUBLE };

typedef struct{
    int src;
    char data[OSMP_MAX_PAYLOAD_LENGTH];
    OSMP_Datatype type;
    size_t len;
    int nextmsg;
    
} message;

typedef struct{
    int firstmsg;
    int lastmsg;
    pid_t pid;
    sem_t freeslots;
    sem_t fullslots;
    sem_t mutex;
} process;

typedef struct{
    int firstempty;
    int lastempty;
    sem_t free_slot;
    sem_t mutex;
} emptyslot;

typedef struct{
    process emptymsg;
    message msg[OSMP_MAX_SLOTS];
    int processAmount;
    process p[];

}shm;

/**
  * Die Routine OSMP_Init() initialisiert die OSMP-Umgebung und ermöglicht den Zugang zu den gemeinsamen Ressourcen der OSMP-Prozesse.
  * Sie muss von jedem OSMP-Prozess zu Beginn aufgerufen werden.
  * @param argc Adresse der Argumentzahl
  * @param argv Adresse des Argumentvektors
  * @return 1=success, 0=fail
  */
int OSMP_Init(int *argc, char ***argv);

/**
 * Die Routine liefert in *size die Zahl der OSMP-Prozesse ohne den OSMP-Starter Prozess zurück.
 * Sollte mit der Zahl übereinstimmen, die in der Kommandozeile dem OSMP-Starter übergeben wird.
 * @param size Zahl der OSMP-Prozesse (output)
 * @return OSMP_SUCCESS oder OSMP_ERROR
 */
int OSMP_Size(int *size);

/**
 * Die Routine liefert in *rank die OSMP-Prozessnummer des aufrufenden OSMP-Prozesses von 0,...,np-1 zurück.
 * @param rank Prozessnummer 0,...,np-1 des aktuellen OSMP-Prozesse (output)
 * @return OSMP_SUCCESS oder OSMP_ERROR
 */
int OSMP_Rank(int *rank);

/**
 * Die Routine sendet eine Nachricht an den Prozess mit der Nummer dest. Die Nachricht besteht aus count Elementen vom Typ datatype.
 * Die zu sendende Nachricht beginnt im aufrufenden Prozess bei der Adresse buf.
 * Die Routine ist blockierend, d.h. wenn sie in das aufrufende Programm zurückkehrt, ist der Kopiervorgang abgeschlossen.
 * @param buf Startadresse des Puffers mit der zu sendenden Nachricht (input)
 * @param count Zahl der Elemente vom angegebenen Typ im Puffer (input)
 * @param datatype OSMP-Typ der Daten im Puffer (input)
 * @param dest Nummer des Empfängers zwischen 0,...,np-1 (input)
 * @return OSMP_SUCCESS oder OSMP_ERROR
 */
int OSMP_Send(const void *buf, int count, OSMP_Datatype datatype, int dest);

/**
 * Der aufrufende Prozess empfängt eine Nachricht mit maximal count Elementen des angegebenen Datentyps datatype.
 * Die Nachricht wird an die Adresse buf des aufrufenden Prozesses geschrieben.
 * Unter source wird die OSMP-Prozessnummer des sendenden Prozesses und unter len die tatsächliche Länge der gelesenen Nachricht abgelegt.
 * Die Routine ist blockierend, d.h. sie wartet, bis eine Nachricht für den Prozess vorhanden ist.
 * Wenn die Routine zurückkehrt, ist der Kopierprozess abgeschlossen.
 * @param buf Startadresse des Puffers im lokalen Speicher des aufrufenden Prozesses, in den die Nachricht kopiert werden soll. (input)
 * @param count maximale Zahl der Elemente vom angegebenen Typ, die empfangen werden können (input)
 * @param datatype OSMP-Typ der Daten im Puffer (input)
 * @param source Nummer des Senders zwischen 0,...,np-1 (output)
 * @param len tatsächliche Länge der empfangenen Nachricht in Byte (output)
 * @return OSMP_SUCCESS oder OSMP_ERROR
 */
int OSMP_Recv(void *buf, int count, OSMP_Datatype datatype,  int *source, int *len);

/**
 * Alle OSMP-Prozesse müssen diese Routine aufrufen, bevor sie sich beenden.
 * Sie geben damit den Zugriff auf die gemeinsamen Ressourcen frei.
 * @return OSMP_SUCCESS oder OSMP_ERROR
 */
int OSMP_Finalize(void);


/**
 * Die Routine sendet eine Nachricht analog zu OSMP_Send(). Die Routine kehrt jedoch sofort zurück,
 * ohne dass das Kopieren der Nachricht sichergestellt ist (nicht blockierendes Senden).
 * @param buf Startadresse des Puffers mit der zu sendenden Nachricht (input)
 * @param count Zahl der Elemente vom angegebenen Typ im Puffer (input)
 * @param datatype OSMP-Typ der Daten im Puffer (input)
 * @param dest Nummer des Empfängers zwischen 0,...,np-1 (input)
 * @param request Adresse einer eigenen Datenstruktur, die später verwendet werden kann, um abzufragen, ob die Operation abgeschlossen ist.
 * @return OSMP_SUCCESS oder OSMP_ERROR
 */
int OSMP_Isend(const void *buf, int count, OSMP_Datatype datatype, int dest, OSMP_Request request);

/**
 * Die Routine empfängt eine Nachricht analog zu OSMP_Recv(). Die Routine kehrt jedoch sofort zurück,
 * ohne dass das Kopieren der Nachricht sichergestellt ist (nicht blockierendes Empfangen).
 * @param buf Startadresse des Puffers im lokalen Speicher des aufrufenden Prozesses, in den die Nachricht kopiert werden soll. (input)
 * @param count maximale Zahl der Elemente vom angegebenen Typ, die empfangen werden können (input)
 * @param datatype OSMP-Typ der Daten im Puffer (input)
 * @param source Nummer des Senders zwischen 0,...,np-1 (output)
 * @param len tatsächliche Länge der empfangenen Nachricht in Byte (output)
 * @param request Adresse einer eigenen Datenstruktur, die später verwendet werden kann, um abzufragen, ob die Operation abgeschlossen ist.
 * @return OSMP_SUCCESS oder OSMP_ERROR
 */
int OSMP_Irecv(void *buf, int count, OSMP_Datatype datatype, int *source, int *len, OSMP_Request request);

/**
 * Die Routine testet, ob die mit dem request verknüpfte Operation abgeschlossen ist. Sie ist nicht blockierend,
 * d.h. sie wartet nicht auf das Ende der mit request verknüpfte Operation.
 * @param request Adresse der Struktur, die eine blockierende Operation spezifiziert (input)
 * @param flag Gibt an, ob Operation abgeschlossen ist oder nicht (output)
 * @return OSMP_SUCCESS oder OSMP_ERROR
 */
int OSMP_Test(OSMP_Request request, int *flag);

/**
 * Die Routine prüft, ob die mit dem request verknüpfte, nicht blockierende Operation abgeschlossen ist.
 * Sie ist so lange blockiert, bis dies der Fall ist.
 * @param request Adresse der Struktur, die eine nicht blockierende Operation spezifiziert (input)
 * @return OSMP_SUCCESS oder OSMP_ERROR
 */
int OSMP_Wait (OSMP_Request request);


/**
 * Die Routine stellt den Speicher für einen Request zur Verfügung.
 * @param request
 * @return OSMP_SUCCESS oder OSMP_ERROR
 */
int OSMP_CreateRequest(OSMP_Request *request);

/**
 * Die Routine dealloziert den Speicher eines Requests.
 * @param request Adresse eines Requests (input)
 * @return OSMP_SUCCESS oder OSMP_ERROR
 */
int OSMP_RemoveRequest(OSMP_Request *request);



/**
 * Fehlerbehandlung
 * @param msg eigene Fehlermessage
 * @param ...
 * @return Statuscode OSMP_ERROR
 */
void error(char* msg, ...);

/**
 * Dient zum Ausgeben einer Message
 * @param message auszugebene Message
 * @param ...
 */
void debug(char* message, ...);

/**
 * Konvertiert Integer zu String
 * @param value zu konvertierende Zahl
 * @return konvertierte Zahl
 */
char* itos(int value);


#endif //OSMP_OSMPLIB_H
