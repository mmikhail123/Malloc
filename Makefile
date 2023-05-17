CC     = gcc
CFLAGS = -std=c99 -g -Wall -fsanitize=address,undefined

all: memgrind testprograms

memgrind: memgrind.o mymalloc.o
	$(CC) $(CFLAGS) $^ -o $@

testprograms: testprograms.o mymalloc.o
	$(CC) $(CFLAGS) $^ -o $@

testprograms.o: testprograms.c mymalloc.h
	$(CC) -c $(CFLAGS) $< -o $@

memgrind.o: memgrind.c mymalloc.h
	$(CC) -c $(CFLAGS) $< -o $@

mymalloc.o: mymalloc.c mymalloc.h
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -rf *.o memgrind testprograms