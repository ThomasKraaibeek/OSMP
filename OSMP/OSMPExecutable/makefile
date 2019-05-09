CC=gcc -g -Wall -Wconversion -std=c99 -D_XOPEN_SOURCE=500
OUTPUT=osmpexecutable
SOURCE=osmpexecutable.c
LIB=../OSMPLib/OSMPLib.a -lrt -lpthread

osmpexecutable: $(SOURCE)
	$(CC) -o $(OUTPUT) $(SOURCE) $(LIB)

.PHONY: clean

clean:
	rm -f $(OUTPUT) 
