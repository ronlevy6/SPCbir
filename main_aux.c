#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "main_aux.h"



#define GENERAL_ERROR_MSG "An error occurred"
#define PCA_DIM_ERROR_MSG "PCA dimension couldn't be resolved"
#define PCA_FILE_NOT_EXIST "PCA file doesn't exist"
#define PCA_FILE_NOT_RESOLVED "PCA filename couldn't be resolved"
#define NUM_OF_IMAGES_ERROR "Number of images couldn't be resolved"
#define NUM_OF_FEATS_ERROR "Number of features couldn't be resolved"
#define MINIMAL_GUI_ERROR "Minimal GUI mode couldn't be resolved"
#define IMAGE_PATH_ERROR "Image path couldn't be resolved"
#define FEAT_PATH_ERROR "Features path couldn't be resolved"
#define IMAGE_NOT_EXIST_MSG ": Images doesn't exist"
#define MINIMAL_GUI_NOT_SET_WARNING "Cannot display images in non-Minimal-GUI mode"
#define ALLOC_ERROR_MSG "Allocation error"
#define INVALID_ARG_ERROR "Invalid arguments"
#define CAN_NOT_OPEN_FILE "Can not open file"
#define CAN_NOT_CREATE_POINT "Point creation failed"
#define CAN_NOT_COPY "KDArray copy failed"
#define ALLOC_MATRIX_ERROR "Matrix allocation error"
#define SPLIT_MAP_ERROR "Error creating the split map"
#define SPLIT_ERR "Error splitting KDArray"
#define CANNOT_CREATE_QUEUE "Cannot create BPQueue"

#define INF 1.0/0.0

#define FREE_AND_NULL(ptr) do { \
								if ((ptr)!= NULL) { \
									free((void*)(ptr)); \
									(ptr) = NULL; \
								} \
} while(0);



PKDArray init(SPPoint* points_arr, int size, SP_AUX_MSG* msg){

	PKDArray result;

	spLoggerPrintInfo("Entered init of KD-Array\n");

	if (points_arr == NULL || size <= 0){
		spLoggerPrintError(INVALID_ARG_ERROR,__FILE__,__func__,__LINE__);
		*msg = SP_AUX_INVALID_ARGUMENT;
		return NULL;
	}

	// allocate memory for kd array
	result = (PKDArray)malloc(sizeof(*result));
	if (result == NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG,__FILE__,__func__,__LINE__);
		*msg = SP_AUX_ALLOC_FAIL;
		return NULL;
	}
	result->size = size;

	result->data = (KDElement*)malloc(sizeof(*(result->data)) * size);
	if (result->data == NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG,__FILE__,__func__,__LINE__);
		FREE_AND_NULL(result);
		*msg = SP_AUX_ALLOC_FAIL;
		return NULL;
	}

	for (int i = 0; i < size; i++){
		result->data[i].index = i;
		result->data[i].point = spPointCopy(points_arr[i]); // before it was just assign
		result->data[i].pivot = -1;
	}

	*msg = SP_AUX_SUCCESS;
	return result;
}

PKDArray copyArray(PKDArray arr, int start, int end, SP_AUX_MSG* msg){

	PKDArray copy = NULL;
	int size = end - start, j = 0;

	spLoggerPrintInfo("Entered copyArray\n");

	if (arr == NULL){
		spLoggerPrintError(INVALID_ARG_ERROR,__FILE__,__func__,__LINE__);
		*msg = SP_AUX_INVALID_ARGUMENT;
		return NULL;
	}

	// allocate memory for kd array
	copy = (PKDArray)malloc(sizeof(*copy));
	if (copy == NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG,__FILE__,__func__,__LINE__);
		*msg = SP_AUX_ALLOC_FAIL;
		return NULL;
	}
	copy->size = size;

	copy->data = (KDElement*)malloc(sizeof(*(copy->data)) * size);
	if (copy->data == NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG,__FILE__,__func__,__LINE__);
		FREE_AND_NULL(copy);
		*msg = SP_AUX_ALLOC_FAIL;
		return NULL;
	}

	// copy data from arr

	for (int i = start; i < end; i++){

		copy->data[j].index = arr->data[i].index; // save the kd array original index
		copy->data[j].point = spPointCopy(arr->data[i].point);
		copy->data[j].pivot = arr->data[i].pivot;
		j++; // copy->data must start from 0 and arr->data may not
	}

	*msg = SP_AUX_SUCCESS;
	return copy;
}

void DestroyKDArray(PKDArray arr){

	if (arr == NULL){
		return;
	}

	// destroy all inner points
	for (int i = 0; i < arr->size; i++){
		spPointDestroy(arr->data[i].point);
	}

	// destroy all KDElements
	FREE_AND_NULL(arr->data);
	FREE_AND_NULL(arr);

}

PMatrix allocate_mem_matrix(int num_of_rows,SP_AUX_MSG* msg){
	PMatrix mat = NULL;

	mat = (PMatrix)malloc(sizeof(*mat));
	if (mat == NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG,__FILE__,__func__,__LINE__);
		*msg = SP_AUX_ALLOC_FAIL;
		return NULL;
	}
	mat->matrix = (int**)malloc(sizeof(int*) * num_of_rows);
	if (mat->matrix == NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG,__FILE__,__func__,__LINE__);
		FREE_AND_NULL(mat);
		*msg = SP_AUX_ALLOC_FAIL;
		return NULL;
	}

	*msg = SP_AUX_SUCCESS;
	return mat;
}

