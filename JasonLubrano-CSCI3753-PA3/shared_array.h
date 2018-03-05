/* Jason Lubrano
 * shared_array.h
 * PA3
 */

#ifndef SHARED_ARRAY_H
#define SHARED_ARRAY_H

#include <stdio.h>

#define MAX_ARRAY_SIZE 100 /* final size of a array */
#define ARRAY_FAIL -1 /* failure */
#define ARRAY_PASS 0 /* passing */

typedef struct array_index_struct {
	void* contains;
} array_index;

typedef struct array_stack_struct {
	array_index_struct* shared_array;
	int head;
	int tail;
	int array_size;
} array_stack;


/* initalize function for stack. When it runs sucessfully, it returns the size,
 * when it fails,, it reutnrs ARRAY_FAIL */
int array_init(array_stack* array, int size);

/* test if the array is empty? 1 : 0*/
int array_empty_test(array_stack *array);

/* test if array is full? 1 : 0 */
int array_full_test(array_stack *array);

/* add item to the array */
int array_add_item(array_stack* array, void* cointains);

/* return first element on top of the array */
int array_remove_item(array_stack* array);

/* clean up the array */
int array_clear(array_stack* array);

#endif