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


/* files to get data from */
#include "names1.txt"
#include "names2.txt"
#include "names3.txt"
#include "names4.txt"
#include "names5.txt"

/* code included by Professor Knox */
#include "util.h"
#include "shared_array.h"

/* writeup */
#define MAX_INPUT_FILES 10 /* max of 10 files */
#define MAX_RESOLVER_THREADS 10 /* max 10 resolver threads */
#define MAX_REQUESTER_THREADS 5 /* max 5 requester threads */
#define MAX_NAME_LENGTH 1025 /* include null terminator */
#define MAX_IP_LENGTH INET6_ADDRSTRLEN /* max ip length */

/* write some of the mutexs */
pthread_mutex_t mutex;
pthread_mutex_t write1; /* blocking for one write */
pthread_mutex_t write2; /* second blcoking for starvation */
pthread_mutex_t read; /* one reader */
pthread_mutex_t queue; /* having the queue, question this */
pthread_mutex_t report; /* error reporting block */
pthread_cond_t signal; /* conditional signal good for waiting threads */

int requestThreads = 0; /* number of request threads completed */
int counter; /* counter for read*/
int debug = 1; /* set this to one for debugging purposes */
int numRequestThreads = -1 /* shows up later */
FILE* foutput = NULL; /* set this equal to null */

/* functions */
void *request_thread(void *filename); /* function to get the requester threads */
void *resolve_thread(void *filename); /* function for the resolver threads */

#endif