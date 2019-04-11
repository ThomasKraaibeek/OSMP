#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <errno.h>
#include <string.h>

int main() {

    char buffer[1024];

    char* arguments[] = {"Hallo", "Welt", "!", NULL};

    pid_t pid;
    pid=fork();
    if(pid<0){
        //Fehler
	    printf("Fehler bei der Prozessaufteilung\n");
    }
    if(pid==0){
        //Child

        printf("pid_child: %d\n", getpid());

        for(int i=0;i<5;i++) {
            printf("Childprocess [%d]\n", i);
            sleep(1);
            execl("../EchoAll/echoall","Hello","World","123",(char*)NULL);
            getcwd(buffer, 1024);
            printf("cwd: %s\n", buffer);
            //execlp("./a.out","",NULL);
            printf("Exec.. returned, ERROR: %s\n",strerror(errno));
            //execv("/home/ms/g/go744355/Schreibtisch/bs_praktikum/Praktikum1/EchoAll/echoall", arguments);

        }
    }
    else if(pid>0) {
        //Parent

        printf("pid_parent: %d\n", getpid());

        for(int i=0;i<5;i++) {
            printf("Parentprocess [%d]\n", i);
            waitpid(pid,NULL,0);

        }
    }
    return 0;
}
