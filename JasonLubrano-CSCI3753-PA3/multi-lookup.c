/* Jason Lubrano
 * multi-lookup.c
 * PA3
 */


#include <stdlib.h>

#include "multi-lookup.h"

/* write some of the mutexs */
pthread_mutex_t mutex;
pthread_mutex_t mutex_write1; /* blocking for one write */
pthread_mutex_t mutex_write2; /* second blcoking for starvation */
pthread_mutex_t mutex_read; /* one reader */
pthread_mutex_t mutex_array; /* having the array, question this */
pthread_mutex_t mutex_report; /* error reporting block */
pthread_cond_t condition_signal; /* conditional signal good for waiting threads */

/* global vars */
int completedRequestThreads = 0; /* number of request threads completed */
int readcount; /* counter for read*/
int debug = 1; /* set this to one for debugging purposes */
int numRequestThreads = -1 /* shows up later */
FILE* foutput = NULL; /* set this equal to null */


/* function to get the requester threads */
void *thread_requester(void *threadarg){
	if(debug){
		printf("debug: inside thread_requester function\n");
	}

	/* starting off the local vars */
	struct dataRequest * requestData; /* ptr for data */
	char hostname[BUFFSIZE]; /* hostname */
	FILE* infilep; /* input file pointer */
	array_stack* s_arr; /* create the shared array */
	char* contains; /* what we put in the index */
	long threadNum; /* thread number */

	if(debug){
		printf("debug: decalred local varrs\n");
	}

	pid_t pidt = syscall(__NR_gettid);

	if(debug){
		printf("debug: syscall, pidt: %d \n", pidt);
	}

	/* lock the mutex */
	pthread_mutex_lock(&mutex_write2);

		/* open up out file, see if it failed */
		foutput = fopen("serviced.txt", "ab");
		if(!foutput){
			perror("ERROR> Opening foutput failed")
		}

		fprintf(foutput, "Thread %d service %d files.\n", pidt, requestData->servicedFiles);

		/* close the file */
		close(foutput);

	/* unlcok the mutex */
	pthread_mutex_unlock(&mutex_write2);


	for(int i = 0; i < requestData->inputFiles.size; i++){
		/* put the data from the input files to a struct */
		infilep = requestData->inputFiles.array[i];
		/* add the item form one array to the node */
		s_arr = requestData->s_arr;
		threadNum = requestData->threadNum;

		/* open the input file, add it to stack */
		if(!infilep){
			printf("ERROR> Opening input file (infilep) failed\n");
			pthread_exit(NULL);
		}

		/* adding it to the stack */
		if(debug){
			printf("debug: file (infilep) being read\ndebug: satarting while loop");
		}

		/* loop until we have to do the requester */
		while(fscanf(infilep, INPUTFS, hostname) > 0){
			/* completed will be set to 1 when finished */
			int completed = 0;

			if(debug){
				printf("before while loop\n");
			}
			
			while(!completed){
				pthread_mutex_lock(&mutex_read);
				pthread_mutex_lock(&mutex_write1);
				if(debug){
					printf("locked the read and write1\n");
				}
				
				/* check to see if our array is full */
				if(!array_full_test(s_arr)){
					contains = malloc(BUFFSIZE);
					strncpy(contains, hostname, BUFFSIZE);

					if(debug){
						printf("adding container to index", contains);
					}

					array_add_item(s_arr, contains);
					completed = 1;
				}

				if(debug){
					printf("unlocking our mutexes\n");
				}
				/* locking the mutexes */
				pthread_mutex_unlock(&mutex_read);
				pthread_mutex_unlock(&mutex_write1);

				/* if we are not completed, we sleep */
				if(!completed){
					usleep((rand()%100)*100000);
				}
			}
		}
		fclose(infilep);
	}

	if(debug){
		printf("Finishing request thread %ld\n", threadNum);
	}

	return NULL;

}


