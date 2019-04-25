
#include "OSMP.h"

int main(int argc, char** argv){

    int rv,size;

    rv = OSMP_Init(&argc,&argv);
    rv = OSMP_Size(&size);
    printf("%d\n",(int)size);

	return 1;
}
