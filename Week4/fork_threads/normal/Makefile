CC = gcc
CFLAGS=-g
LIBS=-lpthread
default: thread_array

thread_array.o: thread_array.c
	$(CC)  $(CFLAGS) -c thread_array.c -o thread_array.o 

thread_array: thread_array.o
	$(CC)  thread_array.o -o thread_array $(LIBS)

clean:
	rm -f *.o core