/* function for the resolver threads */
void *thread_resolver(void *threadarg){
	if(debug){
		printf("debug: inside thread_resolver function\n");
	}

	/* starting off the local vars */
	struct dataResolve * resolveData; /* ptr for data */
	FILE* outfilep; /* input file pointer */
	char ipstr[INET6_ADDRSTRLEN]; /* input string is as long as the add */
	char* hostname /* hostname */
	array_stack* s_arr; /* create the shared array */

	resolveData = (struct dataResolve *) threadarg;
	outfilep = resolveData->fileOutput;
	s_arr = resolveData->s_arr;

	pid_t pidt = syscall(__NR_gettid);
	if(debug){
		printf("debug: syscall, pidt: %d \n", pidt);
	}

	int emptyArray = 0;
	
	/* run through it seein iff the request are completed */
	while(!emptyArray || !completedRequestThreads){
		int resolved_spin = 0;

		/* start locking them with the mutexes */
		pthread_mutex_lock(&mutex_read);
		pthread_mutex_lock(&mutex);

		readcount++;

		if(readcount == 1){
			pthread_mutex_lock(&mutex_write1);
		}
		pthread_mutex_unlock(&mutex);
		pthread_mutex_unlock(&mutex_read);

		/* one at a time, get from the shared array */
		pthread_mutex_lock(&mutex_array);
		emptyArray = array_empty_test(s_arr);
		if(!emptyArray){
			hostname = array_remove_item(s_arr);
			if(hostname != NULL){
				if(debug){
					printf("Get the IP for %s\n", hostname);
				}
				resolvedcount = 1
			}
		}

		pthread_mutex_unlock(&mutex_array);

		pthread_mutex_lock(&mutex);
		readcount--;

		/* we read them all */
		if(readcount == 0){
			pthread_mutex_unlock(&mutex_write1)
		}

		pthread_mutex_unlock(&mutex);

		if(resolved_spin){
			array_stack listDNS;
			array_init(&listDNS, 50);
			if(dnslookup(hostname, ipstr, sizeof(ipstr)) == UTIL_FAILURE){
				fprintf(stderr, "DNS LOOKUP ERROR: %s \n", hostname);
				strncpy(ipstr, "", sizeof(ipstr));
			}

			fprintf(outfilep, "%s\n", hostname);

			char* elem;

			pthread_mutex_lock(&mutex_report);

			while((elem = (char*) array_remove_item(&listDNS)) != NULL){
				fprintf(outfilep, ", %s", elem);
				free(elem);
			}

			fprintf(outfilep, "\n");
			pthread_mutex_unlock(&mutex_report);
			free(hostname);
			array_clear(&listDNS);
		}
	}

	if(debug){
		printf("Completed resolved threads\n");
	}

	return NULL;

}

long long gettimeofday_func(){
	struct timeval te;
	gettimeofday(&te, NULL); /* gets the current time */
	long long milliseconds = te.tv_sec*1000LL + te.tv_usec/1000; /* calculates in miliseconds */
	return milliseconds;
}

