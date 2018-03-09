/*
    JASON LUBRANO
    CSCI 3753
    PA3
    COLLAB: KYLE WARD
    multi-lookup.h
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/syscall.h>

/* file to include handed out by dr. knox */
#include "util.h"

#define SHARED_ARRAY_FAILURE -1
#define SHARED_ARRAY_SUCCESS 0
#define SHARED_ARRAY_MAXSIZE 50
#define MINARGS 3
#define SYNOPSIS "<inputFilePath> <outputFilePath>"
#define BUFFSIZE 1025
#define INPUTFS "%1024s"
#define ARRAY_SIZE_TEST 20
#define MAX_REQ_THS 10
#define MAX_RES_THS 5

/* SHARED ARRAY */

/* the array will have the contained items */
typedef struct SHARED_ARRAY_INDEX_STRUCT{
    void* contains;
} SHARED_ARRAY_INDEX;

/* and this will be the data struct for the pointers in the array */
typedef struct SHARED_ARRAY_STRUCT{
    SHARED_ARRAY_INDEX* array;
    int head;
    int tail;
    int maxSize;
} SHARED_ARRAY;
/* this was a trick i learned from a professor in DS */


/* initalizes the shared array, on success returns 0, else -1 */
int SHARED_ARRAY_INIT(SHARED_ARRAY* s_array, int size);

/* test to see if the shared array is emtpy */
int SHARED_ARRAY_TEST_EMPTY(SHARED_ARRAY* s_array);

/* test if the shared array is full */
int SHARED_ARRAY_TEST_FULL(SHARED_ARRAY* s_array);

/* add an item to the shared array */
int SHARED_ARRAY_ADD_ITEM(SHARED_ARRAY* s_array, void* contains);

/* remove an item from the shared array */
void* SHARED_ARRAY_REMOVE_ITEM(SHARED_ARRAY* s_array);

/* Function to free shared array allocated memory */
void SHARED_ARRAY_CLEAR(SHARED_ARRAY* s_array);

/* THREADS */

/* struct for the dynamic array */
struct DYNAMIC_ARRAY_STRUCT{
    int size;
    FILE *array[5];
};

/* struct for teh requester threads */
struct REQ_TH_DATA_STRUCT{
    long threadNum;
    int filesServiced;
    struct DYNAMIC_ARRAY_STRUCT inFiles;
    SHARED_ARRAY* s_array;
};

/* struct for the resolver threads */
struct RES_TH_DATA_STRUCT{
    FILE* outFile;
    SHARED_ARRAY* s_array;
};

/* thread that handles the requester threads */
void* REQUEST_THREAD(void* threadarg);

/* thread that handles the resolver threads */
void* RESOLVE_THREAD(void* threadarg);

/* function included by professor knox */
long long gettimeofday_func();

/* main */
int main(int argc, char* argv[]);

