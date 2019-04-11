#include <stdio.h>
#include <unistd.h>
#include <wait.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>

int main(int argc,char** argv) {

    char buf[100];



    printf("#args %d\n",argc);
    for (int i = 0; i < argc; i++)
    {
        //printf("argv[%d]: %s\n", i, argv[i]);
    }
    int iter = atoi(argv[1]);
    //printf("Iter: %d\n",iter);
    pid_t pids[iter];
    pid_t pid;
    for(int i=0;i<iter;i++){
        pid = fork();
        if(pid<0){
            //Fehler
            printf("Fehler bei der Prozessaufteilung: %s\n",strerror(errno));
            return 1;
        }
        if(pid==0){
            //Child
            sprintf(buf, "%d", i);
            execlp("./osmpexecutable",buf ,NULL);
            return 1;
        }
        else if(pid>0) {

            //Parentpid immer gleich
            printf("p_pid: %d\n", getpid());
            pids[i] = pid;
            //wait(NULL);
           //printf("Parent");
        }

    }



    for(int i=0;i<iter;i++){
        waitpid(pids[i],NULL,0);
    }
















    /*
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
    }*/
    return 0;
}