int main(int argc, char* argv[]){
	/* calculate the start time */
	long startingtime = gettimeofday_func(); /* gets the current time of day */
	remove("serviced.txt");

	/* local files */
	FILE* outfilep = NULL;

	/* check all of our arguments */
	if(argc > MAX_INPUT_FILES + 1){
		fprintf(stderr, "TOO MANY FILES: %d\n", (argc-1));
		fprintf(stderr, "path: \n %s %s \n", argv[0], PATH);
		return EXIT_FAILURE;
	}

	if(argc < MIN_ARGS){
		fprintf(stderr, "NOT ENOUGH FILES: %d\n", (argc-2));
		fprintf(stderr, "path: \n %s %s \n", argv[0], PATH);
		return EXIT_FAILURE;
	}


	/* open the output file */
	outfilep = fopen(argv[(argc-1)], "w");
	if(!outfilep){
		perror("ERROR OPENING OUTPUT FILE");
		return EXIT_FAILURE;
	}

	/* build the array */
	const int a_size = TEST_ARRAY_SIZE;
	if(array_init(&s_arr, a_size) == ARRAY_FAIL){
		fprintf(stderr, "error: array initalize has failed\n");
	}

	/* create the REQUEST POOL */
	struct dataRequest * requestData[MAX_REQUESTER_THREADS]; /* ptr for data */
	pthread_t requestThreads[MAX_REQUESTER_THREADS];
	int retcode1;
	long t;

	sscanf(argv[1], "%d", &numRequestThreads);

	numRequestThreads = (numRequestThreads > MAX_REQUESTER_THREADS) ? MAX_REQUESTER_THREADS : numRequestThreads;

	if(numRequestThreads == -1){
		printf("NUM OF REQ THREADS CANNOT BE -1 \n");
	}

	int filesPerThread[numRequestThreads];

	int numFiles = 5; /* for our 5 files we have, can have more */

	if(numFiles > numRequestThreads){
		int remFiles = 5;
		int fpert = 5 / numRequestThreads;

		for(int i = 0; i < numRequestThreads; i++){
			filesPerThread[i] = fpert;
			remFiles -= fpert;
		}

		filesPerThread[0] += remFiles;
	} else {
		for(int i = 0; i < numRequestThreads; i++){
			filesPerThread[i] = 1;
		}
	}

	int new_offset;

	for(t = 0; t < numRequestThreads && t < MAX_REQUESTER_THREADS; t++){
		requestData[t].s_arr = &s_arr;
		requestData[t].inputFiles.size = filesPerThread[t];
		int numFiles = filesPerThread[t];

		for(int i = 0; i < numFiles; i++){
			requestData[t].inputFiles.array[i] = fopen(argv[t+new_offset+3], "r");
			printf("%s\n", argv[t+new_offset+3]);
			if(numFiles > 1 && i != numFiles - 1){
				new_offset++;
			}
		}

		requestData[t].threadNum = t;

		requestData[t].servicedFiles = numFiles;

		if(debug){
			printf("Creating REQUEST %ld\n", t+1);
			printf("REQUESTER thread serviced: %d \n\n", numFiles);
		}

		retcode1 = pthread_create(&(requestThreads[t]), NULL, thread_requester, &(requestData[t]));
		if(retcode1){
			printf("ERROR> return from pthread_create() is %d\n", retcode1);
			exit(EXIT_FAILURE);
		}
	}

	/* create the resolve thread pool */

	struct dataResolve resolveData;
	int numResolveThreads;
	sscanf(argv[2], "%d", &numResolveThreads);

	pthread_t resolveThreads[numResolveThreads];
	int retcode2;
	long k;

	resolveData.s_arr = &s_arr;
	resolveData.fileOutput = outfilep;

	for(k = 0; k < numResolveThreads; k++){
		if(debug){
			printf("creating resolve threads\n");
		}

		retcode2 = pthread_create(&(resolveThreads[k]), NULL, thread_resolver, &resolveData);
		if(retcode2){
			printf("ERROR> return from pthread_create() is %d\n", retcode2);
			exit(EXIT_FAILURE);
		}
	}


	/* join the two threads and delete when completed */
	for(t = 0; t < numRequestThreads && t < MAX_REQUESTER_THREADS; t++){
		pthread_join(requestThreads[t], NULL);
	}
	if(debug){
		printf("REQ THREADS DONE!!! \n");
	}

	completedRequestThreads = 1;

	for(t = 0; t < numResolveThreads; t++){
		pthread_join(requestThreads[t], NULL);
	}

	if(debug){
		printf("RES THREADS DONE!!! \n");
	}


	/* close the file */
	fclose(outfilep);

	/* clear out the array */
	array_clear(&s_arr);

	/* calculate the time taken */
	long endtime = gettimeofday_func();

	long turnaround = (endtime - startingtime);

	printf("turnaround time:  %ld seconds\n", elapsedTime/1000);

	return EXIT_SUCCESS:
/* end main */
}