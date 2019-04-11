#include <stdio.h>
int main(int argc,char** argv){
	for (int i = 0; i < argc; i++)
	{
		//printf("argv[%d]: %s\n", i, argv[i]);
	}
	//printf("Waiting for %s\n",argv[0]);

	if(atoi(argv[0]) == 2)
		sleep(20);
	printf("Hello World No. %s\n",(argv[0]));
	return 1;
}
