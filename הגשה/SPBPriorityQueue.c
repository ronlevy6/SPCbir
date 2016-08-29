#include "SPBPriorityQueue.h"
#include "SPList.h"
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>


struct sp_bp_queue_t{
	int maxSize; // Max capacity of the queue
	SPList queue; // The queue using SPList struct
};

SPBPQueue spBPQueueCreate(int maxSize){

	// check input validity
	if (maxSize <= 0){
		return NULL;
	}

	SPBPQueue new_queue = NULL;
	new_queue = (SPBPQueue)malloc(sizeof(*new_queue));
	// check memory allocation
	if (new_queue == NULL){
		return NULL;
	}
	new_queue->maxSize = maxSize;
	// create the list
	SPList list = spListCreate();
	if (list == NULL){
		// memory allocation problem - free previous allocations
		spBPQueueDestroy(new_queue);
		return NULL;
	}
	new_queue->queue = list;

	return new_queue;
}

SPBPQueue spBPQueueCopy(SPBPQueue source){
	SPBPQueue copy_queue;
	int max_size;

	if (source == NULL){
		return NULL;
	}

	max_size = spBPQueueGetMaxSize(source);
	copy_queue = spBPQueueCreate(max_size);
	if (copy_queue == NULL){
		return NULL;
	}

	// copy data from source to copy_queue
	SPList source_queue = spListCopy(source->queue);
	// check source queue is not null - memory allocation failure
	if (source_queue == NULL){
		spBPQueueDestroy(copy_queue);
		return NULL;
	}
	// free the empty list to prevent memory leaks
	spListDestroy(copy_queue->queue);
	copy_queue->queue = source_queue;

	return copy_queue;
}

void spBPQueueDestroy(SPBPQueue source){
	// check input
	if (source == NULL){
		return;
	}

	// queue is not null - destroy list
	spListDestroy(source->queue);
	free(source);
}

void spBPQueueClear(SPBPQueue source){
	// check input validity
	if (source == NULL){
		return;
	}

	spListClear(source->queue);
}

int spBPQueueSize(SPBPQueue source){
	// check input validity
	if (source == NULL){
		return -1;
	}
	return spListGetSize(source->queue);
}

int spBPQueueGetMaxSize(SPBPQueue source){
	// check input validity
	if (source == NULL){
		return -1;
	}

	return source->maxSize;
}

SPListElement spBPQueuePeek(SPBPQueue source){
	// checking input validity
	if (source == NULL){
		return NULL;
	}

	SPListElement first = spListGetFirst(source->queue);

	// create new copy of the first element
	SPListElement first_copy = spListElementCopy(first);

	return first_copy;
}

SPListElement spBPQueuePeekLast(SPBPQueue source){
	// check input validity
	if (source == NULL){
		return NULL;
	}

	// create a copy of the queue element list
	SPList queue =source->queue;
	SPListElement element = spListGetFirst(queue);
	// create new copy of the first element


	int queue_size = spBPQueueSize(source);

	// after receiving the first element in the queue
	//get the last element by looping over size - 1 elements
	for (int i = 0; i < queue_size - 1; i++){
		element = spListGetNext(queue);
	}
	// make a copy of the the last element to be returned
	SPListElement elem_copy = spListElementCopy(element);

	return elem_copy;


}

double spBPQueueMinValue(SPBPQueue source){
	// check input validity
	if (source == NULL){
		return -1;
	}

	SPListElement first = spBPQueuePeek(source);
	if( first == NULL){ // the queue is empty
		return -1;
	}
	double min_val =spListElementGetValue(first);
	// free allocated memory
	spListElementDestroy(first);

	return min_val;

}
double spBPQueueMaxValue(SPBPQueue source){
	// check input validity
	if (source == NULL){
		return -1;
	}

	SPListElement last = spBPQueuePeekLast(source);
	if( last == NULL){ // the queue is empty
		return -1;
	}
	double max_val =spListElementGetValue(last);
	// free allocated memory
	spListElementDestroy(last);

	return max_val;

}