PMatrix createMatrix(PKDArray kd_arr, int dim, SP_AUX_MSG* msg){

	PMatrix mat;
	PKDArray sort_arr;

	if (kd_arr == NULL){
		spLoggerPrintError(INVALID_ARG_ERROR,__FILE__,__func__,__LINE__);
		*msg = SP_AUX_INVALID_ARGUMENT;
		return NULL;
	}


	// allocate memory
	mat = allocate_mem_matrix(dim, msg);
	if (mat == NULL){
		spLoggerPrintError(ALLOC_MATRIX_ERROR,__FILE__,__func__,__LINE__);
		*msg = SP_AUX_MATRIX_ERR;
		return NULL;
	}
	mat->rows = dim; //d
	mat->cols = kd_arr->size; //n;
	mat->kd_array = copyArray(kd_arr, 0, kd_arr->size, msg);
	if (mat->kd_array == NULL){
		spLoggerPrintError(CAN_NOT_COPY,__FILE__,__func__,__LINE__);
		FREE_AND_NULL(mat->matrix);
		FREE_AND_NULL(mat);
		*msg = SP_AUX_COPY_ARR_FAIL;
		return NULL;
	}

	// make a copy of the kd array to be sorted in each iteration
	sort_arr = copyArray(kd_arr, 0, kd_arr->size, msg);
	if (sort_arr == NULL){
		spLoggerPrintError(CAN_NOT_COPY,__FILE__,__func__,__LINE__);
		FREE_AND_NULL(mat->matrix);
		FREE_AND_NULL(mat);
		*msg = SP_AUX_COPY_ARR_FAIL;
		return NULL;
	}


	// building the matrix's rows one by one
	for(int i = 0; i < mat->rows; i++){
		for(int j = 0; j < mat->cols; j++){
			sort_arr->data[j].pivot = i;	// update the pivot by whom we will sort the array
		}
		// sort copied array data by dimension pivot
		qsort(sort_arr->data, mat->cols, sizeof(KDElement), compare);

		// insert the sorted row into the matrix
		mat->matrix[i] = (int*)malloc(sizeof(int) * mat->cols);
		if (mat->matrix[i] == NULL){
			spLoggerPrintError(ALLOC_ERROR_MSG,__FILE__,__func__,__LINE__);
			DestroyMatrix(mat, i);
			*msg = SP_AUX_ALLOC_FAIL;
			return NULL;
		}
		// assign indices
		for(int j = 0; j < mat->cols; j++){
			mat->matrix[i][j] = sort_arr->data[j].index;
		}
	}

	DestroyKDArray(sort_arr);
	*msg = SP_AUX_SUCCESS;
	return mat;
}


void DestroyMatrix(PMatrix mat, int rows){

	if (mat == NULL){
		return;
	}
	for(int i = 0; i < rows; i++){
		FREE_AND_NULL(mat->matrix[i]);
	}
	FREE_AND_NULL(mat->matrix);
	DestroyKDArray(mat->kd_array);
	FREE_AND_NULL(mat);

}

int compare(const void* pa, const void* pb) {

	KDElement fa = *((KDElement*) pa);
	KDElement fb = *((KDElement*) pb);

	if (spPointGetAxisCoor(fa.point, fa.pivot) > spPointGetAxisCoor(fb.point, fb.pivot)){
		return 1;
	}
	else if (spPointGetAxisCoor(fa.point, fa.pivot) <  spPointGetAxisCoor(fb.point, fb.pivot)){
		return -1;
	}
	else{
		return 0;
	}

}

int compare_idx(const void* pa, const void* pb) {

	KDElement fa = *((KDElement*) pa);
	KDElement fb = *((KDElement*) pb);

	if (fa.index > fb.index){
		return 1;
	}
	else if (fa.index < fb.index){
		return -1;
	}
	else{
		return 0;
	}

}

int comapre_img_cnt_desc(const void* pa, const void* pb) {
	ImageCounter fa = *((ImageCounter*) pa);
	ImageCounter fb = *((ImageCounter*) pb);

	// decreasing order by count
	if (fa.cnt < fb.cnt){
		return 1;
	}
	else if (fa.cnt > fb.cnt){
		return -1;
	}
	else{
		// count is the same - sort by index - now in ascending order
		if (fa.img_idx > fb.img_idx){
			return 1;
		}
		else{
			return -1;
		}
	}
}

int* createNewIdx(PMatrix pmat, int* split_map, int indicator, SP_AUX_MSG* msg){
	int* result = NULL;
	int idx_cnt = 0;

	spLoggerPrintInfo("Entered createNewIdx\n");

	result = (int*)malloc(sizeof(int) * pmat->cols);
	if (result == NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG,__FILE__,__func__,__LINE__);
		*msg = SP_AUX_ALLOC_FAIL;
		return NULL;
	}

	for(int i = 0; i < pmat->cols; i++){
		if (split_map[i] == indicator){ // correct mapping
			result[i] = idx_cnt;
			idx_cnt++;
		}
		else{
			result[i] = -1;
		}
	}

	*msg = SP_AUX_SUCCESS;
	return result;
}


int* createSplitMap(PMatrix pmat, int coor, int mid, SP_AUX_MSG* msg){
	int* result = NULL, *coor_arr;

	spLoggerPrintInfo("Entered createSplitMap\n");

	result = (int *)malloc(sizeof(int) * pmat->cols);
	if (result == NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG,__FILE__,__func__,__LINE__);
		*msg = SP_AUX_ALLOC_FAIL;
		return NULL;
	}

	coor_arr = pmat->matrix[coor];
	for (int i = 0; i < pmat->cols; i++){
		if (i < mid){
			result[coor_arr[i]] = 0;	// separating the elements into two groups
		}
		else{// i >= mid
			result[coor_arr[i]] = 1;
		}
	}

	*msg = SP_AUX_SUCCESS;
	return result;

}

