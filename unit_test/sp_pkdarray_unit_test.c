#include "../SPConfig.h"
#include "../SPPoint.h"
#include "unit_test_util.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../main_aux.h"

#define EPS 0.001

bool d_equal(double a, double b, double epsilon){
	return fabs(a-b) < epsilon;
}


bool initTest(){
	SP_AUX_MSG msg = SP_AUX_INDEX_OUT_OF_RANGE;
	PKDArray arr,arr2;

	double d0[3] = {2.0,3.1,1.2};
	SPPoint p0 = spPointCreate(d0,3,0);

	double d1[3] = {1.8,0.1,8.2};
	SPPoint p1 = spPointCreate(d1,3,1);

	double d2[3] = {0.80,3.4,9.7};
	SPPoint p2 = spPointCreate(d2,3,0);

	SPPoint points_arr[3];
	points_arr[0] = p0;
	points_arr[1] = p1;
	points_arr[2] = p2;

	arr2 = init(points_arr,3,&msg);

	ASSERT_TRUE(msg == SP_AUX_SUCCESS);
	ASSERT_TRUE(arr2->size == 3);

	arr = init(points_arr,3,&msg);
	ASSERT_TRUE(msg == SP_AUX_SUCCESS);
	ASSERT_TRUE(arr->size == 3);

	// first index is picture number
	// second index is the index of the picture

	ASSERT_TRUE(arr->data[0].index == 0);
	ASSERT_TRUE(spPointGetIndex((arr->data[0].point)) == 0);
	ASSERT_TRUE(d_equal(spPointGetAxisCoor(arr->data[0].point,0),2.0,EPS));
	ASSERT_TRUE(d_equal(spPointGetAxisCoor(arr->data[0].point,1),3.1,EPS));
	ASSERT_TRUE(d_equal(spPointGetAxisCoor(arr->data[0].point,2),1.2,EPS));

	ASSERT_TRUE(arr->data[1].index == 1);
	ASSERT_TRUE(spPointGetIndex((arr->data[1].point)) == 1);
	ASSERT_TRUE(d_equal(spPointGetAxisCoor(arr->data[1].point,0),1.8,EPS));
	ASSERT_TRUE(d_equal(spPointGetAxisCoor(arr->data[1].point,1),0.1,EPS));
	ASSERT_TRUE(d_equal(spPointGetAxisCoor(arr->data[1].point,2),8.2,EPS));

	ASSERT_TRUE(arr->data[2].index == 2);
	ASSERT_TRUE(spPointGetIndex((arr->data[2].point)) == 0);
	ASSERT_TRUE(d_equal(spPointGetAxisCoor(arr->data[2].point,0),0.8,EPS));
	ASSERT_TRUE(d_equal(spPointGetAxisCoor(arr->data[2].point,1),3.4,EPS));
	ASSERT_TRUE(d_equal(spPointGetAxisCoor(arr->data[2].point,2),9.7,EPS));



	// second array tests
	ASSERT_TRUE(arr2->data[0].index == 0);
	ASSERT_TRUE(spPointGetIndex((arr2->data[0].point)) == 0);
	ASSERT_TRUE(d_equal(spPointGetAxisCoor(arr2->data[0].point,0),2.0,EPS));
	ASSERT_TRUE(d_equal(spPointGetAxisCoor(arr2->data[0].point,1),3.1,EPS));
	ASSERT_TRUE(d_equal(spPointGetAxisCoor(arr2->data[0].point,2),1.2,EPS));

	ASSERT_TRUE(arr2->data[1].index == 1);
	ASSERT_TRUE(spPointGetIndex((arr2->data[1].point)) == 1);
	ASSERT_TRUE(d_equal(spPointGetAxisCoor(arr2->data[1].point,0),1.8,EPS));
	ASSERT_TRUE(d_equal(spPointGetAxisCoor(arr2->data[1].point,1),0.1,EPS));
	ASSERT_TRUE(d_equal(spPointGetAxisCoor(arr2->data[1].point,2),8.2,EPS));

	ASSERT_TRUE(arr2->data[2].index == 2);
	ASSERT_TRUE(spPointGetIndex((arr2->data[2].point)) == 0);
	ASSERT_TRUE(d_equal(spPointGetAxisCoor(arr2->data[2].point,0),0.8,EPS));
	ASSERT_TRUE(d_equal(spPointGetAxisCoor(arr2->data[2].point,1),3.4,EPS));
	ASSERT_TRUE(d_equal(spPointGetAxisCoor(arr2->data[2].point,2),9.7,EPS));

	DestroyKDArray(arr2);
	DestroyKDArray(arr);
	spPointDestroy(p0);
	spPointDestroy(p1);
	spPointDestroy(p2);
	// now check for empty points array

	init(NULL, 2, &msg);
	ASSERT_TRUE(msg == SP_AUX_INVALID_ARGUMENT);

return true;

}



