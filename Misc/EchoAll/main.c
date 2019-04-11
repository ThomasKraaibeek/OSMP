#include <stdio.h>
#include <unistd.h>

main(int argc, char** argv){

    //printf("%s",argv[2]);
    printf("#args %d\n",argc);
    for (int i = 0; i < argc; ++i)
    {
        printf("argv[%d]: %s\n", i, argv[i]);
    }
}
