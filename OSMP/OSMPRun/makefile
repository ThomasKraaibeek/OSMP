CC=gcc -Wall -Wconversion -std=c99 -D_XOPEN_SOURCE=500
SRC=OSMPStarter.c
LIB=../OSMPLib/OSMPLib.a -lrt -lpthread
OUT=osmprun

osmprun: $(SRC)
	$(CC) -o $(OUT) $(SRC) $(LIB)

clean:
	rm -f $(OUT) 
