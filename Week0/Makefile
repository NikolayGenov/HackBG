CC = gcc
CFLAGS=-Wall

default: program

program.o: program.c
	$(CC) $(CFLAGS) -c program.c -o program.o

program: program.o
	$(CC) program.o -o program

clean:
	        rm -f *.o core
