CC=gcc
OUTPUT=osmpexecutable
SOURCE=osmpexecutable.c
LIB=../OSMPLib/OSMPLib.a -lrt -lpthread

osmpexecutable: $(SOURCE)
	$(CC) -o $(OUTPUT) $(SOURCE) $(LIB)

.PHONY: clean

clean:
	rm -f $(OUTPUT) 
