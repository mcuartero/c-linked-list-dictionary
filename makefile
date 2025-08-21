CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2

OBJS = main.o read.o search.o output.o

.PHONY: all clean

all: dict1

dict1: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

main.o: main.c read.h search.h output.h
read.o: read.c read.h
search.o: search.c search.h read.h
output.o: output.c output.h read.h

clean:
	rm -f *.o dict1
