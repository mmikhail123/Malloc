#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <time.h>

#include "mymalloc.h"

#define MAX_SIZE 4096

static char memory[MAX_SIZE];

enum state {Free = -1, Allocated = -2};

//prevents the addition of buffer bytes so that a 2-byte or 4-byte allocation does not each take 8-bytes of space
#pragma pack(1)
typedef struct Metadata{
    enum state current_state;
    short int chunk_size;
}metadata;

static metadata* newMetadata(short int size){
    int remainingMemory = MAX_SIZE;
    void* incrementPtr = &memory;
    metadata* metadataPtr = (metadata *) incrementPtr;
    short int metadataSize = (short) sizeof(metadata);
    
    //checks to see if theres enough space to fit size requested 
    while(remainingMemory >= size + sizeof(metadata)){
        //checks to see if chunk is allcoated
        if(metadataPtr->current_state == Allocated){
            //move on to the next chunk
            remainingMemory = remainingMemory - sizeof(metadata) - metadataPtr->chunk_size;
            incrementPtr = incrementPtr + sizeof(metadata) + (metadataPtr->chunk_size);
            metadataPtr = (metadata *) incrementPtr;
        }
        //checks to see if chunk is free
        else if(metadataPtr->current_state == Free){
            //checks to see if free chunk can fit size requested
            if(metadataPtr->chunk_size > size){
                //checks to see if theres enough space to split large free chunk into 2 chunks
                if(metadataPtr->chunk_size - metadataSize - size > 0){
                    void* temp_ptr = incrementPtr + sizeof(metadata) + size;
                    metadata* newFree = (metadata *) temp_ptr;
                    newFree->current_state = Free;
                    newFree->chunk_size = metadataPtr->chunk_size - size - metadataSize;
                    metadataPtr->chunk_size = size;
                    metadataPtr->current_state = Allocated;
                }
                else{
                    //if not possible, allocate entire chunk
                    metadataPtr->current_state = Allocated;
                }
                return metadataPtr;
            }
            else{
                //since free chunk cannot fit size requested, move on to next chunk
                remainingMemory = remainingMemory - sizeof(metadata) - metadataPtr->chunk_size;
                incrementPtr = incrementPtr + sizeof(metadata) + (metadataPtr->chunk_size);
                metadataPtr = (metadata *) incrementPtr;
            }
        }
    }
    return NULL;
}

static void initialize(){
    void* ptr = &memory;
    metadata* metadataPtr = (metadata *) ptr;
    if(metadataPtr->chunk_size == 0){
        //sets the first free chunk to encompass entire memory block in instances where memory had yet to be allocated.
        metadataPtr->current_state = Free;
        short int metadataSize = sizeof(metadata);
        metadataPtr->chunk_size = MAX_SIZE - metadataSize;
    }
}

static void coalesce(){
    int remainingMemory = MAX_SIZE;
    void* ptr = &memory;
    metadata* metadataPtr = (metadata *) ptr;
    //A loop condition that should always remain true. Loop breaks through internal processes
    while(metadataPtr->current_state == Free || metadataPtr->current_state == Allocated){
        remainingMemory = remainingMemory - sizeof(metadata) - metadataPtr->chunk_size;
        //if current chunk is the final chunk (as designated by remaining memory), exit loop
        if(remainingMemory == 0){
            break;
        }
        //moves on to next chunk
        if(metadataPtr->current_state == Allocated){
            ptr = ptr + sizeof(metadata) + (metadataPtr->chunk_size);
            metadataPtr = (metadata *) ptr;
        }else if(metadataPtr->current_state == Free){
            ptr = ptr + sizeof(metadata) + (metadataPtr->chunk_size);
            metadata* tempMetadata = (metadata *) ptr;
            //checks if chunk after current is also free and then combines free chunks
            if(tempMetadata->current_state == Free){
                remainingMemory = remainingMemory + sizeof(metadata) + metadataPtr->chunk_size;
                ptr = ptr - sizeof(metadata) - (metadataPtr->chunk_size);
                short int metadataSize = (short) sizeof(metadata);
                short int newSize = metadataPtr->chunk_size + metadataSize + tempMetadata->chunk_size;
                metadataPtr->chunk_size = newSize;
            }
            else if(tempMetadata->current_state == Allocated){
                //moves on to chunk after free 
                metadataPtr = (metadata *) ptr;
            }
        }
    }
}

