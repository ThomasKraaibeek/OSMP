CC=gcc
SRC=OSMPStarter.c
LIB=../OSMPLib/OSMPLib.a -lrt -lpthread
OUT=osmprun

osmprun: $(SRC)
	$(CC) -o $(OUT) $(SRC) $(LIB)

clean:
	rm -f $(OUT) 
