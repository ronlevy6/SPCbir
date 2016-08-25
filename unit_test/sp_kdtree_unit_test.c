#include "../SPConfig.h"
#include "../SPPoint.h"
#include "unit_test_util.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include "../main.cpp"


#include "../main_aux.h"


#define EPS 0.001
#define MAX_VAL 10000000

bool db_equal(double a, double b, double epsilon){
	return fabs(a-b) < epsilon;
}





bool get_split_dim_test(){

	SP_AUX_MSG msg = SP_AUX_INDEX_OUT_OF_RANGE;
	SP_KDTREE_SPLIT_METHOD method;
	int res_dim;

	PKDArray arr;

	double d0[3] = {2.0,3.1,1.2};
	SPPoint p0 = spPointCreate(d0,3,0);

	double d1[3] = {1.8,0.1,8.2};
	SPPoint p1 = spPointCreate(d1,3,1);

	double d2[3] = {0.80,3.4,9.7};
	SPPoint p2 = spPointCreate(d2,3,2);

	double d3[3] = {0.4,3.5,11.7};
	SPPoint p3 = spPointCreate(d3,3,3);

	SPPoint points_arr[4];
	points_arr[0] = p0;
	points_arr[1] = p1;
	points_arr[2] = p2;
	points_arr[3] = p3;


	arr = init(points_arr,4,&msg);

	PMatrix mat = createMatrix(arr, 3, &msg);

	// check incremental mode
	method = INCREMENTAL;
	res_dim = get_split_dim(mat,1,method);
	ASSERT_TRUE(res_dim == 2);

	res_dim = get_split_dim(mat,2,method);
	ASSERT_TRUE(res_dim == 0);// according to mod (%) calculation

	// check max spread mode
	method = MAX_SPREAD;
	res_dim = get_split_dim(mat,1,method);
	ASSERT_TRUE(res_dim == 2);

	res_dim = get_split_dim(mat,2,method);
	ASSERT_TRUE(res_dim == 2);// sanity check- should return 2 no matter what the dim is

	// check random mode (not the returned value but to make sure it runs properly)
	method = RANDOM;
	res_dim = get_split_dim(mat,1,method);
	ASSERT_TRUE(res_dim == 2 || res_dim == 1 || res_dim == 0);

	res_dim = get_split_dim(mat,2,method);
	ASSERT_TRUE(res_dim == 2 || res_dim == 1 || res_dim == 0);

	spPointDestroy(p0);
	spPointDestroy(p1);
	spPointDestroy(p2);
	spPointDestroy(p3);
	DestroyMatrix(mat, 3);
	DestroyKDArray(arr);


	return true;
}


bool get_median_value_test(){
	SP_AUX_MSG msg = SP_AUX_INDEX_OUT_OF_RANGE;
	double result;

	PKDArray arr;

	double d0[3] = {2.0,3.1,1.2};
	SPPoint p0 = spPointCreate(d0,3,0);

	double d1[3] = {1.8,0.1,8.2};
	SPPoint p1 = spPointCreate(d1,3,1);

	double d2[3] = {0.80,3.4,9.7};
	SPPoint p2 = spPointCreate(d2,3,2);

	double d3[3] = {0.4,3.5,11.7};
	SPPoint p3 = spPointCreate(d3,3,3);

	SPPoint points_arr[4];
	points_arr[0] = p0;
	points_arr[1] = p1;
	points_arr[2] = p2;
	points_arr[3] = p3;


	arr = init(points_arr,4,&msg);

	PMatrix mat = createMatrix(arr, 3, &msg);

	result = get_median_value(mat, 0);
	ASSERT_TRUE(db_equal(result, 1.8, EPS));

	result = get_median_value(mat, 1);
	ASSERT_TRUE(db_equal(result, 3.4, EPS));

	result = get_median_value(mat, 2);
	ASSERT_TRUE(db_equal(result, 9.7, EPS));

	spPointDestroy(p0);
	spPointDestroy(p1);
	spPointDestroy(p2);
	spPointDestroy(p3);
	DestroyMatrix(mat, 3);
	DestroyKDArray(arr);

	return true;
}

