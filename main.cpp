#include <cstdlib>
#include <cassert>
#include <cstring>
#include <cstdio>
#include "SPImageProc.h"
extern "C" {
#include "SPLogger.h"
#include "SPPoint.h"
#include "SPConfig.h"
#include "main_aux.h"
}



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
#define CAN_NOT_GET_NUM_OF_SIMLAR "Can not assign SPNumOfSimilarImages"
#define CAN_NOT_GET_MINIMAL_GUI "Can not assign Minimal Gui"
#define CAN_NOT_GET_IMAGE_PATH "Can not assign Image Path"
#define INVALID_CMD_LINE "Invalid command line : use -c <config_filename>"
#define VALID_CMD_CANT_OPEN "The configuration file <filename> couldn’t be open"
#define DEFAULT_CMD_CANT_OPEN "The default configuration file spcbir.config couldn’t be open"
#define EXIT_MSG "Exiting...\n"
#define GET_QUERY "Please enter an image path:"
#define CAN_NOT_CREATE_POINT_ARR "MAIN: Can not create array from file"
#define CAN_NOT_CREATE_KD_ARR "MAIN: Can not create KDArray"
#define CAN_NOT_CREATE_MATRIX "MAIN: Can not create matrix object"
#define CAN_NOT_CREATE_KDTREE "MAIN: Can not create KDTree"
#define CAN_NOT_CREATE_IDX_ARR "MAIN: Can not create index array"


#define MAX_PCA_DIM 28
#define CONFIG_DEFAULT "spcbir.config"

#define FREE_AND_NULL_MAIN(ptr) do { \
								if ((ptr)!= NULL) { \
									free((void*)(ptr)); \
									(ptr) = NULL; \
								} \
} while(0);

/*
 * This method gets as input the configuration file, and creates new features files for each image.
 * For image ./images/img5.png the new file will be ./images/img5/feats
 * The format of the features file will be as followed:
 * first line includes number of features of current image, number of dimensions for features and image index
 * the rest of the file will contain a type double matrix - the SIFTs values.
 *
 * for example, image 4 has 2 features, each with 3 dimensions:
 * 2 3 4
 * 4 7 8
 * 5 3 9
 */
void writeFeaturesToFile(SPConfig config);


/*
 * This method gets as input array contains the indices of nearest images, the configuration file
 * and the path of query image. According to configurations it prints or shows the nearest images.
 */
void show_results(int* idx_arr,SPConfig config, char* query_img_path);