PKDArray create_array_by_coor(PKDArray arr, int* sorted_idx_arr){

	PKDArray result;
	int idx;

	result = (PKDArray)malloc(sizeof(*result));
	if (result == NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG,__FILE__,__func__,__LINE__);
		return NULL;
	}
	result->size = arr->size;

	result->data = (KDElement*)malloc(sizeof(*(result->data)) * arr->size);
	if (result->data == NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG,__FILE__,__func__,__LINE__);
		FREE_AND_NULL(result);
		return NULL;
	}

	// for each index add to result create the sorted array
	for(int i = 0; i < result->size; i++){
		idx = sorted_idx_arr[i];
		result->data[i].point = spPointCopy(arr->data[idx].point);
		result->data[i].index = arr->data[idx].index;
		result->data[i].pivot = arr->data[idx].pivot;
	}

	return result;
}



PMatrix* split(PMatrix pmatrix, int coor, SP_AUX_MSG* msg){

	PMatrix* result = NULL;
	PMatrix left, right;
	PKDArray sorted_coor_arr;
	int mid = -1, n, l_idx_cnt=0, r_idx_cnt=0, prev_idx, curr_idx;
	int *l_map, *r_map, *split_map, *idx_sorted_by_coor;

	spLoggerPrintInfo("Entered Split\n");

	if (pmatrix == NULL){
		spLoggerPrintError(INVALID_ARG_ERROR,__FILE__,__func__,__LINE__);
		*msg = SP_AUX_INVALID_ARGUMENT;
		return NULL;
	}

	// check coor validity
	if (coor < 0 || coor >= pmatrix->rows){
		spLoggerPrintError(INVALID_ARG_ERROR,__FILE__,__func__,__LINE__);
		*msg = SP_AUX_INVALID_ARGUMENT;
		return NULL;
	}

	// get sizes
	n = pmatrix->cols;
	mid = (int)(0.5 +(n / 2.0)); // ceil n\2 up using 2.0 to avoid cast into int

	// allocate memory for left
	//printf("in SPLIT before allocate left\n");
	//fflush(NULL);

	// get the kd array sorted according coor
	idx_sorted_by_coor = pmatrix->matrix[coor];
	sorted_coor_arr = create_array_by_coor(pmatrix->kd_array, idx_sorted_by_coor);
	if (sorted_coor_arr == NULL){
		spLoggerPrintError(GENERAL_ERROR_MSG,__FILE__,__func__,__LINE__);
		*msg = SP_AUX_GENERAL_MSG;
		return NULL;
	}

	left = allocate_mem_matrix(pmatrix->rows, msg);
	if (left == NULL){
		spLoggerPrintError(ALLOC_MATRIX_ERROR,__FILE__,__func__,__LINE__);
		DestroyKDArray(sorted_coor_arr);
		*msg = SP_AUX_MATRIX_ERR;
		return NULL;
	}
	left->cols = mid;
	left->rows = pmatrix->rows;


	// create left KD array - first mid elements only
	left->kd_array = copyArray(sorted_coor_arr, 0, mid, msg);
	if (left->kd_array == NULL){
		spLoggerPrintError(ALLOC_MATRIX_ERROR,__FILE__,__func__,__LINE__);
		*msg = SP_AUX_COPY_ARR_FAIL;
		DestroyKDArray(sorted_coor_arr);
		FREE_AND_NULL(left->matrix);
		FREE_AND_NULL(left);
		return NULL;
	}

/*
	for(int i = 0; i < left->kd_array->size; i++){
		printf(" BEFORE QSORT: i = %d, left[i] = %f, %f, kdarray index = %d\n",i, spPointGetAxisCoor(left->kd_array->data[i].point,0),
				spPointGetAxisCoor(left->kd_array->data[i].point,1), left->kd_array->data[i].index);
		fflush(NULL);
	}
*/


	// sort by index after extracting
	qsort(left->kd_array->data, left->cols, sizeof(KDElement), compare_idx);
/*
	// print the idx to see that its fixed
	for(int i = 0; i < left->kd_array->size; i++){
		printf(" AFTER QSORT: i = %d, left[i] = %f, %f, kdarray index = %d\n",i, spPointGetAxisCoor(left->kd_array->data[i].point,0),
				spPointGetAxisCoor(left->kd_array->data[i].point,1), left->kd_array->data[i].index);
		fflush(NULL);
	}
*/



	for (int i = 0; i < mid; i++){	// regenerate indices to the new kd array according to split
		left->kd_array->data[i].index = i;
	}

	//printf("in SPLIT after allocate left before right\n");
	//fflush(NULL);
	//allocate memory for right
	right = allocate_mem_matrix(pmatrix->rows, msg);
	if (right == NULL){
		spLoggerPrintError(ALLOC_MATRIX_ERROR,__FILE__,__func__,__LINE__);
		*msg = SP_AUX_MATRIX_ERR;
		DestroyMatrix(left, 0);
		DestroyKDArray(sorted_coor_arr);
		return NULL;
	}
	right->cols = n-mid;
	right->rows = pmatrix->rows;


	// create right KD array - last (n - mid) elements only
	right->kd_array = copyArray(sorted_coor_arr, mid, n, msg);
	if (right->kd_array == NULL){
		spLoggerPrintError(ALLOC_MATRIX_ERROR,__FILE__,__func__,__LINE__);
		*msg = SP_AUX_COPY_ARR_FAIL;
		DestroyMatrix(left, 0);
		DestroyKDArray(sorted_coor_arr);
		FREE_AND_NULL(right->matrix);
		FREE_AND_NULL(right);
		return NULL;
	}
/*
	for(int i = 0; i < right->kd_array->size; i++){
		printf(" BEFORE QSORT: i = %d, right[i] = %f, %f, kdarray index = %d\n",i, spPointGetAxisCoor(right->kd_array->data[i].point,0),
				spPointGetAxisCoor(right->kd_array->data[i].point,1), right->kd_array->data[i].index);
		fflush(NULL);
	}
*/
	// sort the array according to index
	qsort(right->kd_array->data, right->cols, sizeof(KDElement), compare_idx);
/*
	for(int i = 0; i < right->kd_array->size; i++){
		printf(" AFTER QSORT: i = %d, right[i] = %f, %f, kdarray index = %d\n",i, spPointGetAxisCoor(right->kd_array->data[i].point,0),
				spPointGetAxisCoor(right->kd_array->data[i].point,1), right->kd_array->data[i].index);
		fflush(NULL);
	}
*/
	for (int i = 0; i < n-mid; i++){	// regenerate indices to the new kd array according to split
		right->kd_array->data[i].index = i;
	}

	//printf("in SPLIT after right allocations before split map\n");
	//fflush(NULL);
	// create array of 0\1: 0 for left and 1 for right
	split_map = createSplitMap(pmatrix, coor, mid, msg);

	if (split_map == NULL){
		spLoggerPrintError(SPLIT_MAP_ERROR,__FILE__,__func__,__LINE__);
		*msg = SP_AUX_SPLIT_MAP_ERR;
		DestroyKDArray(sorted_coor_arr);
		DestroyMatrix(left, 0);
		DestroyMatrix(right, 0);
		return NULL;
	}
	//printf("in SPLIT after split map creation before l_map creation\n");
	//fflush(NULL);
	// use l_map and r_map to split the matrix into two parts
	l_map = createNewIdx(pmatrix, split_map, 0, msg);

	if (l_map ==NULL){
		spLoggerPrintError(SPLIT_MAP_ERROR,__FILE__,__func__,__LINE__);
		*msg = SP_AUX_SPLIT_MAP_ERR;
		DestroyMatrix(left, 0);
		DestroyMatrix(right, 0);
		DestroyKDArray(sorted_coor_arr);
		FREE_AND_NULL(split_map);
		return NULL;
	}

	r_map = createNewIdx(pmatrix, split_map, 1, msg);
	if (r_map ==NULL){
		spLoggerPrintError(SPLIT_MAP_ERROR,__FILE__,__func__,__LINE__);
		*msg = SP_AUX_SPLIT_MAP_ERR;
		DestroyMatrix(left, 0);
		DestroyMatrix(right, 0);
		DestroyKDArray(sorted_coor_arr);
		FREE_AND_NULL(split_map);
		FREE_AND_NULL(l_map);
		return NULL;
	}

	//printf("in SPLIT after r_map before new idx update\n");
	//fflush(NULL);
	// update left and right matrices values - new indices
	for(int i = 0; i < pmatrix->rows; i++){

		left->matrix[i] = (int*)malloc(sizeof(int) * left->cols);
		if (left->matrix[i] == NULL){
			spLoggerPrintError(ALLOC_ERROR_MSG,__FILE__,__func__,__LINE__);
			*msg = SP_AUX_ALLOC_FAIL;
			DestroyKDArray(sorted_coor_arr);
			DestroyMatrix(left,i);
			DestroyMatrix(right,i - 1); // because left is allocated before right
			FREE_AND_NULL(split_map);
			FREE_AND_NULL(l_map);
			FREE_AND_NULL(r_map);
			return NULL;
		}
		right->matrix[i] = (int*)malloc(sizeof(int) * right->cols);
		if (right->matrix[i] == NULL){
			spLoggerPrintError(ALLOC_ERROR_MSG,__FILE__,__func__,__LINE__);
			*msg = SP_AUX_ALLOC_FAIL;
			DestroyKDArray(sorted_coor_arr);
			DestroyMatrix(right,i);
			DestroyMatrix(left, i);
			FREE_AND_NULL(split_map);
			FREE_AND_NULL(l_map);
			FREE_AND_NULL(r_map);
			return NULL;
		}

		l_idx_cnt = 0;
		r_idx_cnt = 0;

		for(int j = 0; j < pmatrix->cols; j++){
			prev_idx = pmatrix->matrix[i][j];
			if (split_map[prev_idx] == 0){ // left
				curr_idx = l_map[prev_idx]; // get the new index according to split
				left->matrix[i][l_idx_cnt] = curr_idx;
				l_idx_cnt++;
			}
			if (split_map[prev_idx] == 1){ // right
				curr_idx = r_map[prev_idx]; // get the new index according to split
				right->matrix[i][r_idx_cnt] = curr_idx;
				r_idx_cnt++;
			}
		}
	}
	//printf("in SPLIT after big i loop\n");
	//fflush(NULL);
	// create the array that will contain the results - left in index 0 and right in index 1
	result = (PMatrix*)malloc(sizeof(*result) * 2);// added dereferencing, should be like that?
	if (result == NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG,__FILE__,__func__,__LINE__);
		*msg = SP_AUX_ALLOC_FAIL;
		DestroyKDArray(sorted_coor_arr);
		DestroyMatrix(left, left->rows);
		DestroyMatrix(right, right->rows);
		FREE_AND_NULL(l_map);
		FREE_AND_NULL(r_map);
		FREE_AND_NULL(split_map);
		return NULL;
	}
	//printf("in SPLITbefore final assignment to result\n");
	//fflush(NULL);
	// assign values to result
	result[0] = left;
	result[1] = right;

	// free unneeded allocations
	DestroyKDArray(sorted_coor_arr);
	FREE_AND_NULL(l_map);
	FREE_AND_NULL(r_map);
	FREE_AND_NULL(split_map);
