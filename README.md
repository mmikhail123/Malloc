# My Malloc

Implements a simplified version of the standard library functionsmalloc() and free(). This version allocates and frees memory from a global 4096 byte array. 
Unlike the standard implementations, this version detects common usage errors and reports them.

It detects the following errors and prints out the error messages listed: 

- Double freeing:
    “ERROR: File: filename: Line Number: line number: Already freed!” 
            
- Allocating 0 bytes: 
    “ERROR: File: filename: Line Number: line number: Attempting to allocate 0 bytes” 

- Allocating size that is larger than memory itself (includes negative size malloc calls):
    “ERROR: File: filename: Line Number: line number: Not enough space to fit in memory!”

- Freeing at an address that doesn’t point to the first element of a chunk:
    “ERROR: File: filename: Line Number: line number: Not a valid pointer!”

- Freeing at an address that was not returned by malloc:
    “ERROR: File: filename: Line Number: line number: Address not a valid pointer from malloc!”

To use in own programs: 
In order to use this version of malloc, ensure that in the program you are using, include the mymalloc.h header file. 
In the makefile fill in the parameters that have <program> with your program name. This is what is currently written in the makefile to edit:

```
    all: <program>
    
    <program>: <program>.o mymalloc.o
    $(CC) $(CFLAGS) $^ -o $@

    <program>.o: <program>.c mymalloc.h
    $(CC) -c $(CFLAGS) $< -o $@
 ```

                            

