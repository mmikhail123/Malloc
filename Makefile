CC     = gcc
CFLAGS = -std=c99 -g -Wall -fsanitize=address,undefined

all: <program>

<program>: <program>.o mymalloc.o
	$(CC) $(CFLAGS) $^ -o $@

<program>.o: <program>.c mymalloc.h
	$(CC) -c $(CFLAGS) $< -o $@

mymalloc.o: mymalloc.c mymalloc.h
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -rf *.o <program>