bool create_node_test(){
	PKDTreeNode node;
	double median;
	int split_dim;

	split_dim = 4;
	median = 5.2;
	node = create_node(split_dim, median);
	ASSERT_TRUE(node->data == NULL);
	ASSERT_TRUE(node->dim == 4);
	ASSERT_TRUE(node->left == NULL);
	ASSERT_TRUE(node->right == NULL);
	ASSERT_TRUE(db_equal(node->val, 5.2, EPS));

	free(node);

	split_dim = 100;
	median = 145.987;
	node = create_node(split_dim, median);
	ASSERT_TRUE(node->data == NULL);
	ASSERT_TRUE(node->dim == 100);
	ASSERT_TRUE(node->left == NULL);
	ASSERT_TRUE(node->right == NULL);
	ASSERT_TRUE(db_equal(node->val, 145.987, EPS));

	free(node);

	return true;

}

bool create_leaf_test(){
	PKDTreeNode node;
	SPPoint point;

	double d0[3] = {2.0,3.1,1.2};
	point = spPointCreate(d0,3,0);

	node = create_leaf(point);

	ASSERT_TRUE(db_equal(spPointGetAxisCoor(node->data,0),2.0, EPS));
	ASSERT_TRUE(db_equal(spPointGetAxisCoor(node->data,1),3.1, EPS));
	ASSERT_TRUE(db_equal(spPointGetAxisCoor(node->data,2),1.2, EPS));
	ASSERT_TRUE(spPointGetIndex(node->data) == 0);
	/*
	 dim and val are set to inf - how can we check it?
	ASSERT_TRUE(node->dim > 10000);
	ASSERT_TRUE(node->val > 10000);
	*/
	ASSERT_TRUE(node->left == NULL);
	ASSERT_TRUE(node->right == NULL);

	spPointDestroy(point);
	spPointDestroy(node->data);
	free(node);

	return true;
}