/*
	// this is a print for debuggggs
	printf("in SPLIT kd array right size = %d\n",right->kd_array->size);
	fflush(NULL);
	for(int i = 0; i < right->kd_array->size; i++){
		printf("i = %d, right[i] = %f, %f\n",i, spPointGetAxisCoor(right->kd_array->data[i].point,0),
				spPointGetAxisCoor(right->kd_array->data[i].point,1));
		fflush(NULL);
	}
	printf("in SPLIT kd array left size = %d\n",left->kd_array->size);
	fflush(NULL);

	for (int i = 0; i < left->kd_array->size; i++){
		printf("in SPLIT i = %d, kd array coor at 0 = %f coor at 1 = %f\n",i,spPointGetAxisCoor(left->kd_array->data[i].point,0),
				spPointGetAxisCoor(left->kd_array->data[i].point,1));
		fflush(NULL);
	}
*/
	*msg = SP_AUX_SUCCESS;
	//printf("in SPLIT FINISHED\n");
	//fflush(NULL);
	return result;
}


//######################### PRE PROCCESS - write is in main.cpp ###########################//
/*
void writeFeaturesToFile(SPConfig config){
	int num_of_images, num_of_features, pca_dim;
	char *image_path,*feats_path;
	SP_CONFIG_MSG msg = SP_CONFIG_SUCCESS;
	FILE * fp;
	SPPoint* res_point;
	SPPoint point;

	if (config == NULL){
		spLoggerPrintError(INVALID_ARG_ERROR,__FILE__,__func__,__LINE__);
		return;
	}

	// get needed values from config
	num_of_images = spConfigGetNumOfImages(config,&msg);
	if (msg != SP_CONFIG_SUCCESS) {
		spLoggerPrintError(NUM_OF_IMAGES_ERROR, __FILE__, __func__, __LINE__);
		return;
	}

	num_of_features = spConfigGetNumOfFeatures(config,&msg);
	if (msg != SP_CONFIG_SUCCESS){
		spLoggerPrintError(NUM_OF_FEATS_ERROR,__FILE__,__func__,__LINE__);
		return;
	}

	pca_dim = spConfigGetPCADim(config, &msg);
	if (msg != SP_CONFIG_SUCCESS){
		spLoggerPrintError(PCA_DIM_ERROR_MSG,__FILE__,__func__,__LINE__);
		return;
	}


	for (int i = 0; i < num_of_images; i++){

		// get image path to be used in the extraction of features
		image_path = (char*)malloc(MAX_LINE_LEN);
		if (image_path == NULL){
			spLoggerPrintError(ALLOC_ERROR_MSG,__FILE__,__func__,__LINE__);
			return;
		}
		msg = spConfigGetImagePath(image_path,config,i);
		if (msg != SP_CONFIG_SUCCESS){
			spLoggerPrintError(IMAGE_PATH_ERROR,__FILE__,__func__,__LINE__);
			return;
		}

		// create the .feats file path for the i'th image by overriding the last 3
		// string characters and replacing them in .feat suffix
		feats_path = (char*)malloc(MAX_LINE_LEN);
		if (feats_path == NULL){
			spLoggerPrintError(ALLOC_ERROR_MSG,__FILE__,__func__,__LINE__);
			return;
		}
		msg = spConfigGetFeatsPath(feats_path, config, i);
		if (msg != SP_CONFIG_SUCCESS){
			spLoggerPrintError(FEAT_PATH_ERROR,__FILE__,__func__,__LINE__);
			return;
		}

		fp = fopen(feats_path, "w");
		if (fp == NULL){
			spLoggerPrintError(CAN_NOT_OPEN_FILE,__FILE__,__func__,__LINE__);
			return;
		}
		// feat_path is OK to write to
		sp::ImageProc improc = sp::ImageProc((const SPConfig) config);
		res_point = improc.getImageFeatures((const char*)image_path, i, &num_of_features);
		// write the first line as presented in the file's format
		fprintf(fp, "%d %d %d\n", num_of_features, pca_dim,i); // check if \n is necessary

		// write each point data into a different line in fp
		for (int j = 0; j < num_of_features; j++){
			point = res_point[j];
			for (int k = 0; k < pca_dim; k++){
				fprintf(fp, "%f ", spPointGetAxisCoor(point, k));
			}
			// done entering point's data - feature j
			fprintf(fp, "%s", "\n");
		}// end of j loop
		fclose(fp);
		free(feats_path);
		free(image_path);
		//free res point array
		for(int q = 0; q < num_of_features; q++){
			spPointDestroy(res_point[q]);
		}
		free(res_point);

	}// end of i loop
}
*/
SPPoint* readFeaturesFromFile(SPConfig config, int* size){

	int num_of_images,ffeatures,findex,fpcadim,
		point_arr_cnt = 0, arr_size = -1;
	char *feats_path;
	double read_val;
	SP_CONFIG_MSG msg = SP_CONFIG_SUCCESS;
	FILE * fp;
	SPPoint* res_point;
	SPPoint point;
	double* curr_data;

	//printf("entered readFeaturesFromFile\n");
	//fflush(NULL);

	if (config == NULL){
		spLoggerPrintError(INVALID_ARG_ERROR,__FILE__,__func__,__LINE__);
		return NULL;
	}
	// array size is the number of all features in all the
	//images in the directory referenced by config.
	//printf("before getTotalNumOfFeatures\n");
	//fflush(NULL);
	arr_size = getTotalNumOfFeatures(config);
	if (arr_size == -1){ // failed to extract total number of features
		spLoggerPrintError(GENERAL_ERROR_MSG,__FILE__,__func__,__LINE__);
		return NULL;
	}
	//printf("before malloc of res_point\n");
	//fflush(NULL);
	res_point = (SPPoint*)malloc(sizeof(point) * arr_size);
	if (res_point == NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG,__FILE__,__func__,__LINE__);
		return NULL;
	}
	// get needed values from config

	//printf("Before Num of Images\n");
	//fflush(NULL);
	num_of_images = spConfigGetNumOfImages(config, &msg);
	if (msg != SP_CONFIG_SUCCESS) {
		spLoggerPrintError(NUM_OF_IMAGES_ERROR, __FILE__, __func__, __LINE__);
		free(res_point);
		return NULL;
	}

	//printf("Before i loop\n");
	//fflush(NULL);
	for (int i = 0; i < num_of_images; i++){
		// create the .feats file path for the i'th image by overriding the last 3
		// string characters and replacing them in .feat suffix
		feats_path = (char*)malloc(MAX_LINE_LEN);
		if (feats_path == NULL){
			spLoggerPrintError(ALLOC_ERROR_MSG,__FILE__,__func__,__LINE__);
			free_point_arr(res_point, point_arr_cnt);
			return NULL;
		}
		//printf("Before spConfigGetFeatsPath\n");
		//fflush(NULL);
		msg = spConfigGetFeatsPath(feats_path, config, i);
		if (msg != SP_CONFIG_SUCCESS){
			spLoggerPrintError(FEAT_PATH_ERROR,__FILE__,__func__,__LINE__);
			free_point_arr(res_point, point_arr_cnt);
			free(feats_path);
			return NULL;
		}

		fp = fopen(feats_path, "r");
		if (fp == NULL){
			spLoggerPrintError(CAN_NOT_OPEN_FILE,__FILE__,__func__,__LINE__);
			free_point_arr(res_point, point_arr_cnt);
			free(feats_path);
			return NULL;
		}

		fscanf(fp, "%d %d %d", &ffeatures, &fpcadim, &findex);// TO DO  check if there is a need in \n

		// after reading first line - process features
		for (int j = 0; j < ffeatures; j++){
			// create a tmp array that will be used in creating a new point
			curr_data = (double*)malloc(sizeof(double) * fpcadim);
			if (curr_data == NULL){
				spLoggerPrintError(ALLOC_ERROR_MSG,__FILE__,__func__,__LINE__);
				// free all previous allocations
				free_point_arr(res_point, point_arr_cnt);
				free(feats_path);
				fclose(fp);
				return NULL;
			}

			for (int k = 0; k <fpcadim; k++){
				fscanf(fp,"%lf", &read_val);// lf works fine? or should it be just f?
				curr_data[k] = read_val;
			}
			point = spPointCreate(curr_data, fpcadim, findex);
			// check if point was successfully created
			if (point == NULL){
				spLoggerPrintError(CAN_NOT_CREATE_POINT,__FILE__,__func__,__LINE__);
				// free all previous allocations
				free_point_arr(res_point, point_arr_cnt);
				free(curr_data);
				free(feats_path);
				fclose(fp);
				return NULL;
			}
			res_point[point_arr_cnt] = point;
			point_arr_cnt++;
			free(curr_data);
		}
		// free resources
		free(feats_path);
		fclose(fp);
	} // end of i loop
	//printf("After i loop, before return\n");
	//fflush(NULL);
	*size = arr_size; // update array size
	return res_point;
}