bool spBPQueueIsEmpty(SPBPQueue source){
	assert (source != NULL);

	int size = spBPQueueSize(source);
	if (size == 0){
		return true;
	}
	else{	// queue is not empty
		return false;
	}
}

bool spBPQueueIsFull(SPBPQueue source){
	assert (source != NULL);

	int size = spBPQueueSize(source);
	int max_size = spBPQueueGetMaxSize(source);
	if (size < max_size){	// have not reached max capacity
		return false;
	}
	else{	// size == max_size
		return true;
	}
}

SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue source){

	// check input validity
	if (source == NULL){
		return SP_BPQUEUE_INVALID_ARGUMENT;
	}

	// check if the queue is empty
	if (spBPQueueIsEmpty(source)){
		return SP_BPQUEUE_EMPTY;
	}

	// the queue is not empty - removing first element is possible
	// get the first queue element and remove it
	spListGetFirst(source->queue);	//the iterator is set to the first element
									// the source queue is not empty - queue in not null
	SP_LIST_MSG msg = spListRemoveCurrent(source->queue);
	// sanity check
	if (msg != SP_LIST_SUCCESS){
		printf("Dequeue: Something is wrong in spListRemoveCurrent\n");
	}
	return SP_BPQUEUE_SUCCESS;
}


SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue source, SPListElement element){

	SP_LIST_MSG msg;
	SPListElement last, curr;
	int queue_size;

	// check input validity
	if (source == NULL || element == NULL){
		return SP_BPQUEUE_INVALID_ARGUMENT;
	}

	// check for the queue current capacity - if full check for maximal value
	// and remove it if necessary.
	if (spBPQueueIsFull(source)){
		last = spBPQueuePeekLast(source); // last is not null because source is valid
		if (spListElementCompare(element, last) >= 0){
			// last is bigger or equal to element - no need to insert
			spListElementDestroy(last);
			return SP_BPQUEUE_FULL;
		}
		else{	// remove last from queue to allow insertion
			curr = spListGetFirst(source->queue);// was tmp
			queue_size = spBPQueueSize(source);

			// after receiving the first element in the queue
			//get the last element by looping over size - 1 elements
			for (int i = 0; i < queue_size - 1; i++){
				curr = spListGetNext(source->queue); // was tmp
			}
			// current is now the last element - remove it
			msg = spListRemoveCurrent(source->queue);
			if (msg != SP_LIST_SUCCESS){
				printf("Enqueue: Something is wrong in spListRemoveCurrent");
			}
		}
	spListElementDestroy(last);
	}

	// can insert into queue
	// create a copy of element which will be inserted
	SPListElement copied_elem = spListElementCopy(element);
	if (copied_elem == NULL){
		// allocation failed
		return SP_BPQUEUE_OUT_OF_MEMORY;
	}
	// find insertion point by iterating the queue elements
	// until reaching the last element or a higher valued element.
	curr = spListGetFirst(source->queue);
	while (curr != NULL && spListElementCompare(copied_elem, curr) > 0){
		curr = spListGetNext(source->queue);
	}

	// check why we left the loop
	if (curr == NULL){	// new maximal element
		msg = spListInsertLast(source->queue, copied_elem);
	}
	else{	// insertion point found, current is now one place
			//before element's location in queue
		msg = spListInsertBeforeCurrent(source->queue, copied_elem);
	}
	// need to free copied_elem after inserting it to the queue list
	spListElementDestroy(copied_elem);
	// check insert return value
	if (msg == SP_LIST_NULL_ARGUMENT){
		return SP_BPQUEUE_INVALID_ARGUMENT;
	}
	else if(msg == SP_LIST_OUT_OF_MEMORY){
		return SP_BPQUEUE_OUT_OF_MEMORY;
	}
	else if( msg == SP_LIST_INVALID_CURRENT){
		return SP_BPQUEUE_INVALID_ARGUMENT;
	}
	else{	// success
		return SP_BPQUEUE_SUCCESS;
	}

}