bool copyArrayTest(){
	SP_AUX_MSG msg = SP_AUX_INDEX_OUT_OF_RANGE;
	PKDArray arr, arr1, arr2, arr3;

	double d0[3] = {2.0,3.1,1.2};
	SPPoint p0 = spPointCreate(d0,3,0);

	double d1[3] = {1.8,0.1,8.2};
	SPPoint p1 = spPointCreate(d1,3,1);

	double d2[3] = {0.80,3.4,9.7};
	SPPoint p2 = spPointCreate(d2,3,0);

	SPPoint points_arr[3];
	points_arr[0] = p0;
	points_arr[1] = p1;
	points_arr[2] = p2;

	arr = init(points_arr,3,&msg);
	ASSERT_TRUE(msg == SP_AUX_SUCCESS);
	ASSERT_TRUE(arr->size == 3);

	// copy all array
	arr1 = copyArray(arr, 0, arr->size, &msg);

	ASSERT_TRUE(msg == SP_AUX_SUCCESS);
	ASSERT_TRUE(arr1->size == 3);

	// first index is picture number
	// second index is the index of the picture

	ASSERT_TRUE(arr1->data[0].index == 0);
	ASSERT_TRUE(spPointGetIndex((arr1->data[0].point)) == 0);
	ASSERT_TRUE(d_equal(spPointGetAxisCoor(arr1->data[0].point,0),2.0,EPS));
	ASSERT_TRUE(d_equal(spPointGetAxisCoor(arr1->data[0].point,1),3.1,EPS));
	ASSERT_TRUE(d_equal(spPointGetAxisCoor(arr1->data[0].point,2),1.2,EPS));

	ASSERT_TRUE(arr1->data[1].index == 1);
	ASSERT_TRUE(spPointGetIndex((arr1->data[1].point)) == 1);
	ASSERT_TRUE(d_equal(spPointGetAxisCoor(arr1->data[1].point,0),1.8,EPS));
	ASSERT_TRUE(d_equal(spPointGetAxisCoor(arr1->data[1].point,1),0.1,EPS));
	ASSERT_TRUE(d_equal(spPointGetAxisCoor(arr1->data[1].point,2),8.2,EPS));

	ASSERT_TRUE(arr1->data[2].index == 2);
	ASSERT_TRUE(spPointGetIndex((arr1->data[2].point)) == 0);
	ASSERT_TRUE(d_equal(spPointGetAxisCoor(arr1->data[2].point,0),0.8,EPS));
	ASSERT_TRUE(d_equal(spPointGetAxisCoor(arr1->data[2].point,1),3.4,EPS));
	ASSERT_TRUE(d_equal(spPointGetAxisCoor(arr1->data[2].point,2),9.7,EPS));


	// copy from start
	arr2 = copyArray(arr, 0, 2, &msg);

	ASSERT_TRUE(msg == SP_AUX_SUCCESS);
	ASSERT_TRUE(arr2->size == 2);

	ASSERT_TRUE(arr2->data[0].index == 0);
	ASSERT_TRUE(spPointGetIndex((arr2->data[0].point)) == 0);
	ASSERT_TRUE(d_equal(spPointGetAxisCoor(arr2->data[0].point,0),2.0,EPS));
	ASSERT_TRUE(d_equal(spPointGetAxisCoor(arr2->data[0].point,1),3.1,EPS));
	ASSERT_TRUE(d_equal(spPointGetAxisCoor(arr2->data[0].point,2),1.2,EPS));

	ASSERT_TRUE(arr2->data[1].index == 1);
	ASSERT_TRUE(spPointGetIndex((arr2->data[1].point)) == 1);
	ASSERT_TRUE(d_equal(spPointGetAxisCoor(arr2->data[1].point,0),1.8,EPS));
	ASSERT_TRUE(d_equal(spPointGetAxisCoor(arr2->data[1].point,1),0.1,EPS));
	ASSERT_TRUE(d_equal(spPointGetAxisCoor(arr2->data[1].point,2),8.2,EPS));


	// copy end
	arr3 = copyArray(arr, 1, 3, &msg);

	ASSERT_TRUE(msg == SP_AUX_SUCCESS);
	ASSERT_TRUE(arr3->size == 2);

	ASSERT_TRUE(arr3->data[0].index == 1);
	ASSERT_TRUE(spPointGetIndex((arr3->data[0].point)) == 1);
	ASSERT_TRUE(d_equal(spPointGetAxisCoor(arr3->data[0].point,0),1.8,EPS));
	ASSERT_TRUE(d_equal(spPointGetAxisCoor(arr3->data[0].point,1),0.1,EPS));
	ASSERT_TRUE(d_equal(spPointGetAxisCoor(arr3->data[0].point,2),8.2,EPS));

	ASSERT_TRUE(arr3->data[1].index == 2);
	ASSERT_TRUE(spPointGetIndex((arr3->data[1].point)) == 0);
	ASSERT_TRUE(d_equal(spPointGetAxisCoor(arr3->data[1].point,0),0.8,EPS));
	ASSERT_TRUE(d_equal(spPointGetAxisCoor(arr3->data[1].point,1),3.4,EPS));
	ASSERT_TRUE(d_equal(spPointGetAxisCoor(arr3->data[1].point,2),9.7,EPS));


	// free all resources
	DestroyKDArray(arr);
	DestroyKDArray(arr1);
	DestroyKDArray(arr2);
	DestroyKDArray(arr3);
	spPointDestroy(p0);
	spPointDestroy(p1);
	spPointDestroy(p2);

	return true;
}


