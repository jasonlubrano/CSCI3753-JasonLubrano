/*
 * File: pager-predict.c
 * Author:       Andy Sayler
 *               http://www.andysayler.com
 * Adopted From: Dr. Alva Couch
 *               http://www.cs.tufts.edu/~couch/
 *
 * Project: CSCI 3753 Programming Assignment 4
 * Create Date: Unknown
 * Modify Date: 2012/04/03
 * Description:
 *  This file contains a predictive pageit
 *      implmentation.
 */


/* jasons pager predict */
#include <stdio.h> 
#include <stdlib.h>

#include "simulator.h"


/* going to take some stuff from my pager-lru.c and throw it into here */
/* this func from pager-lru.c will use this for page swaping */
int LRU_SEARCH_PAGE(int TMSTMP[MAXPROCESSES][MAXPROCPAGES], Pentry q[MAXPROCESSES], int TEMP_PROC, int TICK){
	int PAGE_OLDEST = TICK;
	int PAGE_OLD;

	int i;
	for(i = 0; i < MAXPROCPAGES; i++){
		/* check if the page isnt the oldest and its swaped in bc we reassign the oldest */
		if(q[TEMP_PROC].pages[i] && TMSTMP[TEMP_PROC][i] < PAGE_OLDEST){
			/* reassign the oldest, and set the curr pg to be remove */
			PAGE_OLDEST = TMSTMP[TEMP_PROC][i];
			PAGE_OLD = i;
		}
	}
	return PAGE_OLD;
}

void BUBO_SWAP(int *l, int *r){
	int lptr = *l;
	*l = *r;
	*r = lptr;
}

