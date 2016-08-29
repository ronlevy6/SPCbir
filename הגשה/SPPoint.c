#include <stdio.h>
#include "SPPoint.h"
#include <assert.h>
#include <stdlib.h>

#define MALLOC_ERR "Failed in memory allocation"


/* Definition of struct sp_point_t:
 * dim - amount of dimentions of the point
 * data - values of the point
 * index = index of the point
 */
struct sp_point_t{
	int dim;
	double *data;
	int index;
};
/*
 * This method created new SPPoint as written in SPPoint header file
 */
SPPoint spPointCreate(double *data, int dim, int index){

	double *input_data;
	SPPoint point = NULL;

	// check input validity
	if (data == NULL || dim <= 0 || index < 0){
		return NULL;
	}

	point = (SPPoint) malloc(sizeof(*point));
	if (point == NULL){
		return NULL;
	}

	point->dim = dim;
	point->index = index;
	//point->data = data; // Assuming the malloc was done outside - is it

	input_data = (double*) malloc(dim * sizeof(double));
	if (input_data == NULL){
		return NULL;
	}
	for (int i = 0; i < dim; i++){
		input_data[i] = data[i];
	}
	point->data = input_data;

	return point;
}

/*
 * This method gets SPPoint as input and returns copy of that point
 * as instructed in the header file
 */
SPPoint spPointCopy(SPPoint source){

	int idx,dim;
	double *data;
	SPPoint new_point;

	//Validate input
	assert (source != NULL);
	// getting source's fields
	idx = spPointGetIndex(source);
	dim = spPointGetDimension(source);
	data = source->data;

	new_point = spPointCreate(data,dim,idx);

	return new_point;
}

/*
 * Frees all memory allocation associated to point.
 */
void spPointDestroy(SPPoint point){

	if (point == NULL){
		// No point to destroy
		return;
	}
	// point is not null - free it's allocations

	free(point->data);
	free(point);
}

/*
 * This method is a getter for dim field as instructed in header field
 */
int spPointGetDimension(SPPoint point){

	int dim;

	// validate input
	assert (point != NULL);

	// point is not null
	dim = point->dim;

	return dim;
}

/*
 * Getter for index field
 */
int spPointGetIndex(SPPoint point){

	int idx;

	// validate input
	assert (point != NULL);

	idx = point->index;

	return idx;
}
/*
 * A getter for specific coordinate value as instructed in header file
 */
double spPointGetAxisCoor(SPPoint point, int axis){

	double ret_val;
	double *data;

	assert (point != NULL);
	// point is not null
	assert(axis >= 0);
	assert(axis < spPointGetDimension(point));

	data = point->data;
	ret_val = data[axis];

	return ret_val;
}

/*
 * Calculates the L2-squared distance between p and q as instructed in header file.
 */
double spPointL2SquaredDistance(SPPoint p, SPPoint q){

	int dim_p, dim_q;
	double dist,tmp_calc;

	//assert validity of input
	assert(p!=NULL);
	assert(q!=NULL);

	dim_p = spPointGetDimension(p);
	dim_q = spPointGetDimension(q);
	assert(dim_p == dim_q);

	// input is valid

	dist = 0.0;
	//calculate the L2 distance
	for (int i = 0; i < dim_p && i < dim_q; i++){
		tmp_calc = spPointGetAxisCoor(p,i) - spPointGetAxisCoor(q,i); //p[i] - q[i]
		tmp_calc = tmp_calc * tmp_calc;
		dist += tmp_calc;
	}

	return dist;

}