bool createMatrixTest(){
	SP_AUX_MSG msg = SP_AUX_INDEX_OUT_OF_RANGE;
	PKDArray arr;

	double d0[3] = {2.0,3.1,1.2};
	SPPoint p0 = spPointCreate(d0,3,0);

	double d1[3] = {1.8,0.1,8.2};
	SPPoint p1 = spPointCreate(d1,3,1);

	double d2[3] = {0.80,3.4,9.7};
	SPPoint p2 = spPointCreate(d2,3,0);

	double d3[3] = {0.4,3.5,11.7};
	SPPoint p3 = spPointCreate(d3,3,0);

	SPPoint points_arr[4];
	points_arr[0] = p0;
	points_arr[1] = p1;
	points_arr[2] = p2;
	points_arr[3] = p3;


	arr = init(points_arr,4,&msg);

	PMatrix mat = createMatrix(arr, 3, &msg);

	ASSERT_TRUE(msg == SP_AUX_SUCCESS);
	ASSERT_TRUE(mat->cols == 4);
	ASSERT_TRUE(mat->rows == 3);

	ASSERT_TRUE(mat->matrix[0][0] == 3);
	ASSERT_TRUE(mat->matrix[0][1] == 2);
	ASSERT_TRUE(mat->matrix[0][2] == 1);
	ASSERT_TRUE(mat->matrix[0][3] == 0);


	ASSERT_TRUE(mat->matrix[1][0] == 1);
	ASSERT_TRUE(mat->matrix[1][1] == 0);
	ASSERT_TRUE(mat->matrix[1][2] == 2);
	ASSERT_TRUE(mat->matrix[1][3] == 3);


	ASSERT_TRUE(mat->matrix[2][0] == 0);
	ASSERT_TRUE(mat->matrix[2][1] == 1);
	ASSERT_TRUE(mat->matrix[2][2] == 2);
	ASSERT_TRUE(mat->matrix[2][3] == 3);

	DestroyMatrix(mat,3);
	spPointDestroy(p0);
	spPointDestroy(p1);
	spPointDestroy(p2);
	spPointDestroy(p3);
	DestroyKDArray(arr);

	return true;

}

