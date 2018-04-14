/*
	JASON LUBRANO
	CSCI 3753
	PA3
	COLLAB: KYLE WARD
	multi-lookup.c
*/

#include <stdlib.h>

#include "multi-lookup.h"

// #define ThunderWare(a) #a
// const char *ThunderWare_Logo = ThunderWare(
//    (woowww THUNDER WARE 0000000)	
//   (oooowwwwooowooooowwooo00000)		
//  (wwwooowooooooowwwooow00000o)		
//    (wwooooowoow00000oo0000)			
//      (wwwwooowo00000wo0)			
//     	    /    /					
//     	   /    /					
//     	  /   _/					
//     	 /   /						
//  	    /  _/						
// 	   / _/							
// 	  /_/							
// 	 /								
// );

/* mutexes, might inlcude these in a struct */
pthread_mutex_t mutex;
pthread_mutex_t write1; /* mutex for writing to the shared array */
pthread_mutex_t write2; /* mutex for writing to serviced.txt */
pthread_mutex_t read_mutex; /* mutex for reading through the shared array */
pthread_mutex_t shared_array_mutex; /* mutex to lock the array */
pthread_mutex_t error_report; /* mutex for error reporting */

/* global vars, move these to the inside funcs */
int debug = 0; /* Debugging purposes only */
int req_th_done = 0;
int readCount = 0;
FILE* output2 = NULL;

int SHARED_ARRAY_INIT(SHARED_ARRAY* s_array, int size){
	/* find the size of the array */
	if(size > 0){
		s_array->maxSize = size;
	}else{
		s_array->maxSize = SHARED_ARRAY_MAXSIZE; /* I included this for 50, move up and down for testing the full array */
	}

	/* prof knox said to malloc the array so it would find it, so I am */
	s_array->array = malloc(sizeof(SHARED_ARRAY_INDEX) * (s_array->maxSize));
	if(!(s_array->array)){
		perror("ERROR> in SHARED_ARRAY_INIT: MALLOC ERROR\n");
		/* 
			perror seems like a pretty cool tool to use:
			http://pubs.opengroup.org/onlinepubs/009695399/functions/perror.html
		*/
		return SHARED_ARRAY_FAILURE;
	}

	if(debug){
		printf("DEBUG> array malloc successful\n");
	}

	/* Set the entire array to NULL */
	int i;
	for(i = 0; i < s_array->maxSize; ++i){
		s_array->array[i].contains = NULL;
	}

	if(debug){
		printf("DEBUG> entire array is set to NULL\n");
	}

	/*
		as per addendum, one thread takes the next one in the list
		with this, take the tail and attack it to the head
		now its circular, and now I can go form the entire thing and eat away until it gets empty value
	*/
	s_array->head = 0;
	s_array->tail = 0;


	if(debug){
		printf("DEBUG> %s was successful\nreturning s_array->maxSize\n", __func__ );
	}

	/* return the array */
	return s_array->maxSize;
}

/*function to test whether or not my array is empty */
int SHARED_ARRAY_TEST_EMPTY(SHARED_ARRAY* s_array){
	if((s_array->head == s_array->tail) && (s_array->array[s_array->head].contains == NULL)){
		return 1;
	}
	else{
		return 0;
	}
	/* or true if im doing bool, maybe will change, but then i need the library and i didnt talk to a TA about it */
}

/* test whether or not an array is full */
int SHARED_ARRAY_TEST_FULL(SHARED_ARRAY* s_array){
	if((s_array->head == s_array->tail) && (s_array->array[s_array->head].contains != NULL)){
		return 1;
	}
	else{
		return 0;
	}
	/* or true if im doing bool, maybe will change, but then i need the library and i didnt talk to a TA about it */
}