bool create_tree_test(){
	SP_AUX_MSG msg = SP_AUX_INDEX_OUT_OF_RANGE;
	SP_KDTREE_SPLIT_METHOD method;
	PKDTreeNode root, left_node, right_node;


	PKDArray arr;

	double d0[3] = {2.0,3.1,1.2};
	SPPoint p0 = spPointCreate(d0,3,0);

	double d1[3] = {1.8,0.1,8.2};
	SPPoint p1 = spPointCreate(d1,3,1);

	double d2[3] = {0.80,3.4,9.7};
	SPPoint p2 = spPointCreate(d2,3,2);

	double d3[3] = {0.4,3.5,11.7};
	SPPoint p3 = spPointCreate(d3,3,3);

	SPPoint points_arr[4];
	points_arr[0] = p0;
	points_arr[1] = p1;
	points_arr[2] = p2;
	points_arr[3] = p3;


	arr = init(points_arr,4,&msg);

	PMatrix mat = createMatrix(arr, 3, &msg);

	method = INCREMENTAL;
	root = create_tree(mat, method, -1);// incremental mode start with a zero

	// walk on the tree and check it nodes.
	ASSERT_TRUE(root->data == NULL);
	ASSERT_TRUE(root->dim == 0);
	ASSERT_TRUE(root->left != NULL);
	ASSERT_TRUE(root->right != NULL);
	ASSERT_TRUE(db_equal(root->val, 1.8, EPS));

	// go to left child
	left_node = root->left;
	ASSERT_TRUE(left_node->data == NULL);
	ASSERT_TRUE(left_node->dim == 1);
	ASSERT_TRUE(left_node->left != NULL);
	ASSERT_TRUE(left_node->right != NULL);
	ASSERT_TRUE(db_equal(left_node->val, 3.5, EPS));

	// go to root's right child
	right_node = root->right;
	ASSERT_TRUE(right_node->data == NULL);
	ASSERT_TRUE(right_node->dim == 1);
	ASSERT_TRUE(right_node->left != NULL);
	ASSERT_TRUE(right_node->right != NULL);
	ASSERT_TRUE(db_equal(right_node->val, 3.1, EPS));


	// go to leafs // consider adding the dim and val testing
	ASSERT_TRUE(db_equal(spPointGetAxisCoor(left_node->left->data, 0), 0.8, EPS));
	ASSERT_TRUE(db_equal(spPointGetAxisCoor(left_node->left->data, 1), 3.4, EPS));
	ASSERT_TRUE(db_equal(spPointGetAxisCoor(left_node->left->data, 2), 9.7, EPS));
	ASSERT_TRUE(left_node->left->left == NULL);
	ASSERT_TRUE(left_node->left->right == NULL);


	ASSERT_TRUE(db_equal(spPointGetAxisCoor(left_node->right->data, 0), 0.4, EPS));
	ASSERT_TRUE(db_equal(spPointGetAxisCoor(left_node->right->data, 1), 3.5, EPS));
	ASSERT_TRUE(db_equal(spPointGetAxisCoor(left_node->right->data, 2), 11.7, EPS));
	ASSERT_TRUE(left_node->right->left == NULL);
	ASSERT_TRUE(left_node->right->right == NULL);


	ASSERT_TRUE(db_equal(spPointGetAxisCoor(right_node->left->data, 0), 1.8, EPS));
	ASSERT_TRUE(db_equal(spPointGetAxisCoor(right_node->left->data, 1), 0.1, EPS));
	ASSERT_TRUE(db_equal(spPointGetAxisCoor(right_node->left->data, 2), 8.2, EPS));
	ASSERT_TRUE(right_node->left->left == NULL);
	ASSERT_TRUE(right_node->left->right == NULL);


	ASSERT_TRUE(db_equal(spPointGetAxisCoor(right_node->right->data, 0), 2.0, EPS));
	ASSERT_TRUE(db_equal(spPointGetAxisCoor(right_node->right->data, 1), 3.1, EPS));
	ASSERT_TRUE(db_equal(spPointGetAxisCoor(right_node->right->data, 2), 1.2, EPS));
	ASSERT_TRUE(right_node->right->left == NULL);
	ASSERT_TRUE(right_node->right->right == NULL);

	destroyKDTree(root);



	// method = max spread
	mat = createMatrix(arr, 3, &msg);
	method = MAX_SPREAD;
	root = create_tree(mat, method, -1);// incremental mode start with a zero

	// walk on the tree and check it nodes.
	ASSERT_TRUE(root->data == NULL);
	ASSERT_TRUE(root->dim == 2);
	ASSERT_TRUE(root->left != NULL);
	ASSERT_TRUE(root->right != NULL);
	ASSERT_TRUE(db_equal(root->val, 9.7, EPS));

	// go to left child
	left_node = root->left;
	ASSERT_TRUE(left_node->data == NULL);
	ASSERT_TRUE(left_node->dim == 2);
	ASSERT_TRUE(left_node->left != NULL);
	ASSERT_TRUE(left_node->right != NULL);
	ASSERT_TRUE(db_equal(left_node->val, 8.2, EPS));

	// go to root's right child
	right_node = root->right;
	ASSERT_TRUE(right_node->data == NULL);
	ASSERT_TRUE(right_node->dim == 2);
	ASSERT_TRUE(right_node->left != NULL);
	ASSERT_TRUE(right_node->right != NULL);
	ASSERT_TRUE(db_equal(right_node->val, 11.7, EPS));


	// go to leafs // consider adding the dim and val testing
	ASSERT_TRUE(db_equal(spPointGetAxisCoor(left_node->left->data, 0), 2.0, EPS));
	ASSERT_TRUE(db_equal(spPointGetAxisCoor(left_node->left->data, 1), 3.1, EPS));
	ASSERT_TRUE(db_equal(spPointGetAxisCoor(left_node->left->data, 2), 1.2, EPS));
	ASSERT_TRUE(left_node->left->left == NULL);
	ASSERT_TRUE(left_node->left->right == NULL);


	ASSERT_TRUE(db_equal(spPointGetAxisCoor(left_node->right->data, 0), 1.8, EPS));
	ASSERT_TRUE(db_equal(spPointGetAxisCoor(left_node->right->data, 1), 0.1, EPS));
	ASSERT_TRUE(db_equal(spPointGetAxisCoor(left_node->right->data, 2), 8.2, EPS));
	ASSERT_TRUE(left_node->right->left == NULL);
	ASSERT_TRUE(left_node->right->right == NULL);


	ASSERT_TRUE(db_equal(spPointGetAxisCoor(right_node->left->data, 0), 0.80, EPS));
	ASSERT_TRUE(db_equal(spPointGetAxisCoor(right_node->left->data, 1), 3.4, EPS));
	ASSERT_TRUE(db_equal(spPointGetAxisCoor(right_node->left->data, 2), 9.7, EPS));
	ASSERT_TRUE(right_node->left->left == NULL);
	ASSERT_TRUE(right_node->left->right == NULL);


	ASSERT_TRUE(db_equal(spPointGetAxisCoor(right_node->right->data, 0), 0.4, EPS));
	ASSERT_TRUE(db_equal(spPointGetAxisCoor(right_node->right->data, 1), 3.5, EPS));
	ASSERT_TRUE(db_equal(spPointGetAxisCoor(right_node->right->data, 2), 11.7, EPS));
	ASSERT_TRUE(right_node->right->left == NULL);
	ASSERT_TRUE(right_node->right->right == NULL);

	destroyKDTree(root);

	// check that random works fine

	mat = createMatrix(arr, 3, &msg);
	method = RANDOM;
	root = create_tree(mat, method, -1);

	destroyKDTree(root);
	DestroyKDArray(arr);
	//DestroyMatrix(mat,3);
	spPointDestroy(p0);
	spPointDestroy(p1);
	spPointDestroy(p2);
	spPointDestroy(p3);

	return true;
}

