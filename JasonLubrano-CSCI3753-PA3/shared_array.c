/* Jason Lubrano
 * shared_array.c
 * PA3
 */
#include <stdlib.h>

#include "shared_array.h"


// /* Jason Lubrano
//  * shared_array.h
//  * PA3
//  */

// #ifndef SHARED_ARRAY_H
// #define SHARED_ARRAY_H

// #include <stdio.h>

// #define MAX_ARRAY_SIZE 100 /* final size of a array */
// #define ARRAY_FAIL -1 /* failure */
// #define ARRAY_PASS 0 /* passing */

// typedef struct array_index_struct {
// 	void* contains;
// } array_index;

// typedef struct array_stack_struct {
// 	array_index_struct* shared_array;
// 	int head;
// 	int tail;
// 	int array_size;
// } array_stack;


//  initalize function for stack. When it runs sucessfully, it returns the size,
//  * when it fails,, it reutnrs ARRAY_FAIL 
// int array_init(array_stack* array, int size);

// /* test if the array is empty? 1 : 0*/
// int array_empty_test(array_stack *array);

// /* test if array is full? 1 : 0 */
// int array_full_test(array_stack *array);

// /* add item to the array */
// int array_add_item(array_stack* array, void* cointains);

// /* return first element on top of the array */
// int array_remove_item(array_stack* array);

// /* clean up the array */
// int array_clear(array_stack* array);

// #endif



/* initalize function for array. When it runs sucessfully, it returns the size,
 * when it fails,, it reutnrs ARRAY_FAIL */
int array_init(array_stack* array, int size){
	/* test to see if we need to make our array the default size */
	if(0 < size){
		array->array_size = size;
	} else {
		array->array_size = MAX_ARRAY_SIZE;
		perror("ERROR> setting array to default size: new size 100")
	}
	
	/* allocate some memory for our shared_array */
	array->shared_array = malloc(sizeof(queue_node) * (array->array_size));
	
	/* have to check if we have an error */
	if(!(array->shared_array)){
		perror("ERROR> array Malloc failed");
		return ARRAY_FAIL;
	}

	/* set every block in the shared_array to null */
	int nullsetter = 0;
	for(nullsetter; nullsetter < (array->array_size); nullsetter++){
		array->shared_array[nullsetter].contains = NULL;
	}

	/* circular buffer */
	array->head = 0;
	array->tail = 0;

	return array->array_size;
}

/* test if the array is empty? 1 : 0*/
int array_empty_test(array_stack *array){
	/* check to see fi the head and tail are pointing to the same
	 * node, and that they are NULL */
	if((array->head == array->tail) && (array->shared_array[array->head].contains == NULL)){
		return 1;
	} else {
		return 0;
	}
}

/* test if arra is full? 1 : 0 */
int array_full_test(array_stack *array){
	/* check to see fi the head is queal to tail
	 * if it contains some value ? 1: 0 */
	if((array->front) == (array->tail) && (array->shared_array[array->tail].contains != NULL)){
		return 1;
	} else {
		return 0;
	}
}

/* add item to the array */
int array_add_item(array_stack* array, void* cointains){
	/* before we add an item we need to see if its full */
	if(queue_full_test(array)){
		perror("ERROR> array full, unable to add item");
		return ARRAY_FAIL;
	}

	/* assuming it is not full */
	array->shared_array[array->tail].contains = new_contains;
	array->tail = ((array->tail + 1) % array->array_size);
	return ARRAY_PASS;
}

/* return first element on top of the array */
int array_remove_item(array_stack* array){
	/* cant remove an item from an empty array */
	if(queue_empty_test(array)){
		perror("ERROR> array empty, unable to remove item");
		return ARRAY_FAIL;
	}
	/* object to be removed */
	void *old_contains;

	old_contains = array->shared_array[array->head].contains;
	array->shared_array[array->head].contains = NULL;
	array->head = ((array->head + 1) % array->array_size);
}

/* clean up the array */
int array_clear(array_stack* array){
	/* run through the array and clear it up */
	while(queue_empty_test(array)){
		queue_pop(array);
	}

	free(array->shared_array);
}