/* function to remove the first item from the array */
void* SHARED_ARRAY_REMOVE_ITEM(SHARED_ARRAY* s_array){
	void* ret_contains;
	
	/* if the array is empty we cant have anything from it */
	if(SHARED_ARRAY_TEST_EMPTY(s_array)){
		if(debug){
			printf("DEBUG> shared array empty, unable to remove the item\n");
		}
		return NULL;
	}

	/* returns the item that will be removed */
	ret_contains = s_array->array[s_array->head].contains;
	s_array->array[s_array->head].contains = NULL;
	s_array->head = ((s_array->head + 1) % s_array->maxSize);

	return ret_contains;
}

/* function that will add an item to te end of the array */
int SHARED_ARRAY_ADD_ITEM(SHARED_ARRAY* s_array, void* new_contains){
	/* if the array is full we cant add an item to it and must start the reading */
	if(SHARED_ARRAY_TEST_FULL(s_array)){
		if(debug){
			printf("DEBUG> array is full unable to add item\n");
		}
		return SHARED_ARRAY_FAILURE;
	}

	/* make the tail of the shared array contains equal to te new item */
	s_array->array[s_array->tail].contains = new_contains;
	/* move the poniter of the tail over one spot */
	s_array->tail = ((s_array->tail+1) % s_array->maxSize);

	if(debug){
		printf("DEBUG> SHARED_ARRAY_ADD_ITEM was successful\n");
	}

	return SHARED_ARRAY_SUCCESS;
}

/* freeiing up the memory from the array */
void SHARED_ARRAY_CLEAR(SHARED_ARRAY* s_array){
	/* while it isnt empty remove the last item from the list */
	while(!SHARED_ARRAY_TEST_EMPTY(s_array)){
		SHARED_ARRAY_REMOVE_ITEM(s_array);
	}

	/* THIS IS WHERE I WAS LOSING A SHITLAOD OF MEMORY */
	free(s_array->array);
}

/* THREADS */

