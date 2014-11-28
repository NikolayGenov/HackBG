CC = gcc
CFLAGS=-g
LIBS=-lpthread
default: server_battleship

server_battleship.o: server_battleship.c
	$(CC)  $(CFLAGS) -c server_battleship.c -o server_battleship.o 

server_battleship: server_battleship.o
	$(CC)  server_battleship.o -o server_battleship $(LIBS)

clean:
	rm -f *.o core