bool create_tree_test2(){
	SP_AUX_MSG msg = SP_AUX_INDEX_OUT_OF_RANGE;
	SP_KDTREE_SPLIT_METHOD method;
	PKDTreeNode root, left_node;


	PKDArray arr;

	double d0[3] = {2.0,3.1,1.2};
	SPPoint p0 = spPointCreate(d0,3,0);

	double d1[3] = {1.8,0.1,8.2};
	SPPoint p1 = spPointCreate(d1,3,1);

	double d2[3] = {0.80,3.4,9.7};
	SPPoint p2 = spPointCreate(d2,3,2);


	SPPoint points_arr[3];
	points_arr[0] = p0;
	points_arr[1] = p1;
	points_arr[2] = p2;


	arr = init(points_arr,3,&msg);

	PMatrix mat = createMatrix(arr, 3, &msg);

	method = MAX_SPREAD;
	root = create_tree(mat, method, -1);

	// walk on the tree and check it nodes.
	ASSERT_TRUE(root->data == NULL);
	ASSERT_TRUE(root->dim == 2);
	ASSERT_TRUE(root->left != NULL);
	ASSERT_TRUE(root->right != NULL);
	ASSERT_TRUE(db_equal(root->val, 8.2, EPS));

	// go to root's left child
	left_node = root->left;
	ASSERT_TRUE(left_node->data == NULL);
	ASSERT_TRUE(left_node->dim == 2);
	ASSERT_TRUE(left_node->left != NULL);
	ASSERT_TRUE(left_node->right != NULL);
	ASSERT_TRUE(db_equal(left_node->val, 8.2, EPS));

	// go to leafs, right node is a leaf
	ASSERT_TRUE(db_equal(spPointGetAxisCoor(left_node->left->data, 0), 2.0, EPS));
	ASSERT_TRUE(db_equal(spPointGetAxisCoor(left_node->left->data, 1), 3.1, EPS));
	ASSERT_TRUE(db_equal(spPointGetAxisCoor(left_node->left->data, 2), 1.2, EPS));
	ASSERT_TRUE(left_node->left->left == NULL);
	ASSERT_TRUE(left_node->left->right == NULL);


	ASSERT_TRUE(db_equal(spPointGetAxisCoor(left_node->right->data, 0), 1.8, EPS));
	ASSERT_TRUE(db_equal(spPointGetAxisCoor(left_node->right->data, 1), 0.1, EPS));
	ASSERT_TRUE(db_equal(spPointGetAxisCoor(left_node->right->data, 2), 8.2, EPS));
	ASSERT_TRUE(left_node->right->left == NULL);
	ASSERT_TRUE(left_node->right->right == NULL);


	ASSERT_TRUE(db_equal(spPointGetAxisCoor(root->right->data, 0), 0.80, EPS));
	ASSERT_TRUE(db_equal(spPointGetAxisCoor(root->right->data, 1), 3.4, EPS));
	ASSERT_TRUE(db_equal(spPointGetAxisCoor(root->right->data, 2), 9.7, EPS));
	ASSERT_TRUE(root->right->left == NULL);
	ASSERT_TRUE(root->right->right == NULL);


	destroyKDTree(root);
	DestroyKDArray(arr);
	//DestroyMatrix(mat,3);
	spPointDestroy(p0);
	spPointDestroy(p1);
	spPointDestroy(p2);

	return true;
}


