
#ifndef MAIN_AUX_H_
#define MAIN_AUX_H_

#include "SPLogger.h"
#include "SPPoint.h"
#include "SPConfig.h"
#include "SPBPriorityQueue.h"

/*
 * define structs to contain the kd array elements
 *
 * KDElement
 *  pivot - the splitting dimension. default value is -1
 */
typedef struct kd_element{
	SPPoint point;
	int index;
	int pivot;
}KDElement, *PKDElement;

typedef struct kd_array{
	KDElement* data;
	int size;
}KDArray, *PKDArray;

/*
 * a struct that will contain a 2D matrix sized d*n of the KD Arrays after being sorted according
 * to the i'th dimension
 */
typedef struct matrix{
	int ** matrix;
	PKDArray kd_array;
	int rows;
	int cols;
}Matrix, *PMatrix;


//############### KD TREE##############//
/*
 * a struct representing a node in the KDtree.
 * Dim = The splitting dimension
 * Val = The median value of the splitting dimension
 * Left = Pointer to the left subtree
 * Right = Pointer to the right subtree
 * Data = Pointer to a point (only if the current node is a leaf) otherwise this field value is NULL
 */

typedef struct kd_tree_node{
	int dim;
	double val;
	struct kd_tree_node* left;
	struct kd_tree_node* right;
	SPPoint data;
}KDTreeNode, *PKDTreeNode;

/*
 * struct to be used to count amount of appearances of each image as one of the spknn nearest images.
 */
typedef struct img_cnt{
	int img_idx;
	int cnt;
}ImageCounter, *PImageCounter;


/*
 * ENUM contains general messages in order to track system running
 */
typedef enum sp_aux_msg_t {
	SP_AUX_CANNOT_OPEN_FILE,
	SP_AUX_ALLOC_FAIL,
	SP_AUX_INVALID_INTEGER,
	SP_AUX_INVALID_STRING,
	SP_AUX_INVALID_ARGUMENT,
	SP_AUX_GENERAL_MSG,
	SP_AUX_INDEX_OUT_OF_RANGE,
	SP_AUX_MATRIX_ERR,
	SP_AUX_COPY_ARR_FAIL,
	SP_AUX_SPLIT_MAP_ERR,
	SP_AUX_SPLIT_ERR,
	SP_AUX_SUCCESS
} SP_AUX_MSG;


/*
 * This method gets a configuration file, checks that the features files exist.
 * If not - returns NULL and aborts the program.
 * Else - returns array of SPPoints - each point represents a feature of an image.
 *
 * @param
 * config - a configuration file
 * size - a pointer that will contain the size of the returned array
 */
SPPoint* readFeaturesFromFile(SPConfig config, int* size);

/*
 * this method receives a configuration file and sums all features of each image in the directory
 * given in the
 */
int getTotalNumOfFeatures(SPConfig config);

/*
 * this method frees SPPoint array.
 *
 * @param
 * array - an array of points
 * size - length of array
 *
 * we assume that array is not NULL
 */
void free_point_arr(SPPoint* array, int size);


/*
 * this method initialize a SPKDArray from a given SPPoint array
 *
 * @param
 * arr - an array of points
 * size - the size of the array
 *
 * @return
 * SPKDArray containing each point from arr and it's corresponding index
 *
 * if arr == NULL or size <= 0, exit the program
 */
PKDArray init(SPPoint* arr, int size, SP_AUX_MSG* msg);

/*
 * free all KDArray allocations
 */
void DestroyKDArray(PKDArray arr);

/*
 * this method creates a copy of a KDArray that starts in index start and end in index end -1
 *
 *@return
 * copied KDArray - whole array will be copied in start = 0 and end = arr->size
 */
PKDArray copyArray(PKDArray arr, int start, int end, SP_AUX_MSG* msg);

/*
 * a comparator used to compare two KDElements
 * this method will be used in sorting the KD Array points data according to pivot
 *
 *  example - pa > pb = pa's point data in the pivot index in larger then pb's.
 *
 * @return
 * if pa > pb - 1
 * if pa < pb - (-1)
 * otherwise - 0
 */
int compare(const void* pa, const void* pb) ;

/*
 * a comparator used to compare two KDElements
 * this method will be used in sorting the KD Array elements by their indices
 * after splitting the kd array into left and right
 *
 * @return
 * if pa > pb - 1
 * if pa < pb - (-1)
 * otherwise - 0
 */
int compare_idx(const void* pa, const void* pb);

/*
 * Comparator used to compare ImageCounter.
 * The sort according to cnt field is decreasing and
 * in case they are equal we sort by img_idx ascending.
 */

int comapre_img_cnt_desc(const void* pa, const void* pb);