int main(int argc, char* argv[]){

	char *config_filename, *logger_filename, *query_filename, *pos;
	SP_CONFIG_MSG config_msg;
	bool is_default=false, extract_mode, need_to_exit=false;
	SPConfig config;
	SP_LOGGER_LEVEL logger_level;
	SP_LOGGER_MSG logger_msg;
	SPPoint * feats_as_points, *query_features;
	SP_AUX_MSG aux_msg;
	PKDArray kd_array;
	PMatrix pmatrix;
	PKDTreeNode root;
	SP_KDTREE_SPLIT_METHOD method;
	int *nearest_img_idx = NULL;
	int total_features = 0, dim, query_total_features = 0, num_of_img, num_of_similar_img, spknn, query_idx;

	// ##### check command line arguments ###### //

	printf("MAIN: args check\n");

	// argv[0] = program name, argv[1] = flag argv[2] = config file
	if (argc == 1){ // default config file
		is_default = true;
		config_filename = (char *) malloc(sizeof(char) * strlen(CONFIG_DEFAULT));
		if (config_filename == NULL){
			printf("%s\n%s",ALLOC_ERROR_MSG, EXIT_MSG);
			exit(EXIT_FAILURE);
		}
		sprintf(config_filename, (char *)CONFIG_DEFAULT);
	}
	else if (argc == 3){// valid number of arguments
		is_default = false;
		if (strcmp(argv[1], "-c") != 0){ // wrong flag
			printf("%s\n", INVALID_CMD_LINE);
			exit(EXIT_FAILURE); // terminate program
		}

		config_filename = (char *) malloc(sizeof(char) * MAX_LINE_LEN);
		if (config_filename == NULL){
			printf("%s\n%s",ALLOC_ERROR_MSG, EXIT_MSG);
			exit(EXIT_FAILURE);
		}
		strncpy(config_filename, argv[2], strlen(argv[2]));
	}
	else{	// invalid command line argument
		printf("%s\n", INVALID_CMD_LINE);
		exit(EXIT_FAILURE);
	}


	//###### create config ######//
	config = spConfigCreate(config_filename, &config_msg);
	if (config_msg == SP_CONFIG_CANNOT_OPEN_FILE){
		if (is_default == true){
			printf("%s\n", DEFAULT_CMD_CANT_OPEN);
			FREE_AND_NULL_MAIN(config_filename);
			exit(EXIT_FAILURE);
		}
		else{
			printf("The configuration file %s couldn’t be open\n", config_filename);
			FREE_AND_NULL_MAIN(config_filename);
			exit(EXIT_FAILURE);
		}
	}
	else if (config_msg != SP_CONFIG_SUCCESS || config == NULL){
		// create config prints the appropriate message
		printf("%s\n", GENERAL_ERROR_MSG);
		FREE_AND_NULL_MAIN(config_filename);
		exit(EXIT_FAILURE);
	}
	// config is valid - can free config filename
	FREE_AND_NULL_MAIN(config_filename);

	printf("MAIN: config is created\n");


	// ########### extract needed fields from config #########//
	extract_mode = spConfigIsExtractionMode(config, &config_msg);
	if (config_msg != SP_CONFIG_SUCCESS){
		need_to_exit = true;
	}

	dim = spConfigGetPCADim(config, &config_msg);
	if (config_msg != SP_CONFIG_SUCCESS){
		need_to_exit = true;
	}

	method = SPConfigGetSplitMethod(config, &config_msg);
	if (config_msg != SP_CONFIG_SUCCESS){
		need_to_exit = true;
	}

	num_of_img = spConfigGetNumOfImages(config, &config_msg);
	if (config_msg != SP_CONFIG_SUCCESS){
		need_to_exit = true;
	}

	num_of_similar_img = SPConfigGetNumOfSimilarImages(config, &config_msg);
	if (config_msg != SP_CONFIG_SUCCESS){
		need_to_exit = true;
	}

	spknn = SPConfigGetSpknn(config, &config_msg);
	if (config_msg != SP_CONFIG_SUCCESS){
		need_to_exit = true;
	}

	logger_level = SPConfigGetLoggerLevel(config, &config_msg);
	if (config_msg != SP_CONFIG_SUCCESS){
		need_to_exit = true;
	}

	logger_filename = (char *) malloc(sizeof(char) * MAX_LINE_LEN);
	if (logger_filename == NULL){
		printf("%s\n%s",ALLOC_ERROR_MSG, EXIT_MSG);
		spConfigDestroy(config);
		exit(EXIT_FAILURE);
	}
	config_msg = spConfigGetLoggerFilename(logger_filename, config);
	if (config_msg != SP_CONFIG_SUCCESS){
		need_to_exit = true;
	}

	if (need_to_exit == true){
		FREE_AND_NULL_MAIN(logger_filename);
		spConfigDestroy(config);
		printf("An Error occurred - Exiting...\n");
		exit(EXIT_FAILURE);
	}


	// #### create logger #####//
	if (strncmp(logger_filename, (const char *)"stdout", 6) == 0){
		// stdout mode - need to set loger to NULL and print all logger messages to screen
		FREE_AND_NULL_MAIN(logger_filename);
	}

	logger_msg = spLoggerCreate(logger_filename,logger_level);
	if (logger_msg != SP_LOGGER_SUCCESS){
		printf("An Error occurred while creating logger - Exiting...\n");
		FREE_AND_NULL_MAIN(logger_filename);
		spConfigDestroy(config);
		exit(EXIT_FAILURE);
	}
	// logger is valid - can free logger filename
	FREE_AND_NULL_MAIN(logger_filename);

	printf("MAIN: logger is created - to pre process\n");


	//####### pre-process ####### //

	sp::ImageProc improc = sp::ImageProc((const SPConfig) config);

	// write- extract_mode == true, read - extract_mode == false
	if (extract_mode == true){// need to write to file
		writeFeaturesToFile(config);	// consider adding a message to know it works fine
	}

	printf("MAIN: before read from file\n");

	// read mode is always applied
	feats_as_points = readFeaturesFromFile(config, &total_features);
	if (feats_as_points == NULL){
		spLoggerPrintError(CAN_NOT_CREATE_POINT_ARR,__FILE__,__func__,__LINE__);
		spConfigDestroy(config);
		spLoggerDestroy();
		exit(EXIT_FAILURE);
	}


	//######## create the KDTREE #############//
	kd_array = init(feats_as_points, total_features, &aux_msg);
	if (aux_msg != SP_AUX_SUCCESS || kd_array == NULL){
		spLoggerPrintError(CAN_NOT_CREATE_KD_ARR,__FILE__,__func__,__LINE__);
		spConfigDestroy(config);
		spLoggerDestroy();
		destroyPointsArray(feats_as_points, total_features);
		exit(EXIT_FAILURE);
	}

	// create the matrix to be used in building the tree
	pmatrix = createMatrix(kd_array, dim, &aux_msg);
	if (aux_msg != SP_AUX_SUCCESS || pmatrix == NULL){
		spLoggerPrintError(CAN_NOT_CREATE_MATRIX,__FILE__,__func__,__LINE__);
		spConfigDestroy(config);
		spLoggerDestroy();
		destroyPointsArray(feats_as_points, total_features);
		DestroyKDArray(kd_array);
		exit(EXIT_FAILURE);
	}

	printf("MAIN: before tree\n");

	// create the tree
	root = create_tree(pmatrix, method, -1);	// prev dim is always -1 because INCREMENTAL starts with 0
	if (root == NULL){ // problem in memory allocation or in split
		spLoggerPrintError(CAN_NOT_CREATE_KDTREE,__FILE__,__func__,__LINE__);
		spConfigDestroy(config);
		spLoggerDestroy();
		destroyPointsArray(feats_as_points, total_features);
		DestroyKDArray(kd_array);
		if(pmatrix != NULL){	// matrix was not destroyed in create tree
			DestroyMatrix(pmatrix, dim);
		}
		exit(EXIT_FAILURE);
	}

	// kdarray is copied to the matrix and no longer needed
	DestroyKDArray(kd_array);


	//############# QUERY ##############//
	spLoggerPrintInfo("MAIN: Entered query section\n");
	printf("MAIN: query section\n");

	query_filename = (char *)malloc(sizeof(char) *MAX_LINE_LEN);
	if (query_filename == NULL){
		spLoggerPrintError(ALLOC_ERROR_MSG,__FILE__,__func__,__LINE__);
		spConfigDestroy(config);
		spLoggerDestroy();
		destroyPointsArray(feats_as_points, total_features);
		//DestroyMatrix(pmatrix, dim);
		destroyKDTree(root);
		exit(EXIT_FAILURE);
	}

	while (true){
		printf("MAIN: in the beginning while loop of query section\n");
		memset(query_filename, 0x00, strlen(query_filename));
		printf("%s\n", GET_QUERY);
		fflush(NULL);
		fgets(query_filename, MAX_LINE_LEN, stdin);
		if ((pos=strchr(query_filename, '\n')) != NULL){
		    *pos = '\0';
		}
		//query_filename[MAX_LINE_LEN - 1] = '\0'; // replace \n suffix from fgets

		// if the user entered the string "<>" exit program
		if (strncmp((const char *)query_filename, (const char *)"<>",2) == 0){
			spLoggerPrintInfo(EXIT_MSG);
			FREE_AND_NULL_MAIN(query_filename);
			spConfigDestroy(config);
			spLoggerDestroy();
			destroyPointsArray(feats_as_points, total_features);
			//DestroyMatrix(pmatrix, dim);
			destroyKDTree(root);
			printf("%s\n", EXIT_MSG);
			exit(EXIT_SUCCESS);
		}

		// continue program flow
		spLoggerPrintInfo("MAIN: Query path is valid\n");


		//######### get features for query #########//
		// moved UP!
		//sp::ImageProc improc = sp::ImageProc((const SPConfig) config);
		//sp::ImageProc* improc = new sp::ImageProc((const SPConfig) config);
		// set index as num_of_img + 10 to prevent overlaps in the total features points index
		query_idx = num_of_img + 10;
		query_features = improc.getImageFeatures((const char*)query_filename, query_idx, &query_total_features);

		nearest_img_idx = get_nearest_images(root, query_features, query_total_features,
				num_of_img, spknn, num_of_similar_img);

		if (nearest_img_idx == NULL){
			spLoggerPrintError(CAN_NOT_CREATE_IDX_ARR,__FILE__,__func__,__LINE__);
			FREE_AND_NULL_MAIN(query_filename);
			spConfigDestroy(config);
			spLoggerDestroy();
			destroyPointsArray(feats_as_points, total_features);
			destroyPointsArray(query_features, query_total_features);
			//DestroyMatrix(pmatrix, dim);
			destroyKDTree(root);
			exit(EXIT_FAILURE);
		}

		show_results(nearest_img_idx, config, query_filename);
		spLoggerPrintInfo("MAIN: finished showing result for query\n");

		printf("MAIN: in the end of while loop of query section\n");


		// free all allocations
		destroyPointsArray(query_features, query_total_features);
		FREE_AND_NULL_MAIN(nearest_img_idx);
	}

	return 0;
}


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