bool create_tree_test3(){
	SP_AUX_MSG msg = SP_AUX_INDEX_OUT_OF_RANGE;
	SP_KDTREE_SPLIT_METHOD method;
	PKDTreeNode root;


	PKDArray arr;

	double d0[3] = {2.0,3.1,1.2};
	SPPoint p0 = spPointCreate(d0,3,0);

	SPPoint points_arr[1];
	points_arr[0] = p0;


	arr = init(points_arr,1,&msg);

	PMatrix mat = createMatrix(arr, 3, &msg);

	method = RANDOM;
	root = create_tree(mat, method, -1);

	ASSERT_TRUE(db_equal(spPointGetAxisCoor(root->data, 0), 2.0, EPS));
	ASSERT_TRUE(db_equal(spPointGetAxisCoor(root->data, 1), 3.1, EPS));
	ASSERT_TRUE(db_equal(spPointGetAxisCoor(root->data, 2), 1.2, EPS));
	ASSERT_TRUE(root->left == NULL);
	ASSERT_TRUE(root->right == NULL);

	destroyKDTree(root);
	DestroyKDArray(arr);
	//DestroyMatrix(mat,3);
	spPointDestroy(p0);

	return true;
}

bool create_tree_test4(){
	SP_AUX_MSG msg = SP_AUX_INDEX_OUT_OF_RANGE;
	SP_KDTREE_SPLIT_METHOD method;
	PKDTreeNode root, left_node;


	PKDArray arr;

	double d1[3] = {1.8,0.1,8.2};
	SPPoint p1 = spPointCreate(d1,3,1);

	double d2[3] = {0.80,3.4,9.7};
	SPPoint p2 = spPointCreate(d2,3,2);

	double d3[3] = {0.4,3.5,11.7};
	SPPoint p3 = spPointCreate(d3,3,3);

	SPPoint points_arr[3];
	points_arr[0] = p1;
	points_arr[1] = p2;
	points_arr[2] = p3;


	arr = init(points_arr,3,&msg);

	PMatrix mat = createMatrix(arr, 3, &msg);

	method = MAX_SPREAD;
	root = create_tree(mat, method, -1);
	DestroyKDArray(arr);

	// walk on the tree and check it nodes.
	ASSERT_TRUE(root->data == NULL);
	ASSERT_TRUE(root->dim == 2);
	ASSERT_TRUE(root->left != NULL);
	ASSERT_TRUE(root->right != NULL);
	ASSERT_TRUE(db_equal(root->val, 9.7, EPS));

	// go to root's left child
	left_node = root->left;
	ASSERT_TRUE(left_node->data == NULL);
	ASSERT_TRUE(left_node->dim == 1);
	ASSERT_TRUE(left_node->left != NULL);
	ASSERT_TRUE(left_node->right != NULL);
	ASSERT_TRUE(db_equal(left_node->val, 3.4, EPS));

	// go over the leafs
	ASSERT_TRUE(db_equal(spPointGetAxisCoor(left_node->left->data, 0), 1.8, EPS));
	ASSERT_TRUE(db_equal(spPointGetAxisCoor(left_node->left->data, 1), 0.1, EPS));
	ASSERT_TRUE(db_equal(spPointGetAxisCoor(left_node->left->data, 2), 8.2, EPS));
	ASSERT_TRUE(left_node->left->left == NULL);
	ASSERT_TRUE(left_node->left->right == NULL);

	ASSERT_TRUE(db_equal(spPointGetAxisCoor(left_node->right->data, 0), 0.80, EPS));
	ASSERT_TRUE(db_equal(spPointGetAxisCoor(left_node->right->data, 1), 3.4, EPS));
	ASSERT_TRUE(db_equal(spPointGetAxisCoor(left_node->right->data, 2), 9.7, EPS));
	ASSERT_TRUE(left_node->right->left == NULL);
	ASSERT_TRUE(left_node->right->right == NULL);

	ASSERT_TRUE(db_equal(spPointGetAxisCoor(root->right->data, 0), 0.4, EPS));
	ASSERT_TRUE(db_equal(spPointGetAxisCoor(root->right->data, 1), 3.5, EPS));
	ASSERT_TRUE(db_equal(spPointGetAxisCoor(root->right->data, 2), 11.7, EPS));
	ASSERT_TRUE(root->right->left == NULL);
	ASSERT_TRUE(root->right->right == NULL);

	destroyKDTree(root);
	//DestroyMatrix(mat, 3);
	spPointDestroy(p1);
	spPointDestroy(p2);
	spPointDestroy(p3);


	return true;
}


