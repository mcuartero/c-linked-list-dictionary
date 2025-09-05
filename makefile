CC      = gcc
CFLAGS  = -Wall -Wextra -std=c99 -O2
OBJS    = main.o read.o search.o bit.o csv.o list.o print.o row.o

# Object builds (keep your explicit style)
dict1: $(OBJS)
	$(CC) $(CFLAGS) -o bin/dict1 $(OBJS)

main.o: main.c read.h search.h print.h list.h row.h bit.h
	$(CC) $(CFLAGS) -c main.c

read.o: read.c read.h row.h csv.h list.h
	$(CC) $(CFLAGS) -c read.c

search.o: search.c search.h row.h
	$(CC) $(CFLAGS) -c search.c

bit.o: bit.c bit.h
	$(CC) $(CFLAGS) -c bit.c

csv.o: csv.c csv.h row.h
	$(CC) $(CFLAGS) -c csv.c

list.o: list.c list.h row.h
	$(CC) $(CFLAGS) -c list.c

print.o: print.c print.h row.h
	$(CC) $(CFLAGS) -c print.c

row.o: row.c row.h
	$(CC) $(CFLAGS) -c row.c

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean