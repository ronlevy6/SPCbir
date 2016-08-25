#include "../SPConfig.h"
#include "unit_test_util.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
/*
typedef enum sp_kdtree_split_method_t{
	RANDOM,
	MAX_SPREAD,
	INCREMENTAL
} SP_KDTREE_SPLIT_METHOD;
*/
struct sp_config_t{
	char * spImagesDirectory;
	char* spImagesPrefix;
	char * spImagesSuffix;
	int spNumOfImages;
	int spPCADimension;
	char * spPCAFilename;
	int spNumOfFeatures;
	bool spExtractionMode;
	int spNumOfSimilarImages;
	SP_KDTREE_SPLIT_METHOD spKDTreeSplitMethod;
	int spKNN;
	bool spMinimalGUI;
	SP_LOGGER_LEVEL spLoggerLevel;
	char * spLoggerFilename;
};

// this is a unit test for config.c
bool spConfigCreateTest(){
	SP_CONFIG_MSG msg;
	SPConfig config;

	config = spConfigCreate("valid_config.txt", &msg);
	//ASSERT_TRUE(msg == SP_CONFIG_SUCCESS);
	ASSERT_TRUE(msg == 10); // 10 stands for SP_CONFIG_SUCCESS
	ASSERT_TRUE(strcmp(config->spImagesDirectory, "./images/") == 0);
	ASSERT_TRUE(strcmp(config->spImagesPrefix, "img") == 0);
	ASSERT_TRUE(strcmp(config->spImagesSuffix, ".png") == 0);
	ASSERT_TRUE(strcmp(config->spPCAFilename, "pca_new.yml") == 0);
	ASSERT_TRUE(config->spNumOfImages == 12);
	ASSERT_TRUE(config->spPCADimension == 16);
	ASSERT_TRUE(config->spNumOfFeatures == 8);
	ASSERT_TRUE(config->spExtractionMode == true);
	ASSERT_TRUE(config->spNumOfSimilarImages == 2);
	ASSERT_TRUE(config->spKDTreeSplitMethod == RANDOM);
	ASSERT_TRUE(config->spKNN == 3);
	ASSERT_TRUE(config->spMinimalGUI == false);
	ASSERT_TRUE(config->spLoggerLevel == SP_LOGGER_WARNING_ERROR_LEVEL);
	ASSERT_TRUE(strcmp(config->spLoggerFilename, "logger_file") == 0);
	spConfigDestroy(config);


	config = spConfigCreate("valid_empty_line_and_comment_config.txt", &msg);
	ASSERT_TRUE(msg == SP_CONFIG_SUCCESS);
	ASSERT_TRUE(strcmp(config->spImagesDirectory, "./images/") == 0);
	ASSERT_TRUE(strcmp(config->spImagesPrefix, "img") == 0);
	ASSERT_TRUE(strcmp(config->spImagesSuffix, ".gif") == 0);
	ASSERT_TRUE(strcmp(config->spPCAFilename, "pca_new.yml") == 0);
	ASSERT_TRUE(config->spNumOfImages == 10);
	ASSERT_TRUE(config->spPCADimension == 16);
	ASSERT_TRUE(config->spNumOfFeatures == 8);
	ASSERT_TRUE(config->spExtractionMode == true);
	ASSERT_TRUE(config->spNumOfSimilarImages == 2);
	ASSERT_TRUE(config->spKDTreeSplitMethod == RANDOM);
	ASSERT_TRUE(config->spKNN == 3);
	ASSERT_TRUE(config->spMinimalGUI == false);
	ASSERT_TRUE(config->spLoggerLevel == SP_LOGGER_WARNING_ERROR_LEVEL);
	ASSERT_TRUE(strcmp(config->spLoggerFilename, "logger_file") == 0);
	spConfigDestroy(config);


	config = spConfigCreate("valid_malan_whitespaces_config.txt", &msg);
	ASSERT_TRUE(msg == SP_CONFIG_SUCCESS);
	ASSERT_TRUE(strcmp(config->spImagesDirectory, "./images/") == 0);
	ASSERT_TRUE(strcmp(config->spImagesPrefix, "img") == 0);
	ASSERT_TRUE(strcmp(config->spImagesSuffix, ".png") == 0);
	ASSERT_TRUE(strcmp(config->spPCAFilename, "pca_new.yml") == 0);
	ASSERT_TRUE(config->spNumOfImages == 10);
	ASSERT_TRUE(config->spPCADimension == 16);
	ASSERT_TRUE(config->spNumOfFeatures == 8);
	ASSERT_TRUE(config->spExtractionMode == true);
	ASSERT_TRUE(config->spNumOfSimilarImages == 2);
	ASSERT_TRUE(config->spKDTreeSplitMethod == RANDOM);
	ASSERT_TRUE(config->spKNN == 3);
	ASSERT_TRUE(config->spMinimalGUI == false);
	ASSERT_TRUE(config->spLoggerLevel == SP_LOGGER_WARNING_ERROR_LEVEL);
	ASSERT_TRUE(strcmp(config->spLoggerFilename, "logger_file") == 0);
	spConfigDestroy(config);


	config = spConfigCreate("bad_KDsplit_enum_config.txt", &msg);
	ASSERT_TRUE(msg == SP_CONFIG_INVALID_STRING);
	ASSERT_TRUE(config == NULL);
	spConfigDestroy(config);


	config = spConfigCreate("bad_num_of_img_config.txt", &msg);
	ASSERT_TRUE(msg == SP_CONFIG_INVALID_STRING);
	ASSERT_TRUE(config == NULL);
	spConfigDestroy(config);

	config = spConfigCreate("bad_prefix_key_config.txt", &msg);
	ASSERT_TRUE(msg == SP_CONFIG_INVALID_STRING);
	spConfigDestroy(config);

	config = spConfigCreate("invalid_gibrish_key_config.txt", &msg);
	ASSERT_TRUE(msg == SP_CONFIG_INVALID_STRING);
	spConfigDestroy(config);

	config = spConfigCreate("negetive_num_of_img_config.txt", &msg);
	ASSERT_TRUE(msg == SP_CONFIG_INVALID_INTEGER);
	spConfigDestroy(config);

	config = spConfigCreate("no_directory_config.txt", &msg);
	ASSERT_TRUE(msg == SP_CONFIG_MISSING_DIR);
	spConfigDestroy(config);

	config = spConfigCreate("no_value_num_of_feat_config.txt", &msg);
	ASSERT_TRUE(msg == SP_CONFIG_INVALID_STRING);
	spConfigDestroy(config);

	config = spConfigCreate("spKNN_in_comment_config.txt", &msg);
	ASSERT_TRUE(msg == SP_CONFIG_SUCCESS);
	ASSERT_TRUE(strcmp(config->spImagesDirectory, "./images/") == 0);
	ASSERT_TRUE(strcmp(config->spImagesPrefix, "img") == 0);
	ASSERT_TRUE(strcmp(config->spImagesSuffix, ".png") == 0);
	ASSERT_TRUE(strcmp(config->spPCAFilename, "pca_new.yml") == 0);
	ASSERT_TRUE(config->spNumOfImages == 10);
	ASSERT_TRUE(config->spPCADimension == 16);
	ASSERT_TRUE(config->spNumOfFeatures == 8);
	ASSERT_TRUE(config->spExtractionMode == true);
	ASSERT_TRUE(config->spNumOfSimilarImages == 2);
	ASSERT_TRUE(config->spKDTreeSplitMethod == RANDOM);
	ASSERT_TRUE(config->spKNN == 1);
	ASSERT_TRUE(config->spMinimalGUI == false);
	ASSERT_TRUE(config->spLoggerLevel == SP_LOGGER_WARNING_ERROR_LEVEL);
	ASSERT_TRUE(strcmp(config->spLoggerFilename, "logger_file") == 0);
	spConfigDestroy(config);


	config = spConfigCreate("directory_without_equal.txt", &msg);
	ASSERT_TRUE(msg == SP_CONFIG_INVALID_STRING);
	ASSERT_TRUE(config == NULL);
	spConfigDestroy(config);


	return true;

}