bool knn_test(){
	// no way to check for real.. just see that it doesn't collapse
	int spknn = 3;
	SPBPQueue queue = init_bpqueue(spknn);

	SP_AUX_MSG msg = SP_AUX_INDEX_OUT_OF_RANGE;
	SP_KDTREE_SPLIT_METHOD method;
	PKDTreeNode root;


	PKDArray arr;

	double d0[3] = {2.0,3.1,1.2};
	SPPoint p0 = spPointCreate(d0,3,0);

	double d1[3] = {1.8,0.1,8.2};
	SPPoint p1 = spPointCreate(d1,3,1);

	double d2[3] = {0.80,3.4,9.7};
	SPPoint p2 = spPointCreate(d2,3,2);


	SPPoint points_arr[3];
	points_arr[0] = p0;
	points_arr[1] = p1;
	points_arr[2] = p2;


	arr = init(points_arr,3,&msg);

	PMatrix mat = createMatrix(arr, 3, &msg);

	method = MAX_SPREAD;
	root = create_tree(mat, method, -1);

	DestroyKDArray(arr);

	knn(root, queue, p0);

	destroyKDTree(root);
	//DestroyMatrix(mat, 3);
	spPointDestroy(p1);
	spPointDestroy(p2);
	spPointDestroy(p0);
	spBPQueueDequeue(queue);


	return true;

}

bool get_nearest_images_test(){

	// no way to check for real.. just see that it doesn't collapse
	int spknn = 3, arr_size = 3, num_of_similar_images = 2, num_of_images = 2;
	int* nearest_imgs;

	SP_AUX_MSG msg = SP_AUX_INDEX_OUT_OF_RANGE;
	SP_KDTREE_SPLIT_METHOD method;
	PKDTreeNode root;


	PKDArray arr;

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

	PMatrix mat = createMatrix(arr, 3, &msg);

	method = MAX_SPREAD;
	root = create_tree(mat, method, -1);

	nearest_imgs = get_nearest_images(root,points_arr, arr_size, num_of_images,spknn,num_of_similar_images);

	destroyKDTree(root);
	DestroyKDArray(arr);
	//DestroyMatrix(mat, 3);
	spPointDestroy(p1);
	spPointDestroy(p2);
	spPointDestroy(p0);
	free(nearest_imgs);

	return true;

}
/*
  This test cannot run here because it is c++ and not c and we ran this exact code in the main.
  It went good - kululush

bool show_results_test(){

	int idx_arr[4] = {5,2,6,7};
	char *query_path = "C:\Ron";
	SP_CONFIG_MSG msg;

	// first test when gui is true - suppose to show images
	SPConfig config_gui_true =
			spConfigCreate("C:\\Users\\USER\\c_workspace\\finalProject\\show_result_gui_true_config.txt",&msg);
	show_results(idx_arr, config_gui_true, query_path);

	// second test when gui is false - suppose to print
	SPConfig config_gui_false = spConfigCreate("C:\\Users\\USER\\c_workspace\\finalProject\\show_result_gui_false_config.txt",&msg);
	show_results(idx_arr, config_gui_false, query_path);


	spConfigDestroy(config_gui_true);
	spConfigDestroy(config_gui_false);

	return true;
}
*/

int main(){

	RUN_TEST(get_split_dim_test);
	RUN_TEST(get_median_value_test);
	RUN_TEST(create_node_test);
	RUN_TEST(create_leaf_test);
	RUN_TEST(create_tree_test);
	RUN_TEST(create_tree_test2);
	RUN_TEST(create_tree_test3);
	RUN_TEST(create_tree_test4);
	RUN_TEST(knn_test);
	RUN_TEST(get_nearest_images_test);

	return 0;

}


