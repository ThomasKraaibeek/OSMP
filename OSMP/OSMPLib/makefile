CC=gcc -g -std=c99 -D_XOPEN_SOURCE=500
NAME=OSMPLib
#-Wall -Wconversion
osmplib:	$(NAME).c
	$(CC) -c $(NAME).c
	ar -cvq $(NAME).a $(NAME).o

clean:	
	rm -f $(NAME).o $(NAME).a 
