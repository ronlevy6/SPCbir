#ifndef SPCONFIG_H_
#define SPCONFIG_H_

#include <stdbool.h>
#include <stdio.h>
#include "SPLogger.h"

#define MAX_LINE_LEN 1024

/**
 * A data-structure which is used for configuring the system.
 */

typedef enum sp_config_msg_t {
	SP_CONFIG_MISSING_DIR,
	SP_CONFIG_MISSING_PREFIX,
	SP_CONFIG_MISSING_SUFFIX,
	SP_CONFIG_MISSING_NUM_IMAGES,
	SP_CONFIG_CANNOT_OPEN_FILE,
	SP_CONFIG_ALLOC_FAIL,
	SP_CONFIG_INVALID_INTEGER,
	SP_CONFIG_INVALID_STRING,
	SP_CONFIG_INVALID_ARGUMENT,
	SP_CONFIG_INDEX_OUT_OF_RANGE,
	SP_CONFIG_SUCCESS
} SP_CONFIG_MSG;

typedef struct sp_config_t* SPConfig;

/*
 * An enum represents the cut method when the kd-tree is build.
 * One of the following values {RANDOM, MAX_SPREAD, INCREMENTAL }. Default = MAX_SPREAD.
 */

typedef enum sp_kdtree_split_method_t{
	RANDOM,
	MAX_SPREAD,
	INCREMENTAL
} SP_KDTREE_SPLIT_METHOD;

/**
 * Creates a new system configuration struct. The configuration struct
 * is initialized based on the configuration file given by 'filename'.
 * 
 * @param filename - the name of the configuration file
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return NULL in case an error occurs. Otherwise, a pointer to a struct which
 * 		   contains all system configuration.
 * 
 * The resulting value stored in msg is as follow:
 * - SP_CONFIG_INVALID_ARGUMENT - if filename == NULL
 * - SP_CONFIG_CANNOT_OPEN_FILE - if the configuration file given by filename cannot be open
 * - SP_CONFIG_ALLOC_FAIL - if an allocation failure occurred
 * - SP_CONFIG_INVALID_INTEGER - if a line in the config file contains invalid integer
 * - SP_CONFIG_INVALID_STRING - if a line in the config file contains invalid string
 * - SP_CONFIG_MISSING_DIR - if spImagesDirectory is missing
 * - SP_CONFIG_MISSING_PREFIX - if spImagesPrefix is missing
 * - SP_CONFIG_MISSING_SUFFIX - if spImagesSuffix is missing 
 * - SP_CONFIG_MISSING_NUM_IMAGES - if spNumOfImages is missing
 * - SP_CONFIG_SUCCESS - in case of success
 *
 *
 */
SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg);

/*
 * Returns true if spExtractionMode = true, false otherwise.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return true if spExtractionMode = true, false otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
bool spConfigIsExtractionMode(const SPConfig config, SP_CONFIG_MSG* msg);

/*
 * Returns true if spMinimalGUI = true, false otherwise.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return true if spExtractionMode = true, false otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
bool spConfigMinimalGui(const SPConfig config, SP_CONFIG_MSG* msg);

/*
 * Returns the number of images set in the configuration file, i.e the value
 * of spNumOfImages.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
int spConfigGetNumOfImages(const SPConfig config, SP_CONFIG_MSG* msg);

/*
 * Returns the number of features to be extracted. i.e the value
 * of spNumOfFeatures.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
int spConfigGetNumOfFeatures(const SPConfig config, SP_CONFIG_MSG* msg);

/**
 * Returns the dimension of the PCA. i.e the value of spPCADimension.
 *
 * @param config - the configuration structure
 * @assert msg != NULL
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
 */
int spConfigGetPCADim(const SPConfig config, SP_CONFIG_MSG* msg);

/**
 * Given an index 'index' the function stores in imagePath the full path of the
 * ith image.
 *
 * For example:
 * Given that the value of:
 *  spImagesDirectory = "./images/"
 *  spImagesPrefix = "img"
 *  spImagesSuffix = ".png"
 *  spNumOfImages = 17
 *  index = 10
 *
 * The functions stores "./images/img10.png" to the address given by imagePath.
 * Thus the address given by imagePath must contain enough space to
 * store the resulting string.
 *
 * @param imagePath - an address to store the result in, it must contain enough space.
 * @param config - the configuration structure
 * @param index - the index of the image.
 *
 * @return
 * - SP_CONFIG_INVALID_ARGUMENT - if imagePath == NULL or config == NULL
 * - SP_CONFIG_INDEX_OUT_OF_RANGE - if index >= spNumOfImages or index < 0
 * - SP_CONFIG_SUCCESS - in case of success
 */
SP_CONFIG_MSG spConfigGetImagePath(char* imagePath, const SPConfig config,
		int index);

/**
 * The function stores in pcaPath the full path of the pca file.
 * For example given the values of:
 *  spImagesDirectory = "./images/"
 *  spPcaFilename = "pca.yml"
 *
 * The functions stores "./images/pca.yml" to the address given by pcaPath.
 * Thus the address given by pcaPath must contain enough space to
 * store the resulting string.
 *
 * @param imagePath - an address to store the result in, it must contain enough space.
 * @param config - the configuration structure
 * @return
 *  - SP_CONFIG_INVALID_ARGUMENT - if pcaPath == NULL or config == NULL
 *  - SP_CONFIG_SUCCESS - in case of success
 */