bool createSplitMapTest(){
	SP_AUX_MSG msg = SP_AUX_INDEX_OUT_OF_RANGE;
	PKDArray arr;
	int *map0, *map1,*map2;

	double d0[3] = {2.0,3.1,1.2};
	SPPoint p0 = spPointCreate(d0,3,0);

	double d1[3] = {1.8,0.1,8.2};
	SPPoint p1 = spPointCreate(d1,3,1);

	double d2[3] = {0.80,3.4,9.7};
	SPPoint p2 = spPointCreate(d2,3,0);

	double d3[3] = {0.4,2.9,11.7};
	SPPoint p3 = spPointCreate(d3,3,0);

	SPPoint points_arr[4];
	points_arr[0] = p0;
	points_arr[1] = p1;
	points_arr[2] = p2;
	points_arr[3] = p3;


	arr = init(points_arr,4,&msg);

	PMatrix mat = createMatrix(arr, 3, &msg);

	ASSERT_TRUE(msg == SP_AUX_SUCCESS);
	ASSERT_TRUE(mat->cols == 4);
	ASSERT_TRUE(mat->rows == 3);

	map0 = createSplitMap(mat, 0 ,2, &msg);
	ASSERT_TRUE(msg == SP_AUX_SUCCESS);

	ASSERT_TRUE(map0[0] == 1);
	ASSERT_TRUE(map0[1] == 1);
	ASSERT_TRUE(map0[2] == 0);
	ASSERT_TRUE(map0[3] == 0);

	map1 = createSplitMap(mat, 1 ,2, &msg);
	ASSERT_TRUE(msg == SP_AUX_SUCCESS);

	ASSERT_TRUE(map1[0] == 1);
	ASSERT_TRUE(map1[1] == 0);
	ASSERT_TRUE(map1[2] == 1);
	ASSERT_TRUE(map1[3] == 0);


	map2 = createSplitMap(mat, 2 ,2, &msg);
	ASSERT_TRUE(msg == SP_AUX_SUCCESS);

	ASSERT_TRUE(map2[0] == 0);
	ASSERT_TRUE(map2[1] == 0);
	ASSERT_TRUE(map2[2] == 1);
	ASSERT_TRUE(map2[3] == 1);


	free(map0);
	free(map1);
	free(map2);
	DestroyMatrix(mat,3);
	spPointDestroy(p0);
	spPointDestroy(p1);
	spPointDestroy(p2);
	spPointDestroy(p3);
	DestroyKDArray(arr);

	return true;
}