int getTotalNumOfFeatures(SPConfig config){
	int num_of_images, ffeatures, all_feats = 0;
	char * feats_path;
	FILE* fp;
	SP_CONFIG_MSG msg;


	//printf("entered getTotalNumOfFeatures\n");
	//fflush(NULL);

	if (config == NULL){
		spLoggerPrintError(INVALID_ARG_ERROR,__FILE__,__func__,__LINE__);
		return -1;
	}

	//printf("getTotalNumOfFeatures - before get num of images\n");
	//fflush(NULL);
	num_of_images = spConfigGetNumOfImages(config,&msg);
	if (msg != SP_CONFIG_SUCCESS) {
		spLoggerPrintError(NUM_OF_IMAGES_ERROR, __FILE__, __func__, __LINE__);
		return -1;
	}

	//printf("getTotalNumOfFeatures - before i loop\n");
	//fflush(NULL);
	// for each image read the first line from the .feats file and extract number of features
	for (int i = 0; i < num_of_images; i ++){
		feats_path = (char*)malloc(MAX_LINE_LEN);
		if (feats_path == NULL){
			spLoggerPrintError(ALLOC_ERROR_MSG,__FILE__,__func__,__LINE__);
			return -1;
		}
		//printf("getTotalNumOfFeatures - before get feats path\n");
		//fflush(NULL);
		msg = spConfigGetFeatsPath(feats_path, config, i);

		//printf("feats_path = %s\n",feats_path);
		//fflush(NULL);

		if (msg != SP_CONFIG_SUCCESS){
			spLoggerPrintError(FEAT_PATH_ERROR,__FILE__,__func__,__LINE__);
			free(feats_path);
			return -1;
		}

		//printf("getTotalNumOfFeatures - before open file\n");
		//fflush(NULL);
		fp = fopen(feats_path, "r");
		if (fp == NULL){
			spLoggerPrintError(CAN_NOT_OPEN_FILE,__FILE__,__func__,__LINE__);
			free(feats_path);
			return -1;
		}

		//printf("getTotalNumOfFeatures - before assigns\n");
		//fflush(NULL);

		fscanf(fp, "%d", &ffeatures);
		all_feats += ffeatures;
		fclose(fp);
		free(feats_path);
		//printf("getTotalNumOfFeatures - finished iteration\n");
		//fflush(NULL);
	}
	//printf("getTotalNumOfFeatures - after i loop\n");
	//fflush(NULL);

	return all_feats;
}

