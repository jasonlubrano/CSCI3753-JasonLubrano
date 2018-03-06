/* Jason Lubrano
 * multi-lookup.h
 * PA3
 */

#ifndef multi_lookup_h
#define multi_lookup_h

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/syscalls.h>

/* file for the shared array */
#include "shared_array.h"

/* files to get data from */
#include "names1.txt"
#include "names2.txt"
#include "names3.txt"
#include "names4.txt"
#include "names5.txt"

/* code included by Professor Knox */
#include "util.h"

/* writeup */
#define MAX_INPUT_FILES 10 /* max of 10 files */
#define MAX_RESOLVER_THREADS 10 /* max 10 resolver threads */
#define MAX_REQUESTER_THREADS 5 /* max 5 requester threads */
#define MAX_NAME_LENGTH 1025 /* include null terminator */
#define MAX_IP_LENGTH INET6_ADDRSTRLEN /* max ip length */
#define MIN_ARGS 3 /* min num of arguments */
#define PATH "<inputFilePath> <outputFilePath>"
#define BUFFSIZE 1025
#define INPUTFS "%1024s"
#define EXIT_FAILURE -1 /* returns this whenever we fail */

/* structs */
struct dynamicArray{
	FILE *array[5];
	int size;
};

struct dataRequest{
	long threadNum;
	struct dynamicArray inputFiles;
	array_stack* s_arr;
	int servicedFiles;
};

struct dataResolve{
	FILE* fileOutput;
	array_stack* s_arr;
};

/* functions */
void *thread_requester(void *threadarg); /* function to get the requester threads */
void *thread_resolver(void *threadarg); /* function for the resolver threads */
long long gettimeofday_func(); /* syscall to get the time */

int main(int argc, char* argv[]);


#endif