void show_results(int* idx_arr, SPConfig config, char* query_img_path){

	char* img_path;
	int img_idx;
	int num_of_similar_images;
	bool minimal_gui;
	SP_CONFIG_MSG msg = SP_CONFIG_MISSING_PREFIX; // temporary value

	num_of_similar_images = SPConfigGetNumOfSimilarImages(config, &msg);
	if (msg != SP_CONFIG_SUCCESS){
		spLoggerPrintError(CAN_NOT_GET_NUM_OF_SIMLAR,__FILE__,__func__,__LINE__);
		return;
	}

	minimal_gui = spConfigMinimalGui(config, &msg);
	if (msg != SP_CONFIG_SUCCESS){
		spLoggerPrintError(CAN_NOT_GET_MINIMAL_GUI,__FILE__,__func__,__LINE__);
		return;
	}

	sp::ImageProc improc = sp::ImageProc((const SPConfig) config); // how to free this ###############

	if (minimal_gui == false){
		printf("Best candidates for - %s - are:\n",query_img_path);
		fflush(NULL);
	}

	// for each image - present it according to minimal gui
	for (int i = 0; i < num_of_similar_images; i++){
		img_idx = idx_arr[i];
		img_path = (char*)malloc(sizeof(char) * MAX_LINE_LEN);
		if (img_path == NULL){
			spLoggerPrintError(ALLOC_ERROR_MSG,__FILE__,__func__,__LINE__);
			return;
		}

		msg = spConfigGetImagePath(img_path, config, img_idx);
		if (msg != SP_CONFIG_SUCCESS){
			spLoggerPrintError(CAN_NOT_GET_IMAGE_PATH,__FILE__,__func__,__LINE__);
			FREE_AND_NULL_MAIN(img_path);
			return;
		}

		if (minimal_gui == true){
			// show image on screen
			improc.showImage((const char*)img_path);
		}
		else{
			printf("%s\n",img_path);
			fflush(NULL);
		}
		FREE_AND_NULL_MAIN(img_path);
	}

}
