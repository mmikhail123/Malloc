CC     = gcc
CFLAGS = -std=c99 -g -Wall -fsanitize=address,undefined


mymalloc.o: mymalloc.c mymalloc.h
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -rf *.o <program>
