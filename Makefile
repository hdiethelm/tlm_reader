CC=gcc
CFLAGS=-g -c -Wall
LDFLAGS=-lm
SOURCES=main.c tlm_reader.c

OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=tlm_reader

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

.PHONY: clean

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)