bool spConfigGetImagePathTest(){
	SP_CONFIG_MSG msg;
	SPConfig config;
	char * path1, *path2, *path3;
	int idx = 8;

	path1 = (char*)malloc(sizeof(char)*MAX_LINE_LEN);
	path2 = (char*)malloc(sizeof(char)*MAX_LINE_LEN);
	path3 = (char*)malloc(sizeof(char)*MAX_LINE_LEN);
	config = spConfigCreate("valid_config.txt", &msg);
	ASSERT_TRUE(msg == SP_CONFIG_SUCCESS);
	msg = spConfigGetImagePath(path1, config, idx);
	ASSERT_TRUE(msg == SP_CONFIG_SUCCESS);
	ASSERT_TRUE(strcmp(path1, "./images/img8.png")== 0);
	free(path1);

	idx = 10;
	msg = spConfigGetImagePath(path2, config, idx);
	ASSERT_TRUE(msg == SP_CONFIG_SUCCESS);
	ASSERT_TRUE(strcmp(path2, "./images/img10.png")== 0);
	free(path2);

	idx = 20;
	msg = spConfigGetImagePath(path3, config, idx);
	ASSERT_TRUE(msg == SP_CONFIG_INDEX_OUT_OF_RANGE);
	free(path3);

	msg = spConfigGetImagePath(NULL, config, idx);
	ASSERT_TRUE(msg == SP_CONFIG_INVALID_ARGUMENT);
	spConfigDestroy(config);

	return true;
}

