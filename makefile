CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
OBJS = main.o read.o search.o bit.o

dict1: $(OBJS)
	$(CC) $(CFLAGS) -o dict1 $(OBJS)

main.o: main.c read.h search.h
	$(CC) $(CFLAGS) -c main.c

read.o: read.c read.h row.h
	$(CC) $(CFLAGS) -c read.c

search.o: search.c search.h row.h
	$(CC) $(CFLAGS) -c search.c

bit.o: bit.c bit.h
	$(CC) $(CFLAGS) -c bit.c

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: clean