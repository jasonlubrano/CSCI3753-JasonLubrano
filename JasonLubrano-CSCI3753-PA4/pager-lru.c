/*
 * File: pager-lru.c
 * Author:       Andy Sayler
 *               http://www.andysayler.com
 * Adopted From: Dr. Alva Couch
 *               http://www.cs.tufts.edu/~couch/
 *
 * Project: CSCI 3753 Programming Assignment 4
 * Create Date: Unknown
 * Modify Date: 2012/04/03
 * Description:
 * 	This file contains an lru pageit implmentation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <limits.h>

#include "simulator.h"


void pageit(Pentry q[MAXPROCESSES]) { 
    
    /* This file contains the stub for an LRU pager */
    /* You may need to add/remove/modify any part of this file */

    /* Static vars */
    static int INIT = 0;
    static int TICK = 1; // artificial time
    static int TMSTMP[MAXPROCESSES][MAXPROCPAGES];

    /* Local vars */
    /* temp vars will be updated throughotu program */
    int TEMP_PROC;
    int TEMP_PAGE;
    /* page old and oldest will be the ones updated
    to either be swapped out or kept */
	int PAGE_OLD;
	int PAGE_OLDEST;
	/* loop */
	int i;
    /* initialize vars */
	if(!INIT){
		for(TEMP_PROC=0; TEMP_PROC < MAXPROCESSES; TEMP_PROC++){
			for(TEMP_PAGE=0; TEMP_PAGE < MAXPROCPAGES; TEMP_PAGE++){
				TMSTMP[TEMP_PROC][TEMP_PAGE] = 0; 
			} /* end for */
		} /* end for */
		/* it init */
		INIT = 1;
	} /* end if */

    /* lru algo */
	for(TEMP_PROC = 0; TEMP_PROC < MAXPROCESSES; TEMP_PROC++){
		/* is proc in q active? */
		if(q[TEMP_PROC].active){
			/* assing the cur pg */
			TEMP_PAGE = q[TEMP_PROC].pc / PAGESIZE;
			/* if pg isnt in, swap it in and get one ready 2 b kicked otu */
			if(!q[TEMP_PROC].pages[TEMP_PAGE] && !pagein(TEMP_PROC, TEMP_PAGE)){
				/* oldlest pg num is tick */
				PAGE_OLDEST = TICK;
				/* loop through pgs in the curr proc */
				for(i = 0; i < MAXPROCPAGES; i++){
					/* if pg @ i is swap & not oldest
					reagin the oldest page and then set one to be remove*/
					if(q[TEMP_PROC].pages[i] && TMSTMP[TEMP_PROC][i] < PAGE_OLDEST){
						/* reassign the oldest page */
						PAGE_OLDEST = TMSTMP[TEMP_PROC][i];
						/* curr pg be remove */
						PAGE_OLD = i;
					} /* end if */
				} /* end for */
				/* the one we assign revently to be remove */
				pageout(TEMP_PROC, PAGE_OLD);
				break;
 			} /* end if */
 			/* update the tick time */
			TMSTMP[TEMP_PROC][TEMP_PAGE] = TICK;
		} /* end if */
	} /* end for */
    /* advance time for next pageit iteration */
    TICK++; /* update the time */
} 