bool spConfigGetPCAPathTest(){
	SP_CONFIG_MSG msg;
	SPConfig config;
	char *path1;

	config = spConfigCreate("valid_config.txt", &msg);
	path1 = (char*)malloc(sizeof(char)*MAX_LINE_LEN);
	msg = spConfigGetPCAPath(path1, config);
	ASSERT_TRUE(msg == SP_CONFIG_SUCCESS);
	ASSERT_TRUE(strcmp(path1, "./images/pca_new.yml")== 0);
	free (path1);

	msg = spConfigGetPCAPath(NULL, config);
	ASSERT_TRUE(msg == SP_CONFIG_INVALID_ARGUMENT);
	spConfigDestroy(config);

	return true;
}

bool spGettersTest(){
	SP_CONFIG_MSG msg;
	SPConfig config;
	bool extract, gui;
	int num_of_img, num_of_feat, pca_dim;

	config = spConfigCreate("valid_config.txt", &msg);

	extract = spConfigIsExtractionMode(config, &msg);
	ASSERT_TRUE(msg == SP_CONFIG_SUCCESS);
	ASSERT_TRUE(extract == true);
	gui = spConfigMinimalGui(config, &msg);
	ASSERT_TRUE(msg == SP_CONFIG_SUCCESS);
	ASSERT_TRUE(gui == false);
	num_of_img = spConfigGetNumOfImages(config, &msg);
	ASSERT_TRUE(msg == SP_CONFIG_SUCCESS);
	ASSERT_TRUE(num_of_img == 12);
	num_of_feat = spConfigGetNumOfFeatures(config, &msg);
	ASSERT_TRUE(msg == SP_CONFIG_SUCCESS);
	ASSERT_TRUE(num_of_feat == 8);
	pca_dim = spConfigGetPCADim(config, &msg);
	ASSERT_TRUE(msg == SP_CONFIG_SUCCESS);
	ASSERT_TRUE(pca_dim == 16);

	// bad inputs
	extract = spConfigIsExtractionMode(NULL, &msg);
	ASSERT_TRUE(msg == SP_CONFIG_INVALID_ARGUMENT);
	ASSERT_TRUE(extract == false);
	gui = spConfigMinimalGui(NULL, &msg);
	ASSERT_TRUE(msg == SP_CONFIG_INVALID_ARGUMENT);
	ASSERT_TRUE(gui == false);
	num_of_img = spConfigGetNumOfImages(NULL, &msg);
	ASSERT_TRUE(msg == SP_CONFIG_INVALID_ARGUMENT);
	ASSERT_TRUE(num_of_img < 0);
	num_of_feat = spConfigGetNumOfFeatures(NULL, &msg);
	ASSERT_TRUE(msg == SP_CONFIG_INVALID_ARGUMENT);
	ASSERT_TRUE(num_of_feat < 0);
	pca_dim = spConfigGetPCADim(NULL, &msg);
	ASSERT_TRUE(msg == SP_CONFIG_INVALID_ARGUMENT);
	ASSERT_TRUE(pca_dim < 0);
	spConfigDestroy(config);


	return true;
}
/*
int main(){
	RUN_TEST(spConfigCreateTest);
	RUN_TEST(spConfigGetImagePathTest);
	RUN_TEST(spConfigGetPCAPathTest);
	RUN_TEST(spGettersTest);

	return 0;
}

*/