bool createNewIdxTest(){
	SP_AUX_MSG msg = SP_AUX_INDEX_OUT_OF_RANGE;
	PKDArray arr;
	int *map0, *map1;
	int *new0, *new1, *new2, *new3;

	double d0[3] = {2.0,3.1,1.2};
	SPPoint p0 = spPointCreate(d0,3,0);

	double d1[3] = {1.8,0.1,8.2};
	SPPoint p1 = spPointCreate(d1,3,1);

	double d2[3] = {0.80,3.4,9.7};
	SPPoint p2 = spPointCreate(d2,3,0);

	double d3[3] = {0.4,2.9,11.7};
	SPPoint p3 = spPointCreate(d3,3,0);

	SPPoint points_arr[4];
	points_arr[0] = p0;
	points_arr[1] = p1;
	points_arr[2] = p2;
	points_arr[3] = p3;


	arr = init(points_arr,4,&msg);

	PMatrix mat = createMatrix(arr, 3, &msg);

	map0 = createSplitMap(mat, 0 ,2, &msg);
	ASSERT_TRUE(msg == SP_AUX_SUCCESS);

	new0 = createNewIdx(mat,map0,0,&msg);
	ASSERT_TRUE(msg == SP_AUX_SUCCESS);
	ASSERT_TRUE(new0[0] == -1);
	ASSERT_TRUE(new0[1] == -1);
	ASSERT_TRUE(new0[2] == 0);
	ASSERT_TRUE(new0[3] == 1);

	new1 = createNewIdx(mat,map0,1,&msg);
	ASSERT_TRUE(msg == SP_AUX_SUCCESS);
	ASSERT_TRUE(new1[0] == 0);
	ASSERT_TRUE(new1[1] == 1);
	ASSERT_TRUE(new1[2] == -1);
	ASSERT_TRUE(new1[3] == -1);

	map1 = createSplitMap(mat, 1 ,2, &msg);
	ASSERT_TRUE(msg == SP_AUX_SUCCESS);

	new2 = createNewIdx(mat,map1,0,&msg);
	ASSERT_TRUE(msg == SP_AUX_SUCCESS);
	ASSERT_TRUE(new2[0] == -1);
	ASSERT_TRUE(new2[1] == 0);
	ASSERT_TRUE(new2[2] == -1);
	ASSERT_TRUE(new2[3] == 1);

	new3 = createNewIdx(mat,map1,1,&msg);
	ASSERT_TRUE(msg == SP_AUX_SUCCESS);
	ASSERT_TRUE(new3[0] == 0);
	ASSERT_TRUE(new3[1] == -1);
	ASSERT_TRUE(new3[2] == 1);
	ASSERT_TRUE(new3[3] == -1);


	free(map0);
	free(map1);
	free(new0);
	free(new1);
	free(new2);
	free(new3);
	DestroyMatrix(mat,3);
	DestroyKDArray(arr);
	spPointDestroy(p0);
	spPointDestroy(p1);
	spPointDestroy(p2);
	spPointDestroy(p3);

	return true;
}