/* REQUEST THREADS */
void* REQUEST_THREAD(void* threadarg){
	struct REQ_TH_DATA_STRUCT * REQ_TH_DATA_STRUCT; /* create the request thread */
	char hostname[BUFFSIZE]; /* hostname fo the website with BUFFSIZE equal to whatever */
	FILE* inputfp; /* input files */
	SHARED_ARRAY* s_array; /* this is the shared array */
	char* contains; /* whatever is at the index of the shared array */
	long threadNum; /* thread number */
	/* thread argument */
	REQ_TH_DATA_STRUCT = (struct REQ_TH_DATA_STRUCT *) threadarg;

	/* 
		NOTE I did not know how to do this, source for reference:
			https://stackoverflow.com/questions/21091000/how-to-get-thread-id-of-a-pthread-in-linux-c-program#21206357
			Homie here hooks me up
	*/
	pid_t req_thread_id = syscall(__NR_gettid);

	printf("requester thread # %d says Hello world in %s\n", req_thread_id, __func__);

	if(debug){
		printf("DEBUG> REQ THREAD ID: %d \n", req_thread_id);
	}

	/* lock with the write2 mutex */
	pthread_mutex_lock(&write2);

	/*
		open serviced.txt and then give it write permissions:
		http://pubs.opengroup.org/onlinepubs/009695399/functions/fopen.html
	*/
	output2 = fopen("serviced.txt", "ab");
	if(!output2){
		perror("Error Output2 file was not opened correctly"); /* THIS is the debug message */
	}

	/* keeps track of the number of files serviced by each thread */
	fprintf(output2, "REQUESTER THREAD #%d SERVICED %d FILES\n", req_thread_id, REQ_TH_DATA_STRUCT->filesServiced);

	/* close the output file */
	fclose(output2);
	/* forgot to unlock lol */
	pthread_mutex_unlock(&write2);

	/* 
		so this is where the dynamic array inside of the request thread comes in handy
		take the files in the array and then make an array for that struct
		each thread grabs x number of files remaining
		
		one file goes in, and then the next thread grabs a file, and so forth
	*/
	int i;
	for(i = 0; i < REQ_TH_DATA_STRUCT->inFiles.size; i++){
		/* add those files to the array of files in teh struct */
		inputfp = REQ_TH_DATA_STRUCT->inFiles.array[i];
		/* maake the shared array equal to the array of the struct */
		s_array = REQ_TH_DATA_STRUCT->s_array;
		/* and set the thread number handled by the function equal to the specific thread num */
		threadNum = REQ_TH_DATA_STRUCT->threadNum;

		/* open the input files */
		if(!inputfp){
			printf("Error Opening Input File");
			/* 
				pthread_exit - thread termination 
				http://pubs.opengroup.org/onlinepubs/7908799/xsh/pthread_exit.html
			*/
			pthread_exit(NULL);
		}

		if (debug){
			printf("DEBUG> Reading through the file\n");
		}

		/* add to the shared array */
		while(fscanf(inputfp, INPUTFS, hostname) > 0){
			/*
				Notes on fscanf here:
				fscanf, scanf, sscanf - convert formatted input
				http://pubs.opengroup.org/onlinepubs/009695399/functions/fscanf.html
			*/
			/* completed would be a bool */
			int completed = 0;
			/* while it isnt completed reading loop */
			while(!completed){
				/* do the lock on the read and write so we can only read and write per thread */
				pthread_mutex_lock(&read_mutex);
				pthread_mutex_lock(&write1);
				/* first is to check if the array is full */
				if (!SHARED_ARRAY_TEST_FULL(s_array)){
					/* malloc size for the buffer */
					contains = malloc(BUFFSIZE);

					/*
						strncpy - copy part of a string
						http://pubs.opengroup.org/onlinepubs/7908799/xsh/strncpy.html
					*/
					strncpy(contains, hostname, BUFFSIZE);

					if(debug){
						printf("DEBUG> ADDING: %s TO THE SHARED ARRAY\n", contains);
					}
					/* add the item to the shared array */
					SHARED_ARRAY_ADD_ITEM(s_array, contains);

					/* were completed, set it to true, err, 1*/
					completed = 1;
				}
				
				/* unlock it so we can do it again */
				pthread_mutex_unlock(&write1);
				pthread_mutex_unlock(&read_mutex);

				/* sleep boi */
				if (!completed){
					usleep((rand()%100)*100000);
				}
			}
		}

		/* close the input file */
		fclose(inputfp);
	}

	if(debug){
		printf("DEBUG> %s is closing, requester thread #%ld says goodnight\n", __func__, threadNum);
	}

	FILE *fileout = fopen("performance.txt", "ab");
	fprintf(fileout, "Number for requester thread = %d\n", req_thread_id);
	fclose(fileout);


	/* were done */
	return NULL;
}