void free_point_arr(SPPoint* array, int size){
	for (int i = 0; i < size; i++){
		spPointDestroy(array[i]);
	}
	free(array);
}


//######################### KD TREE ##################################//

int get_max_spread(PMatrix pmatrix){

	int res_dim, min_idx, max_idx;
	double max_val, min_val, max_spread = -1.0, tmp_spread;

	for (int i = 0; i < pmatrix->rows; i++){
		min_idx = pmatrix->matrix[i][0];	// first value in each matrix's row is the minimun
		max_idx = pmatrix->matrix[i][pmatrix->cols-1];	// last value of each matrix's row is the highest

		// extract the values of the min and max points from kd array
		//according to their index for each dimension
		min_val = spPointGetAxisCoor(pmatrix->kd_array->data[min_idx].point, i);
		max_val = spPointGetAxisCoor(pmatrix->kd_array->data[max_idx].point, i);

		tmp_spread = max_val - min_val;

		if (tmp_spread > max_spread){
			max_spread = tmp_spread;
			res_dim = i;
		}
	}

	return res_dim;
}

int get_split_dim(PMatrix pmatrix, int prev_split_dim, SP_KDTREE_SPLIT_METHOD method){

	int result, num_of_dims;

	num_of_dims = pmatrix->rows;

	if (method == INCREMENTAL){
		result = (prev_split_dim + 1) % num_of_dims;
	}
	else if (method == RANDOM){ // choose a random dimension in range 0 - dim
		srand(time(NULL));	// to generate a different values
		result = rand() % (num_of_dims - 0) + 0; //according to: rand() % (high - low) + low;
	}
	else if (method == MAX_SPREAD){
		result = get_max_spread(pmatrix);
	}
	else{// something went wrong - did not get one of the ligall methods
		spLoggerPrintError(INVALID_ARG_ERROR,__FILE__,__func__,__LINE__);
		result = -1;
	}

	return result;
}