void pageit(Pentry q[MAXPROCESSES]) { 
	
	/* This file contains the stub for a predictive pager */
	/* You may need to add/remove/modify any part of this file */

	/* static vars */
	/* givens */
	static int INIT = 0;
	static int TICK = 1; // artificial time
	/* created */
	static int PROC_CNTR[MAXPROCESSES]; /* for counter array */
	static int LAST_PC[MAXPROCESSES]; /* lalst used process process counter */
	static int TMSTMP[MAXPROCESSES][MAXPROCPAGES]; /* time stamp for pages and procs */
	/* use the page TMSTMP, number, and frequency~ to determine when it should be paged out. */
	/* use 3d array so each element has something else it is pointing to for another flow patern */
	static int *CTRL_FLOW_TMSTMP[MAXPROCESSES][MAXPROCPAGES][MAXPROCPAGES];
	static int CTRL_FLOW_PG_NUM[MAXPROCESSES][MAXPROCPAGES][MAXPROCPAGES];
	static int CTRL_FLOW_FREQ[MAXPROCESSES][MAXPROCPAGES][MAXPROCPAGES];
	/* Local vars */
	/* we were given none :c */
	/* so like with the 3ed array we need to determine it for bekadys algorithm.*/
	int **PRED_TMSTMP;
	int *PRED_PG_NUM;
	int *PRED_FREQ;

	/* local vars to determine lru and last/current page loc */
	int TEMP_PROC;
	int TEMP_PAGE;
	int CURR_PG_NUM;
	int LAST_PG_NUM;
	int NUM_PG_FILLED;
	int i, j, k;

	/* like in the lru, INIT the static vars */
	if(!INIT){
		/* INIT the cmplx vars */
		for(i = 0; i < MAXPROCESSES; i++){
			for(j = 0; j < MAXPROCESSES; j++){
				for(k = 0; k < MAXPROCESSES; k++){
					/* loop through elems in 3D array */
					CTRL_FLOW_FREQ[i][j][k] = -1;
					CTRL_FLOW_PG_NUM[i][j][k] = -1;
					CTRL_FLOW_TMSTMP[i][j][k] = NULL;
				} /* end for */
			} /* end for */
		} /* end for */
		for(TEMP_PROC = 0; TEMP_PROC < MAXPROCESSES; TEMP_PROC++){
			for(TEMP_PAGE = 0; TEMP_PAGE < MAXPROCPAGES; TEMP_PAGE++){
				TMSTMP[TEMP_PROC][TEMP_PAGE] = 0;
			}
			PROC_CNTR[TEMP_PROC] = 0;
		}
		/* after its finished, its fully INIT */
		INIT = 1;
	}
	
	/* TODO: Implement Predictive Paging */
	/* going to udate the ctrl flow for each process */
	for(TEMP_PROC = 0; TEMP_PROC < MAXPROCESSES; TEMP_PROC++){
		if(q[TEMP_PROC].active && LAST_PG_NUM != -1){
			/* if q is inactive then PAGE OUT */
			LAST_PG_NUM = LAST_PC[TEMP_PROC] / PAGESIZE;
			/* update LAST_PC to curr prog ct */
			LAST_PC[TEMP_PROC] = q[TEMP_PROC].pc;
			/* update curr pg num */
			CURR_PG_NUM = LAST_PC[TEMP_PROC] / PAGESIZE;
			TEMP_PAGE = (q[TEMP_PROC].pc - 1) / PAGESIZE;
			/* time sta mp = curr tik */
			TMSTMP[TEMP_PROC][TEMP_PAGE] = TICK;
			/* if the last page is the current page then we skip! */
			if(CURR_PG_NUM != LAST_PG_NUM){
				/* PAGE OUT */
				pageout(TEMP_PROC, LAST_PG_NUM);
				/* for throughout the proc last page */
				for(i = 0; i < MAXPROCPAGES; i ++){
					/* the magic of belady */
					if(CURR_PG_NUM == CTRL_FLOW_PG_NUM[TEMP_PROC][LAST_PG_NUM][i]){
						/* the freq goe up */
						CTRL_FLOW_FREQ[TEMP_PROC][LAST_PG_NUM][i]++;
						break;
					} /* end if */
					if(CTRL_FLOW_PG_NUM[TEMP_PROC][LAST_PG_NUM][i] == -1){
						CTRL_FLOW_FREQ[TEMP_PROC][LAST_PG_NUM][i] = 1;
						CTRL_FLOW_PG_NUM[TEMP_PROC][LAST_PG_NUM][i] = CURR_PG_NUM;
						CTRL_FLOW_TMSTMP[TEMP_PROC][LAST_PG_NUM][i] = &(TMSTMP[TEMP_PROC][i]);
						break;
					} /* end if */
				} /* end for */
			} /* end if */
		} /* end if */
	} /* end for loop */
	/* active process page swap */
	for(TEMP_PROC = 0; TEMP_PROC < MAXPROCESSES; TEMP_PROC++){
		/* if q is inactive then we swap out pages */
		if(q[TEMP_PROC].active){
			/* the temp page is teh page number the next process will require */
			TEMP_PAGE = (q[TEMP_PROC].pc) / PAGESIZE;
			/* check if the page is swaped in */
			if(q[TEMP_PROC].pages[TEMP_PAGE] != 1){
				/* page swap in, proc ctr eq 0 since no waiting for page out anymore */
				/* check if page in progress via the pro cntr 0 if so dont call page out */
				if(pagein(TEMP_PROC, TEMP_PAGE)){
					if(PROC_CNTR[TEMP_PROC]){
						PROC_CNTR[TEMP_PROC] = 0;
						/* run a LRU search for page. this is where hte func comes in.
							swap out old and in the needed one
							checkk if another page queue for swap out if not exit */
						if(LRU_SEARCH_PAGE(TMSTMP, q, TEMP_PROC, TICK)){
							PROC_CNTR[TEMP_PROC] = 1;
						} /* end if */
					} /* end if */
				} /* end if */
			} /* end if */
		} else {
			/* proc is idle so page out */
			for(TEMP_PAGE = 0; TEMP_PAGE < MAXPROCPAGES; TEMP_PAGE++){
				pageout(TEMP_PROC, TEMP_PAGE);
			} /* end for */
		} /* end if else */
	} /* end for */

	for(TEMP_PROC = 0; TEMP_PROC < MAXPROCESSES; TEMP_PROC++){
		if(q[TEMP_PROC].active){
			/* learning algorithm */
			NUM_PG_FILLED = 0;
			PRED_TMSTMP = CTRL_FLOW_TMSTMP[TEMP_PROC][(q[TEMP_PROC].pc + 101) / PAGESIZE];
			PRED_FREQ = CTRL_FLOW_FREQ[TEMP_PROC][(q[TEMP_PROC].pc + 101) / PAGESIZE];
			PRED_PG_NUM = CTRL_FLOW_PG_NUM[TEMP_PROC][(q[TEMP_PROC].pc + 101) / PAGESIZE];
			/* look for empty pages and if not inc the num o filled pages. eleminate seg faults & pg faults */
			for(i = 0; i < MAXPROCPAGES; i++){
				if((NUM_PG_FILLED < MAXPROCPAGES) && (PRED_PG_NUM[NUM_PG_FILLED] != -1)){
					NUM_PG_FILLED++;
				} /* end if */
			} /* end for */
			/* go until the t is less than tehe cur t, swhap bc beladys */
			for(i = 0; i < MAXPROCPAGES; i++){
				for(int j = 1; j < NUM_PG_FILLED; j++){
					if((*PRED_TMSTMP[j]) > (*PRED_TMSTMP[j - 1])){
						if((PRED_FREQ[j]) > (PRED_FREQ[j - 1])){
							BUBO_SWAP(PRED_PG_NUM + (j - 1), PRED_PG_NUM + j);
							BUBO_SWAP(*PRED_TMSTMP + (j - 1), *PRED_TMSTMP + j);
							BUBO_SWAP(PRED_FREQ + (j - 1), PRED_FREQ + j);
						} /* end if */
					} /* end if */
				} /* end for */
			} /* end for */
			/* its all sorted ptimally so we swap in*/
			for(i = 0; i < NUM_PG_FILLED; i++){
				pagein(TEMP_PROC, PRED_PG_NUM[i]);
			} /* end for */
		} /* end if */
	} /* end for */

	/*fprintf(stderr, "pager-predict not yet implemented. Exiting...\n");
	exit(EXIT_FAILURE);*/



	/* advance time for next pageit iteration */
	TICK++;
}