/*
 * memory allocation for PMatrix
 */
PMatrix allocate_mem_matrix(int num_of_rows, SP_AUX_MSG* msg);

/*
 * this method generate an indices matrix object from a given KD array.
 *  by sorting the array according to each dimension the function creates an array of indices
 *  corresponds to the indices of those elements in the original KD array.
 *
 *  @param
 *  KD array
 *	dim - the size of the points array in each point (assuming all points has the same data length
 *
 *  @return
 *  a matrix of indices, a line for each sort.
 */
PMatrix createMatrix(PKDArray kd_arr, int dim, SP_AUX_MSG* msg);


/*
 * this method frees all memory allocations of a Matrix object up to the i'th row
 */
void DestroyMatrix(PMatrix mat, int rows);

/*
 * This method receives PMatrix, a coordinate and a middle point integer. Maps the elements
 * into left and right sub-KDArrays.
 */
int* createSplitMap(PMatrix pmat, int coor, int mid, SP_AUX_MSG* msg);

/*
 *  This method receives a matrix, split map and indicator and returns an array of the new indices
 *  of each element in the array according to the split indicator as instructed in the instruction file.
 */
int* createNewIdx(PMatrix pmat, int* split_map, int indicator, SP_AUX_MSG* msg);

/*
 * This method receives a kd array and an array of integers representing their indices when
 * sorted according to specific coor (pmatrix->matrix[coor]) and creates
 * a new kd array sorted according those indices
 */
PKDArray create_array_by_coor(PKDArray arr, int* sorted_idx_arr);


/*
 * this function receives a matrix object and an integer coordinate
 * and splits the kd array according to coor dimension into two separated arrays.
 *
 * @param
 * pmatrix - a PMatrix object which contains the KD array
 * coor - the dimension to split by
 *
 * @return
 * if pmatrix == NULL or coor < 0  or coor <= dimension return NULL
 * a pointer to a PMatrix array containing the two parts of the matrix after split
 */
PMatrix* split(PMatrix pmatrix, int coor, SP_AUX_MSG* msg);


/*
 * this method calculates the dimension to split by while KDArray method is
 * MAX_SPREAD - the returned coordinate will be the one which the differences between the
 * maximum value and the minimum value is the largest
 *
 * @return
 * the splitting dimension in case of success, -1 otherwise
 */
int get_max_spread(PMatrix pmatrix);

/*
 * this method receives a pointer to a matrix, a splitting method and previous splitting dimension
 * and returns the current splitting dimension according to method as instructed
 */
int get_split_dim(PMatrix pmatrix, int prev_split_dim, SP_KDTREE_SPLIT_METHOD method);

/*
 * this method extract the median value according to dimension split_dim
 */
double get_median_value(PMatrix pmatrix, int split_dim);

/*
 * this method creates a new node from type PKDTreeNode as instructed
 * note that the fields left and right are initialized as NULL and will be
 * generated recursively
 *
 */
PKDTreeNode create_node(int split_dim, double median);

/*
 * this method creates a leaf node in the KDTree
 * INF - represents invalid for dim and val fields
 */
PKDTreeNode create_leaf(SPPoint point);


/*
 * This is a recursive method which creates a KDTree from a PMatrix.
 * It uses the split_method and prev_split_dim in order to decide how to split the next
 * level of the tree.


 * see in KDTree struct definition "for us".
 */
PKDTreeNode create_tree(PMatrix pmatrix, SP_KDTREE_SPLIT_METHOD split_method, int prev_split_dim);


/*
 * This method creates a new BPQueue to be used in the SPknn search.
 * The spknn is the maximum capacity of the new queue.
 */
SPBPQueue init_bpqueue(int spknn);

/*
 * This is a recursive method that gets a point that represents a feature and fills the priority queue
 * with the indices of the nearest images according to the pseudo code in the instruction file.
 */
void knn(PKDTreeNode node,SPBPQueue queue, SPPoint point);

/*
 * This method receives the root of the KDTree, array of SPPoints represents all features
 * and using the knn method to get the nearest images according to the SPPoints array
 * @param
 * root - The root of the KDTree
 * query_image_features - array of SPPoints for the features of query image
 * arr_size - amount of elements in query_image_features
 */
int* get_nearest_images(PKDTreeNode root ,SPPoint* query_image_features, int arr_size,
		int num_of_images, int spknn, int num_of_similar_images);

/*
 * This method destroys all allocation of a KDTree starting with node as a root
 */
void destroyKDTree(PKDTreeNode node);

/*
 * destroy an array of points
 */
void destroyPointsArray(SPPoint *points, int size);

#endif /* MAIN_AUX_H_ */