double get_median_value(PMatrix pmatrix, int split_dim){

	double median_value;
	int n, mid, median_idx;
	n = pmatrix->cols;
	mid = (int)(0.5 +(n / 2.0)); // ceil n\2 up using 2.0 to avoid cast into int
	if (n % 2 != 0){ // n is odd
		mid = mid -1;	// array index starts with zero
	}
	//printf("in get_median_value, split dim = %d, mid = %d, n = %d\n", split_dim, mid, n);

	median_idx = pmatrix->matrix[split_dim][mid];
	median_value = spPointGetAxisCoor(pmatrix->kd_array->data[median_idx].point, split_dim);

	return median_value;
}

PKDTreeNode create_node(int split_dim, double median){

	PKDTreeNode node = NULL;
	node = (PKDTreeNode)malloc(sizeof(*node));
	if (node == NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG,__FILE__,__func__,__LINE__);
		return NULL;
	}

	node->dim = split_dim;
	node->val = median;
	node->data = NULL;
	node->left = NULL;
	node->right = NULL;

	return node;
}

PKDTreeNode create_leaf(SPPoint point){

	PKDTreeNode node = NULL;
	node = (PKDTreeNode)malloc(sizeof(*node));
	if (node == NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG,__FILE__,__func__,__LINE__);
		return NULL;
	}

	node->dim = INF;
	node->val = INF;
	node->data = spPointCopy(point);
	node->left = NULL;
	node->right = NULL;

	return node;
}

PKDTreeNode create_tree(PMatrix pmatrix, SP_KDTREE_SPLIT_METHOD split_method, int prev_split_dim){

	// consider adding sanity checks for input validation
	PKDTreeNode node = NULL;
	SPPoint point = NULL;
	PMatrix left, right;
	PMatrix *split_result;
	int kd_arr_size, split_dim;
	double median;

	SP_AUX_MSG msg = SP_AUX_SUCCESS;

	kd_arr_size = pmatrix->kd_array->size;

	// stop condition size is 1 create and return node
	if (kd_arr_size == 1){
		point = pmatrix->kd_array->data[0].point;	// only one point left
		node = create_leaf(point);
		if (node == NULL){
			spLoggerPrintError(ALLOC_ERROR_MSG,__FILE__,__func__,__LINE__);
			return NULL;
		}
		return node;
	}

	// need to split the kd array
	// get the next split dimension and median value corresponds to that dimension
	split_dim = get_split_dim(pmatrix, prev_split_dim, split_method);
	// check split_dim is valid
	if (split_dim == -1){
		spLoggerPrintError(INVALID_ARG_ERROR,__FILE__,__func__,__LINE__);
		DestroyMatrix(pmatrix, pmatrix->rows);
		return NULL;
	}
	median = get_median_value(pmatrix, split_dim);

	// create new node

	node = create_node(split_dim, median);
	if (node == NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG,__FILE__,__func__,__LINE__);
		DestroyMatrix(pmatrix, pmatrix->rows);
		return NULL;
	}

	//printf("val = %f, median = %f split dim = %d\n", node->val, median, split_dim);

	// split the kd array according to split dim
	split_result = split(pmatrix, split_dim,&msg);
	if (msg != SP_AUX_SUCCESS){
		spLoggerPrintError(SPLIT_ERR,__FILE__,__func__,__LINE__);
		FREE_AND_NULL(node);
		DestroyMatrix(pmatrix, pmatrix->rows);
		return NULL;
	}

	left = split_result[0];
	right = split_result[1];
	// %T*$&$RYRETERTC**%^*$%^*$%*^
	DestroyMatrix(pmatrix, pmatrix->rows);

	node->left = create_tree(left, split_method, split_dim);

	node->right = create_tree(right, split_method, split_dim);

	return node;
}