bool splitTest(){
	SP_AUX_MSG msg = SP_AUX_INDEX_OUT_OF_RANGE;
	PKDArray arr;
	PMatrix* split_res;
	PMatrix left,right;

	double d0[3] = {1,2};
	SPPoint p0 = spPointCreate(d0,2,0); // a in Moab example

	double d1[3] = {123,70};
	SPPoint p1 = spPointCreate(d1,2,1); // b in Moab example

	double d2[3] = {2,7};
	SPPoint p2 = spPointCreate(d2,2,0); // c in Moab example

	double d3[3] = {9,11};
	SPPoint p3 = spPointCreate(d3,2,0); // d in Moab example

	double d4[3] = {3,4};
	SPPoint p4 = spPointCreate(d4,2,0); // e in Moab example

	SPPoint points_arr[5];
	points_arr[0] = p0;
	points_arr[1] = p1;
	points_arr[2] = p2;
	points_arr[3] = p3;
	points_arr[4] = p4;


	arr = init(points_arr,5,&msg);
	ASSERT_TRUE(msg == SP_AUX_SUCCESS);

	PMatrix mat = createMatrix(arr, 2, &msg);
	ASSERT_TRUE(msg == SP_AUX_SUCCESS);

	// split according the first coordinate
	split_res = split(mat,0,&msg);
	ASSERT_TRUE(msg == SP_AUX_SUCCESS);

	left = split_res[0];

	ASSERT_TRUE(left->cols == 3);
	ASSERT_TRUE(left->rows == 2);
	// check a is the same
	ASSERT_TRUE(spPointGetAxisCoor(left->kd_array->data[0].point,0) == 1);
	ASSERT_TRUE(spPointGetAxisCoor(left->kd_array->data[0].point,1) == 2);

	// check c is the same
	ASSERT_TRUE(spPointGetAxisCoor(left->kd_array->data[1].point,0) == 2);
	ASSERT_TRUE(spPointGetAxisCoor(left->kd_array->data[1].point,1) == 7);

	// check e is the same
	ASSERT_TRUE(spPointGetAxisCoor(left->kd_array->data[2].point,0) == 3);
	ASSERT_TRUE(spPointGetAxisCoor(left->kd_array->data[2].point,1) == 4);

	ASSERT_TRUE(left->matrix[0][0] == 0);
	ASSERT_TRUE(left->matrix[0][1] == 1);
	ASSERT_TRUE(left->matrix[0][2] == 2);
	ASSERT_TRUE(left->matrix[1][0] == 0);
	ASSERT_TRUE(left->matrix[1][1] == 2);
	ASSERT_TRUE(left->matrix[1][2] == 1);


	right = split_res[1];

	ASSERT_TRUE(right->cols == 2);
	ASSERT_TRUE(right->rows == 2);
	// check b is the same
	ASSERT_TRUE(spPointGetAxisCoor(right->kd_array->data[0].point,0) == 123);
	ASSERT_TRUE(spPointGetAxisCoor(right->kd_array->data[0].point,1) == 70);

	// check d is the same
	ASSERT_TRUE(spPointGetAxisCoor(right->kd_array->data[1].point,0) == 9);
	ASSERT_TRUE(spPointGetAxisCoor(right->kd_array->data[1].point,1) == 11);


	ASSERT_TRUE(right->matrix[0][0] == 1);
	ASSERT_TRUE(right->matrix[0][1] == 0);
	ASSERT_TRUE(right->matrix[1][0] == 1);
	ASSERT_TRUE(right->matrix[1][1] == 0);


	DestroyMatrix(right, 2);
	DestroyMatrix(left, 2);
	free(split_res);



	// split according the second coordinate
	split_res = split(mat,1,&msg);
	ASSERT_TRUE(msg == SP_AUX_SUCCESS);

	left = split_res[0];

	ASSERT_TRUE(left->cols == 3);
	ASSERT_TRUE(left->rows == 2);

	// check a is the same
	ASSERT_TRUE(spPointGetAxisCoor(left->kd_array->data[0].point,0) == 1);
	ASSERT_TRUE(spPointGetAxisCoor(left->kd_array->data[0].point,1) == 2);

	// check e is the same
	ASSERT_TRUE(spPointGetAxisCoor(left->kd_array->data[1].point,0) == 2);
	ASSERT_TRUE(spPointGetAxisCoor(left->kd_array->data[1].point,1) == 7);

	// check c is the same
	ASSERT_TRUE(spPointGetAxisCoor(left->kd_array->data[2].point,0) == 3);
	ASSERT_TRUE(spPointGetAxisCoor(left->kd_array->data[2].point,1) == 4);

	ASSERT_TRUE(left->matrix[0][0] == 0);
	ASSERT_TRUE(left->matrix[0][1] == 1);
	ASSERT_TRUE(left->matrix[0][2] == 2);
	ASSERT_TRUE(left->matrix[1][0] == 0);
	ASSERT_TRUE(left->matrix[1][1] == 2);
	ASSERT_TRUE(left->matrix[1][2] == 1);


	right = split_res[1];

	ASSERT_TRUE(right->cols == 2);
	ASSERT_TRUE(right->rows == 2);
	// check b is the same
	ASSERT_TRUE(spPointGetAxisCoor(right->kd_array->data[0].point,0) == 123);
	ASSERT_TRUE(spPointGetAxisCoor(right->kd_array->data[0].point,1) == 70);

	// check d is the same
	ASSERT_TRUE(spPointGetAxisCoor(right->kd_array->data[1].point,0) == 9);
	ASSERT_TRUE(spPointGetAxisCoor(right->kd_array->data[1].point,1) == 11);


	ASSERT_TRUE(right->matrix[0][0] == 1);
	ASSERT_TRUE(right->matrix[0][1] == 0);
	ASSERT_TRUE(right->matrix[1][0] == 1);
	ASSERT_TRUE(right->matrix[1][1] == 0);


	DestroyMatrix(right, 2);
	DestroyMatrix(left, 2);
	DestroyKDArray(arr);
	DestroyMatrix(mat, 2);
	spPointDestroy(p0);
	spPointDestroy(p1);
	spPointDestroy(p2);
	spPointDestroy(p2);
	spPointDestroy(p4);
	free(split_res);

	return true;
}
/*
int main(){
	RUN_TEST(initTest);
	RUN_TEST(copyArrayTest);
	RUN_TEST(createMatrixTest);
	RUN_TEST(createSplitMapTest);
	RUN_TEST(createNewIdxTest);
	RUN_TEST(splitTest);
}
*/

