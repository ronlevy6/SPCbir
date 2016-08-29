#ifndef SPBPRIORITYQUEUE_H_
#define SPBPRIORITYQUEUE_H_
#include "SPListElement.h"
#include <stdbool.h>

/**
 * SP Bounded Priority Queue summary
 *
 * Implements a priority queue using a linked list.
 * Each queue has a bounded sized SPList linked list, which it's elements are SPListElement
 * ordered in increasing order.
 *
 * The following functions are available:
 *
 * spBPQueueCreate				- creates an empty queue with a given maximum capacity
 * spBPQueueCopy				- creates a copy of a given queue
 * spBPQueueDestroy				- frees all memory allocation associated with the queue
 * spBPQueueClear				- removes all the elements in the queue
 * spBPQueueSize				- returns the number of elements in the queue
 * spBPQueueGetMaxSize			- returns the maximum capacity of the queue
 * spBPQueueEnqueue				- Inserts a NEW COPY (must be allocated) element to the queue
 * spBPQueueDequeue				- removes the element with the lowest value
 * spBPQueuePeek				- returns a NEW COPY of the element with the lowest value
 * spBPQueuePeekLast			– returns a NEW COPY of the element with the highest value
 * spBPQueueMinValue			- returns the minimum value in the queue
 * spBPQueueMaxValue			- returns the maximum value in the queue
 * spBPQueueIsEmpty 			– returns true if the queue is empty
 * spBPQueueIsFull				- returns true if the queue is full
 *
 */


/** type used to define Bounded priority queue **/
typedef struct sp_bp_queue_t* SPBPQueue;

/** type for error reporting **/
typedef enum sp_bp_queue_msg_t {
	SP_BPQUEUE_OUT_OF_MEMORY,
	SP_BPQUEUE_FULL,
	SP_BPQUEUE_EMPTY,
	SP_BPQUEUE_INVALID_ARGUMENT,
	SP_BPQUEUE_SUCCESS
} SP_BPQUEUE_MSG;

/**
 * Allocates a new BPQueueCreate in the memory.
 * Given maxSize, The new queue will have maximum capacity of maxSize
 * The method will use SPList.createNode
 *
 * - maxSize = maxSize
 * - queue = new SPList
 *
 * The method uses spListCreate given in SPList.h
 *
 * @return
 * NULL in case allocation failure occurred OR maxSize <= 0
 * Otherwise, the new queue is returned
 */
SPBPQueue spBPQueueCreate(int maxSize);

/**
 * Creates a copy of target SPBPQueue.
 *
 * The new copy will contain all the nodes from the source queue in the same
 * order. The maxSize param will be cpoied as well
 *
 * The method uses spListCopy given in SPList.h
 *
 * @param source The target queue to copy
 * @return
 * NULL if source == NULL or if a memory allocation failed.
 * A queue containing the same elements with same order as queue otherwise.
 */
SPBPQueue spBPQueueCopy(SPBPQueue source);

/**
 * spBPQueueDestroy: Deallocates an existing queue. Clears all elements by using the
 * stored free function.
 *
 * The method uses spListDestroy given in SPList.h
 *
 * @param source Target queue to be deallocated. If source is NULL nothing will be
 * done
 */
void spBPQueueDestroy(SPBPQueue source);

/**
 * Removes all elements from target queue.
 *
 * The elements are deallocated using the stored freeing function
 *
 * The method uses spListClear given in SPList.h
 *
 * @param source Target source to remove all element from
 * @assert (source != null)
 */
void spBPQueueClear(SPBPQueue source);

/**
 * Returns the number of elements in a queue.
 *
 * The method uses spListGetSize given in SPList.h
 *
 * @param source The target queue which size is requested.
 * @return
 * -1 if a NULL pointer was sent.
 * Otherwise the number of elements in the queue.
 */
int spBPQueueSize(SPBPQueue source);

/**
 * Returns max capacity of elements in queue
 *
 * @param source The target queue which size is requested.
 * @return
 * -1 if a NULL pointer was sent.
 * Otherwise the number of elements in the queue.
 */
int spBPQueueGetMaxSize(SPBPQueue source);

/**
 * Inserts a new copy of element to the queue in the following scenarios:
 * - the queue is not at full.
 * - the queue is full but element smaller according to spListElementCompare given
 * 	in SPListElement.h
 *
 * 	this method uses the following functions: spListElementCompare given in SPListElement.h
 *@ param source - a SPBPQueue queue
 *		  element - a SPListElement element which be inserted to source
 *@ return
 * SP_BPQUEUE_INVALID_ARGUMENT - if source or element equals NULL
 * SP_BPQUEUE_OUT_OF_MEMORY - allocation failed in making element copy.
 * SP_BPQUEUE_FULL - cannot insert element into source queue
 * SP_BPQUEUE_SUCCESS - element was inserted into queue successfully
 *
 *
 */
SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue source, SPListElement element);

/**
 * removes the element with the lowest value in source queue
 *
 * this method uses the following functions: spListRemoveCurrent, spListGetFirst
 * given in SPList.h.
 *
 * @param  source - a SPBPQueue queue
 * @ return
 *  SP_BPQUEUE_INVALID_ARGUMENT - if source == NULL
 *  SP_BPQUEUE_EMPTY - if the queue is empty
 *
 */
SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue source);

/**
 * Returns new copy of the element with the lowest value.
 * In spBPQueue it means the first value in the queue since the queue elements
 * are sorted by value.
 *
 * The method uses spListGetFirst, given in SPList.h and spListElementCopy given in SPListElement.h
 *
 * @param source - queue which value will be taken from
 * @return
 * NULL if queue is empty or queue == NULL
 * else - the first value of the queue
 */
SPListElement spBPQueuePeek(SPBPQueue source);

/**
 * Returns new copy of the element with the highest value.
 * In spBPQueue it means the last value in the queue since the queue elements
 * are sorted by value in increasing order.
 *
 * The method uses spListCopy, spListGetFirst and spListGetNext given in SPList.h.
 * It also uses spListElementCopy given in SPListElement.h
 *
 * @param source - queue which value will be taken from
 * @return
 * NULL if queue is empty or queue == NULL
 * else - the last value of the queue
 */
SPListElement spBPQueuePeekLast(SPBPQueue source);

/**
 * returns the minimal element which is the value of the first element in the queue
 * since queue's elements are sorted by increasing order.
 *
 * this method uses spListElementGetValue given in SPListElement.h
 *
 * @param source - a queue
 * @return
 * -1 if a null pointer was sent or if queue is empty,
 * else - the value of the first element in the queue
 */
double spBPQueueMinValue(SPBPQueue source);

/**
 * returns the maximal element which is the value of the first element in the queue
 * since queue's elements are sorted by increasing order.
 *
 * this method uses spListElementGetValue given in SPListElement.h
 *
 * @param source - a queue
 * @return
 * -1 if a null pointer was sent or if queue is empty,
 * else - the value of the first element in the queue
 */
double spBPQueueMaxValue(SPBPQueue source);

/**
 * returns true iff the queue is empty - size == 0
 *
 * @param - a queue source
 * @assert (source != NULL)
 * @return
 * true - if size == 0
 * false - otherwise
 *
 */
bool spBPQueueIsEmpty(SPBPQueue source);

/**
 * returns true iff the queue size equals queue's max capacity
 *
 * @param - a source queue
 * @assert (source != NULL)
 * @return
 * true - if max capacity == queue size
 * false - otherwise
 */
bool spBPQueueIsFull(SPBPQueue source);

#endif