//#################################################
//###################### KNN ######################
//#################################################

SPBPQueue init_bpqueue(int spknn){

	SPBPQueue queue = spBPQueueCreate(spknn);
	if (queue == NULL){
		spLoggerPrintError(CANNOT_CREATE_QUEUE,__FILE__,__func__,__LINE__);
		return NULL;
	}

	return queue;
}


void knn(PKDTreeNode node,SPBPQueue queue, SPPoint point){

	int node_idx, node_dim;
	SPPoint node_point;
	double dist, dist_from_val;
	SPListElement elem;
	bool goes_left = false;
	SP_BPQUEUE_MSG msg;


	if (node == NULL){
		return;
	}

	// node is not null - if it is a leaf insert into queue
	if (node->data != NULL){
		// node is a leaf
		node_point = node->data;
		dist = spPointL2SquaredDistance(point, node_point);
		node_idx = spPointGetIndex(node_point);
		elem = spListElementCreate(node_idx, dist);
		// if the distance is too big and the queue is full then the element will not be enqueued
		msg = spBPQueueEnqueue(queue,elem);
		if (msg != SP_BPQUEUE_SUCCESS){
			// elem not enqueued - destroy element
			spListElementDestroy(elem);
		}
		return;
	}
	/* Recursively search the half of the tree that contains the test point. */
	node_dim = node->dim;
	if (spPointGetAxisCoor(point,node_dim) <= node->val){
		// search the left subtree
		knn(node->left, queue, point);
		goes_left = true; // mark subtree we searched
	}
	else{
		knn(node->right, queue, point);
	}

	dist_from_val = node->val - spPointGetAxisCoor(point,node_dim);
	dist_from_val = dist_from_val * dist_from_val; // square distance to maintain spPointL2SquaredDistance
	// check if there is need to scan the other subtree
	if (!spBPQueueIsFull(queue) || dist_from_val < spBPQueueMaxValue(queue)){
		if (goes_left == true){
			// needs to go right
			knn(node->right, queue, point);
		}
		else{
			// needs to go left
			knn(node->left, queue, point);
		}
	}
}

int* get_nearest_images(PKDTreeNode root ,SPPoint* query_image_features, int arr_size,
		int num_of_images, int spknn, int num_of_similar_images){

	int *idx_result, elem_img_idx;
	ImageCounter *img_cnt;
	SPBPQueue queue;
	SPListElement elem;


	queue = init_bpqueue(spknn);
	if (queue == NULL){
		spLoggerPrintError(CANNOT_CREATE_QUEUE,__FILE__,__func__,__LINE__);
		return NULL;
	}

	img_cnt = (ImageCounter*)malloc(sizeof(*img_cnt) * num_of_images);
	if (img_cnt == NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG,__FILE__,__func__,__LINE__);
		spBPQueueDestroy(queue);
		return NULL;
	}

	// init array values - index number for each image and count to 0
	for (int i = 0; i < num_of_images; i++){
		img_cnt[i].img_idx = i;
		img_cnt[i].cnt = 0;
	}

	// for each point (feature of queried image) find spknn images and update img_cnt
	for (int i = 0; i < arr_size; i++){
		// queue is emptied so it's OK to reuse it
		knn(root, queue, query_image_features[i]); // make sure there is no need to replicate root before
		// queue is full with nearest images - empty it and update array
		while (!(spBPQueueIsEmpty(queue))){
			elem = spBPQueuePeek(queue); // get first element in the queue
			elem_img_idx = spListElementGetIndex(elem); // image index
			img_cnt[elem_img_idx].cnt++; // increase counter
			spBPQueueDequeue(queue);// to receive the next element in the queue
			spListElementDestroy(elem); // free allocation
		}
	}

	// sort img_cnt by appearance and return SPNumOfSimilarImages indices
	qsort(img_cnt, num_of_images, sizeof(ImageCounter), comapre_img_cnt_desc);

	// create the final array with indices of nearest images
	idx_result = (int*)malloc(sizeof(int) * num_of_similar_images);
	if (idx_result == NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG,__FILE__,__func__,__LINE__);
		spBPQueueDestroy(queue);
		FREE_AND_NULL(img_cnt);
		return NULL;
	}

	for (int i = 0; i < num_of_similar_images; i++){
		idx_result[i] = img_cnt[i].img_idx;
	}

	// free all allocations
	spBPQueueDestroy(queue);
	FREE_AND_NULL(img_cnt);

	return idx_result;

}

void destroyKDTree(PKDTreeNode node){

	if(node == NULL){
		return;
	}

	// recursively destroy each subtree
	destroyKDTree(node->left);
	destroyKDTree(node->right);

	// when getting to a leaf destroy the point elae do nothing
	if (node->data != NULL){
		spPointDestroy(node->data);
	}
	FREE_AND_NULL(node);
}

void destroyPointsArray(SPPoint *points, int size){

	for (int i = 0; i < size; i++){
		spPointDestroy(points[i]);
	}
	FREE_AND_NULL(points);
}