SP_CONFIG_MSG spConfigGetPCAPath(char* pcaPath, const SPConfig config);

/**
 * Frees all memory resources associate with config. 
 * If config == NULL nothig is done.
 */
void spConfigDestroy(SPConfig config);


/**
 * Gets config struct and updates the key field with value.
 * Check that constraints are met before doing so.
 * @param config - the configuration structure
 * @param key - field to update
 * @param value - field's new content
 * @param i - number of tokens in line. Used to determine line validity
 *
 * @return
 * 	- SP_CONFIG_SUCCESS - in case of success
 * 	- SP_CONFIG_INVALID_INTEGER - integer constraint not met
 * 	- SP_CONFIG_INVALID_STRING - string constraint not met
 * 	- SP_CONFIG_INVALID_ARGUMENT - invalid line
 * 	- SP_CONFIG_ALLOC_FAIL - in case of memory allocation failure
 */
SP_CONFIG_MSG update_config_param(SPConfig config, char* key, char* value, int i);

/**
 * Gets a config structure and checks whether the fields without default value have values.
 * @return
 * 	field key if has no value:
 * 		1 for spImagesDirectory
 * 		2 for spImagesPrefix
 * 		3 for spImagesSuffix
 * 		4 for spNumOfImages
 * 	0 in case of all fields have value
 */
int check_config_values(SPConfig config);

/*
 * this method recieves a string representing a line in a text file and checks if it contains only
 * whitespaces such as  \r\n\t\v\f or ' '.
 *
 * @return
 * true - the line contains only whitespaces
 * false - otherwhise
 */
bool IsEmptyLine(char line[MAX_LINE_LEN]);

/*
 * This method is used in detecting a line which starts in zero or more whitespaces followed by
 * '=' character.
 * this case will can be recognized by strtok function as valid config line.
 *
 * @param
 * line - a line in the configuration file
 *
 * @return
 * true - if the line is as described above
 * false- otherwise
 */
bool is_whitespace_and_equal(char line[MAX_LINE_LEN]);


/*
 * this method receives a configuration file, an image index and a pointer that will store the
 * resulting path string
 *
 * @param featsPath - an address to store the result in, it must contain enough space.
 * @param config - the configuration structure
 * @param index - the index of the image.
 *
 * @return
 * - SP_CONFIG_INVALID_ARGUMENT - if featsPath == NULL or config == NULL
 * - SP_CONFIG_INDEX_OUT_OF_RANGE - if index >= spNumOfImages or index < 0
 * - SP_CONFIG_SUCCESS - in case of success
 *
 */

SP_CONFIG_MSG spConfigGetFeatsPath(char* featsPath, const SPConfig config, int index);

/*
 * Returns the number of similar images set in the configuration file, i.e the value
 * of spNumOfSimilarImages.
 *
 * @param config - the configuration structure
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
*/
int SPConfigGetNumOfSimilarImages(SPConfig config, SP_CONFIG_MSG* msg);

/*
 * The function stores in loger_filename the full path of the logger file.

 *
 * The functions stores a path to a file to the address given by logger_filename.
 * Thus this address must contain enough space to store the resulting string.
 *
 * @param logger_filename - an address to store the result in, it must contain enough space.
 * @param config - the configuration structure
 * @return
 *  - SP_CONFIG_INVALID_ARGUMENT - if logger_filename == NULL or config == NULL
 *  - SP_CONFIG_SUCCESS - in case of success
 */
SP_CONFIG_MSG spConfigGetLoggerFilename(char* logger_filename, const SPConfig config);

/*
 *returns the logger level according to the configurations file
 * @param config - the configuration structure
 * @param msg - pointer in which the msg returned by the function is stored
 *
 * @return
 *
 * - SP_CONFIG_SUCCESS - in case of success
 * - SP_CONFIG_ALLOC_FAIL - in case of memory allocation failure
 */
SP_LOGGER_LEVEL SPConfigGetLoggerLevel(SPConfig config, SP_CONFIG_MSG* msg);

/*
 *returns the KDTree splitting method according to the configurations file
 * @param config - the configuration structure
 * @param msg - pointer in which the msg returned by the function is stored
 *
 * @return
 *
 * - SP_CONFIG_SUCCESS - in case of success
 * - SP_CONFIG_ALLOC_FAIL - in case of memory allocation failure
 */
SP_KDTREE_SPLIT_METHOD SPConfigGetSplitMethod(SPConfig config, SP_CONFIG_MSG* msg);

/*
 * Returns the spKNN set in the configuration file, i.e the value
 * of spNumOfSimilarImages.
 *
 * @param config - the configuration structure
 * @param msg - pointer in which the msg returned by the function is stored
 * @return positive integer in success, negative integer otherwise.
 *
 * - SP_CONFIG_INVALID_ARGUMENT - if config == NULL
 * - SP_CONFIG_SUCCESS - in case of success
*/
int SPConfigGetSpknn(SPConfig config, SP_CONFIG_MSG* msg);

#endif /* SPCONFIG_H_ */