/* RESOLVE THREADS */
void* RESOLVE_THREAD(void* threadarg){
	struct RES_TH_DATA_STRUCT * RES_TH_DATA_STRUCT; /* resolver thread struct */
	FILE* outfilep; /* file we are writing to */
	char firstipstr[INET6_ADDRSTRLEN]; /* how we write the thing */
	char * hostname; /* name of the host */
	SHARED_ARRAY* s_array; /* shared array */
	RES_TH_DATA_STRUCT = (struct RES_TH_DATA_STRUCT *) threadarg;
	/* make the outfile equal to the outfile from the resolver thread */
	outfilep = RES_TH_DATA_STRUCT->outFile;
	/* shared array equal to whatever the thread has to do */
	s_array = RES_TH_DATA_STRUCT->s_array;

	/* system call to get the thread id */
	pid_t res_thread_id = syscall(__NR_gettid);

	printf("resolver thread # %d says Hello world in %s\n", res_thread_id, __func__);

	/* the array cant be empty when we go to read, therefor make the bool */
	int EMPTY_SHARED_ARRAY = 0;

	/* while the array isnt empty */
	while (!EMPTY_SHARED_ARRAY || !req_th_done){
		int resolved = 0; /* bool to see if it is resolved */
		/* make the lock mutex */
		pthread_mutex_lock(&read_mutex);
		pthread_mutex_lock(&mutex);
		/* we are reading! */
		readCount = readCount + 1;
		if(debug){
			printf("DEBUG> Thread %d busy reading while writing is happening\nRead_mutex and mutex are locked.)\n", res_thread_id);
		}

		/* because we are reading, we will have to lock otu the writers */
		if (readCount==1){
			pthread_mutex_lock(&write1);
		}
		/* and then we can unlock the readers */
		pthread_mutex_unlock(&mutex);
		pthread_mutex_unlock(&read_mutex);

		/* this is why i set the shared array test to 0 if it passes */
		pthread_mutex_lock(&shared_array_mutex);
		/* if it is 0, then we can /still/ read from it */
		EMPTY_SHARED_ARRAY = SHARED_ARRAY_TEST_EMPTY(s_array);
		if(!EMPTY_SHARED_ARRAY){
			/* hostname is equal to the first item on the list */
			hostname = SHARED_ARRAY_REMOVE_ITEM(s_array);
			/*if it isnt empty we will get the id, set the bool int resolverd equal to 1 */
			if (hostname != NULL){
				if(debug){
					printf("DEBUG> SEARCHING IP for: %s\n", hostname);
				}
				resolved = 1; /* boolean that was a local var */
			}
		}
		/* unlock the mutex for the shared array */
		pthread_mutex_unlock(&shared_array_mutex);

		/* mutex lock, set the readCount down one */
		pthread_mutex_lock(&mutex);
		readCount = readCount - 1;
		if (readCount == 0){
			pthread_mutex_unlock(&write1);
			/* if the reading is back down to 0, meaning we read all, then unlcok teh writes so we can write more */
		}
		pthread_mutex_unlock(&mutex);

		/* if resolved is set to 1 */
		if(resolved){
			SHARED_ARRAY IP_LIST; /* list of IP addresses */
			SHARED_ARRAY_INIT(&IP_LIST, ARRAY_SIZE_TEST); /* INIT the IP, set the max to 50 bc its the size of the main shared array */
			pthread_mutex_lock(&error_report);
			if(dnslookup(hostname, firstipstr, sizeof(firstipstr)) == UTIL_FAILURE){
				fprintf(stderr, "dnslookup bogus hostname error: %s\n", hostname);
				strncpy(firstipstr, "", sizeof(firstipstr));
				/* something happened and we failed the lookup */
			}
			/* print to the file the hostname and the ipistr which would be the IP address */
			fprintf(outfilep, "%s, %s\n", hostname, firstipstr);
			if(debug){
				printf("DEBUG> writing to output file: %s, %s\n", hostname, firstipstr);
			}
			pthread_mutex_unlock(&error_report);
			/* free up the hostname */
			free(hostname);
			/* empty the IP_LIST array */
			SHARED_ARRAY_CLEAR(&IP_LIST);
		}
	}

	if(debug){
		printf("DEBUG> %s is closing, resolver thread says goodnight\n", __func__);
	}

	FILE *fileout2 = fopen("performance.txt", "ab");
	fprintf(fileout2, "Number for resolver thread = %d\n", res_thread_id);
	fclose(fileout2);

	return NULL;
}


/* get time of day. run a system call that can get it.. nvm got it to work*/
/* for some reason this isnnt working, try to email prathyusha */
long long gettimeofday_func() {
	/*
		resource on get time of day:
		https://linux.die.net/man/2/gettimeofday
	*/
	struct timeval evaltime;
	/* gets the current time */
	gettimeofday(&evaltime, NULL); /* system call */
	/* calculate the miliseconds */
	long long milliseconds = evaltime.tv_sec*1000LL + evaltime.tv_usec/1000;
	/* return the milliseconds */
	return milliseconds;
}

