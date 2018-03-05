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

/* writeup */
#define MAX_INPUT_FILES 10 /* max of 10 files */
#define MAX_RESOLVER_THREADS 10 /* max 10 resolver threads */
#define MAX_REQUESTER_THREADS 5 /* max 5 requester threads */
#define MAX_NAME_LENGTH 1025 /* include null terminator */
#define MAX_IP_LENGTH INET6_ADDRSTRLEN /* max ip length */

/* write some of the mutexs */
pthread_mutex_t mutex_wait; /* make a wait mutex */
pthread_mutex_t mutex_full; /* lock the bb if its full */
pthread_cond_t signal; /* conditional signal good for waiting threads */
queue process_queue; /* the queue where we will store our processes */
int counter; /* counter */

/* functions */
void *requester(void *filename); /* function to get the requester threads */
void *resolver(void *filename); /* function for the resolver threads */

#endif