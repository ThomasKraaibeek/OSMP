CC=gcc -g -Wall -Wconversion -std=c99 -D_XOPEN_SOURCE=500
OUTPUT=osmpexecutable2
SOURCE=osmpexecutable2.c
LIB=../OSMPLib/OSMPLib.a -lrt -lpthread

osmpexecutable2: $(SOURCE)
	$(CC) -o $(OUTPUT) $(SOURCE) $(LIB)

.PHONY: clean

clean:
	rm -f $(OUTPUT) 