/* AND WE MADE IT TO THE MAIN */
int main(int argc, char* argv[]){
	//printf("%s\n", ThunderWare_Logo);
	/* set the start time */
	long startTime = gettimeofday_func();

	/* number of our req threads set to -1 for error checking */
	int num_req_ths = -1;
	
	/* start off with a new serviced.txt and remove the old one */
	remove("serviced.txt");
	remove("performance.txt");

	/* Local Vars */
	FILE* outfilep = NULL;

	/*
		Check Arguments, check if its less than the min argumetns
	*/
	if(argc < MINARGS){
		fprintf(stderr, "ERROR> Not enough arguments: %d\n", (argc - 1));
		fprintf(stderr, "ERROR> SYNOPSIS:\n %s %s\n", argv[0], SYNOPSIS);
		return EXIT_FAILURE;
	}
	if (argc > MAX_REQ_THS + 1){
		fprintf(stderr, "Too many files: %d\n", (argc - 2));
		fprintf(stderr, "SYNOPSIS:\n %s %s\n", argv[0], SYNOPSIS);
		return EXIT_FAILURE;
	}

	/* Open Output File */
	outfilep = fopen(argv[(argc-1)], "w");
	if(!outfilep){
		perror("Error Opening Output File");
		return EXIT_FAILURE;
	}

	/* make the shared array */
	SHARED_ARRAY s_array;
	/* this will be the size of the array */
	const int array_size = ARRAY_SIZE_TEST;

	/* if the INIT of the shared array has failed, knwo */
	if(SHARED_ARRAY_INIT(&s_array, array_size) == SHARED_ARRAY_FAILURE){
		fprintf(stderr, "error: SHARED_ARRAY_INIT failed!\n");
	}

	/* create a pool of request threads */
	struct REQ_TH_DATA_STRUCT REQ_TH_DATA_STRUCT[MAX_REQ_THS]; /* try it with max request threads */
	pthread_t REQ_THREADS[MAX_REQ_THS]; /* pthread it */
	int req_code; /* when we create the thread we will be given a code */
	long thread_counter; /* count the threads, redundnet */

	/* scan the arguemtn for the threads */
	sscanf (argv[1],"%d",&num_req_ths);

	/* if the num_req_ths is greater than whats allowed, set it to max threads */
	num_req_ths = (num_req_ths > MAX_REQ_THS) ? MAX_REQ_THS : num_req_ths;

	if(debug){
		printf("DEBUG> number of req threads %d \n", num_req_ths);
	}
	
	if (num_req_ths == -1){
		printf("\nhow did you even get here?\n");

	}

	/* 
		this is where i dont understand the addendum
		do we take each thread and find out how many files that one thread should work on?
		one file after another? file1 is worked on by thread 1, but thread 1 works on file1 & file6
	*/
	int files_per_thread[num_req_ths];

	/* we have the 5 input files */
	int numFiles = 5;

	/* the number of files greater than request threads */
	if (numFiles > num_req_ths){
		/* modular math */
		int remainingFiles = 5;
		/* so it says how many files each thread has */
		int fpt = 5 / num_req_ths;
		/* go through the remaining files, count how much they have to work on */
		for(int i = 0; i < num_req_ths; i++){
			files_per_thread[i] = fpt;
			remainingFiles -= fpt;
			/* hard coding preemptive files*/
		}

		/* and then we just have to append the files per thread */
		files_per_thread[0] += remainingFiles;
	
	}else{
		/* each thread gets (at least) one file */
		for(int i = 0; i < num_req_ths; i++){
			files_per_thread[i] = 1;
		}
	}

	/* work with offset again */
	int offset = 0;

	for(thread_counter = 0; thread_counter < num_req_ths && thread_counter < MAX_REQ_THS; thread_counter++){
		/* the thread array gets equal to the main shared array */
		REQ_TH_DATA_STRUCT[thread_counter].s_array = &s_array;
		/* set the thread file zie to the file per thread, so we know how much to work with */
		REQ_TH_DATA_STRUCT[thread_counter].inFiles.size = files_per_thread[thread_counter];
		/*and then the number of files each thread works on is equal to the files per thread per that thread */
		int numFiles = files_per_thread[thread_counter];

		for(int i = 0; i < numFiles; i++){
			/* open that the files were going to work on the offset per the argumetn, give it read only permissions */
			REQ_TH_DATA_STRUCT[thread_counter].inFiles.array[i] = fopen(argv[thread_counter+offset+3], "r");
			/* print the argument */
			printf("%s\n", argv[thread_counter+offset+3]);
			/* add the offset value by 1 */
			if(numFiles > 1 && i != numFiles - 1){
				offset = offset + 1;
			}
		}

		/* thread counter eq to that thrad num */
		REQ_TH_DATA_STRUCT[thread_counter].threadNum = thread_counter;
		/* we have the files serviced equal to the number of files */
		REQ_TH_DATA_STRUCT[thread_counter].filesServiced = numFiles;
		
		if (debug){
			printf("HELLO> REQUEST THREAD %ld\n", thread_counter+1);
			printf("REQUESTER> number of files serviced: %d \n\n", numFiles);
		}

		/* creating the request code */
		req_code = pthread_create(&(REQ_THREADS[thread_counter]), NULL, REQUEST_THREAD, &(REQ_TH_DATA_STRUCT[thread_counter]));
		

		if (req_code){
			printf("ERROR; return code from pthread_create() is %d\n", req_code);
			exit(EXIT_FAILURE);
		}
	}

	/* create the resolver thread pool */
	struct RES_TH_DATA_STRUCT RES_TH_DATA_STRUCT;
	int num_res_ths;
	int ret_code_2; /* return code 2 */
	sscanf (argv[2],"%d",&num_res_ths);

	pthread_t RES_THREADS[num_res_ths];
	long thread_counter2; /* counter for the resolver threads */

	RES_TH_DATA_STRUCT.s_array = &s_array;
	RES_TH_DATA_STRUCT.outFile = outfilep;

	/* RESOLVE threads */

	for(thread_counter2=0; thread_counter2<num_res_ths; thread_counter2++){
		if (debug){
			printf("DEBUG> Welcome RESOLVER Thread number %ld to the world\n", thread_counter2);
		}
		ret_code_2 = pthread_create(&(RES_THREADS[thread_counter2]), NULL, RESOLVE_THREAD, &RES_TH_DATA_STRUCT);
		if (ret_code_2){
			printf("ERROR: return code from pthread_create() is %d\n", ret_code_2);
			exit(EXIT_FAILURE);
		}
	}

	/*
		playing around with the joining threads, wait for them to finish
		pthread_join - wait for thread termination
		http://pubs.opengroup.org/onlinepubs/9699919799/functions/pthread_join.html

		The only way pthread_join fails is if a deadlock was detected
	*/
	for(thread_counter = 0; (thread_counter < num_req_ths) && (thread_counter < MAX_REQ_THS); thread_counter++){
		pthread_join(REQ_THREADS[thread_counter],NULL);
		/* join the rquester threads, wait for them to finish */
	}

	if (debug){
		printf("DEBUG> Requester threads are joined and done\n");
	}

	req_th_done = 1;

	/* join the resolver threads when they finished */
	for(thread_counter = 0; thread_counter < num_res_ths; thread_counter++){
		pthread_join(RES_THREADS[thread_counter],NULL);
		/* join the resolver threads and wait for them to finish */
	}

	if (debug){
		printf("DEBUG> Resolver threads are done and joined\n");
	}

	/* Close Output File */
	fclose(outfilep);

	/* Cleanup SHARED_ARRAY */
	SHARED_ARRAY_CLEAR(&s_array);

	/* calculate the time the program toook */
	long endTime = gettimeofday_func();
	/* turn around every now and then i get a little bit loney because you never come around */
	long turnaround = (endTime - startTime);
	printf("Turnaround: %ld seconds\n", turnaround/1000);

	FILE *fileout3 = fopen("performance.txt", "ab");
	fprintf(fileout3, "Total run time = %ld\n", turnaround/1000);
	fclose(fileout3);

	/* hopefully this work */
	//printf("%s\n", ThunderWare_Logo);
	return EXIT_SUCCESS;
}
