# My Malloc

Implements a simplified version of the standard library functionsmalloc() and free(). This version allocates and frees memory from a global 4096 byte array. 
Unlike the standard implementations, this version detects common usage errors and reports them.

It detects the following errors and prints out the error messages listed: 
    (1) Double freeing
            “ERROR: File: filename: Line Number: line number: Already freed!” 
    (2) Allocating 0 bytes: 
            “ERROR: File: filename: Line Number: line number: Attempting to allocate 0 bytes” 
    (3) Allocating size that is larger than memory itself (includes negative size malloc calls)
            “ERROR: File: filename: Line Number: line number: Not enough space to fit in memory!”
    (4) Freeing at an address that doesn’t point to the first element of a chunk 
            “ERROR: File: filename: Line Number: line number: Not a valid pointer!”
    (5) Freeing at an address that was not returned by malloc 
            “ERROR: File: filename: Line Number: line number: Address not a valid pointer from malloc!”