void *mymalloc(size_t size, char *file, int line){
    initialize();
    //error if allocating 0 bytes 
    if(size == 0){
        fprintf(stderr, "ERROR: File: %s: Line Number %d: Attempting to allocate 0 bytes\n", file, line);
        return NULL;
    }
    //error if attempting to allocate size bigger than memory itself
    if(size >= MAX_SIZE - sizeof(metadata)){
        fprintf(stderr, "ERROR: File: %s: Line Number %d: Not enough space to fit in memory!\n", file, line);
        return NULL;
    }
    short int newSize = (short) size;
    metadata* data = newMetadata(newSize);
    if(data == NULL){
        //since newMetadata returned NULL, error generated as there wasn't enough space to fit size requested
        fprintf(stderr, "ERROR: File: %s: Line Number %d: Not enough space to fit in memory!\n", file, line);
        return NULL;
    }
    else{
        //return pointer of first element of chunk
        void* returnPtr = data;
        return (returnPtr + sizeof(metadata));
    }
    coalesce();
}

void myfree(void *ptr, char *file, int line){
    initialize();
    //checks if pointer is valid address returned from malloc, prints error if not
    if(ptr >= (void *) &memory && ptr <= (void *) &memory + MAX_SIZE){
        int remainingMemory = MAX_SIZE;
        void* incrementPtr = &memory;
        metadata* metadataPtr = (metadata *) incrementPtr;
        void* testPtr = incrementPtr + sizeof(metadata);
        //will continue looping until either all chunks have been checked or the searched ptr/address has been passed
        while(testPtr <= ptr && remainingMemory > 0){
            //check if current chunk results in ptr address
            if(testPtr == ptr){
                //error if chunk is already freed
                if(metadataPtr->current_state == Free){
                    fprintf(stderr, "ERROR: File: %s: Line Number %d: Already freed!\n", file, line);
                }
                //free the chunk
                else if(metadataPtr->current_state == Allocated){
                    metadataPtr->current_state = Free;
                }
                break;
            }
            //otherwise continue to next chunk
            else{
                remainingMemory = remainingMemory - sizeof(metadata) - metadataPtr->chunk_size;
                incrementPtr = incrementPtr + sizeof(metadata) + (metadataPtr->chunk_size);
                testPtr = incrementPtr + sizeof(metadata);
                metadataPtr = (metadata *) incrementPtr;
            }
        }
        //error if ptr was never found
        if(testPtr != ptr){
            fprintf(stderr, "ERROR: File: %s: Line Number %d: Not a valid pointer!\n", file, line);
        }
    }
    else{
        fprintf(stderr, "ERROR: File: %s: Line Number %d: Address not a valid pointer from malloc!\n", file, line);
    }
    coalesce();
}

void checkLeak(){
    coalesce();
    int remainingMemory = MAX_SIZE;
    void* ptr = &memory;
    metadata* metadataPtr = (metadata *) ptr;
    int leakedMem = 0;
    //iterates through all chunks and detects what chunks are still considered allocated. Saves the amount of bytes those chunks encompass onto leakedMem.
    while(metadataPtr->current_state == Free || metadataPtr->current_state == Allocated){
        remainingMemory = remainingMemory - sizeof(metadata) - metadataPtr->chunk_size;
        if(remainingMemory == 0){
            break;
        }
        if(metadataPtr->current_state == Allocated){
            leakedMem = leakedMem + metadataPtr->chunk_size + sizeof(metadata);
        }
        ptr = ptr + sizeof(metadata) + (metadataPtr->chunk_size);
        metadataPtr = (metadata *) ptr;
    }
    printf("Memory leak!: %u bytes of memory are not freed\n", leakedMem);
}