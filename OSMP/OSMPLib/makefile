CC=gcc
NAME=OSMPLib

osmplib:	$(NAME).c
	$(CC) -c $(NAME).c
	ar -cvq $(NAME).a $(NAME).o

clean:	
	rm -f $(NAME).o $(NAME).a 
