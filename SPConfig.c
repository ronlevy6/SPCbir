#include "SPConfig.h"
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>



/*
 * An enum represents the cut method when the kd-tree is build.
 * One of the following values {RANDOM, MAX_SPREAD, INCREMENTAL }. Default = MAX_SPREAD.
 */
/*
typedef enum sp_kdtree_split_method_t{
	RANDOM,
	MAX_SPREAD,
	INCREMENTAL
} SP_KDTREE_SPLIT_METHOD;

*/

/*
 * Struct sp_config_t keeps all data from config file.
 * Each field corresponds to the system variable written in the instruction file
 * Fields with default value will be initialized at struct creation
 * The fields are:
 *
 * spImagesDirectory - path of the images’ directory. Doesn't contain spaces.
 * spImagesPrefix - the images prefix. Doesn't contain spaces.
 * spImagesSuffix - the images file format. One of the following - .jpg , .png , .bmp , .gif
 * spNumOfImages - number of images. Positive integer.
 *
 * spPCADimension - number of reduced dimensions. Integer from 10 to 28. Default = 20
 * spPCAFilename - filename of the PCA file. Doesn't contain spaces. Default = pca.yml
 *
 * spNumOfFeatures - number of features which will be extracted per image. A positive integer. Default = 100
 * spExtractionMode - boolean which indicates if preprocessing must be done first or if the features are extracted from files.
 * 					  Default = true
 * spNumOfSimilarImages - number of similar images which will be presented given a query image. A positive integer. Default = 1.
 * spKDTreeSplitMethod - SP_KDTREE_SPLIT_METHOD enum
 * spKNN - used in the k nearest neighbor search algorithm. A positive integer. Default = 1.
 *
 * spMinimalGUI - indicates if minimal GUI is supported, Boolean. default = False.
 * spLoggerLevel - The value 1 indicates error level, the value 2 indicates warning level, the value 3 indicates
 * 			       info level and 4 indicates debug level. Default = 3.
 * spLoggerFilename - log file name. Doesn't contain spaces. Default = stdout.
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

SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg){
	FILE * fp = NULL;
	int line_cnt = 1, i = 0,is_valid;
	char line[MAX_LINE_LEN];
	char * key, * value,*pch;
	SP_CONFIG_MSG update_msg;
	bool has_equal = false;

	//printf("entered spConfigCreate\n");

	// validate inputs
	assert (msg != NULL);
	if (filename == NULL){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return NULL;
	}

	SPConfig config = (SPConfig)malloc(sizeof(*config));
	if (config == NULL){
		*msg = SP_CONFIG_ALLOC_FAIL;
		return NULL;
	}
	// open file
	fp = fopen(filename, "r");
	if (fp == NULL){
		*msg = SP_CONFIG_CANNOT_OPEN_FILE;
		free(config);	// free all previous allocation
		return NULL;
	}

	// initialize the struct default values
	config->spPCADimension = 20;
	config->spNumOfFeatures = 100;
	config->spExtractionMode = true;
	config->spMinimalGUI = false;
	config->spNumOfSimilarImages = 1;
	config->spKNN = 1;
	config->spKDTreeSplitMethod = MAX_SPREAD;
	config->spLoggerLevel = SP_LOGGER_INFO_WARNING_ERROR_LEVEL;


	config->spPCAFilename = (char *)malloc(sizeof(char)*MAX_LINE_LEN);

	if (config->spPCAFilename == NULL){
		*msg =SP_CONFIG_CANNOT_OPEN_FILE;
		free(config);	// free all previous allocation
		fclose(fp);
		return NULL;

	}
	// set default value of spPCAFilename to pca.yml
	config->spPCAFilename = "pca.yml";
	config->spLoggerFilename = (char *)malloc(sizeof(char)*MAX_LINE_LEN);

	if (config->spLoggerFilename == NULL){
		*msg = SP_CONFIG_CANNOT_OPEN_FILE; //TO DO!!! CHECK IF & IS NEEDED
		free(config->spPCAFilename);
		free(config);	// free all previous allocation
		fclose(fp);
		return NULL;
	}

	//set default value of spLoggerFilename to stdout
	config->spLoggerFilename = "stdout";

	// assign NULL to all other fields:
	config->spImagesDirectory = NULL;
	config->spImagesPrefix = NULL;
	config->spImagesSuffix = NULL;
	config->spNumOfImages = -1;

	//######################################################################################
	//######################################################################################
	//######################################################################################

	// load data from fp

	while (fgets(line, MAX_LINE_LEN, fp) != NULL){
		//printf("line = %s len = %d\n", line, strlen(line));
		fflush(NULL);
		if (!IsEmptyLine(line)){


			if (strchr(line,'=') == NULL){// line has no '='
				has_equal = false;
			}
			else{
				has_equal = true;
			}
			pch = strtok(line, "= \r\n\t\v\f"); // split line using whitespace and "=" as delimiters
			if (*pch != '#'){ // Line is not comment
				if (!has_equal){// invalid line - no '=' in a not comment line
					printf("File: %s\nLine: %d\nMessage: Invalid configuration line\n",filename,line_cnt);
					*msg = SP_CONFIG_INVALID_STRING;
					spConfigDestroy(config); // free all previous allocation
					fclose(fp);
					return NULL;
				}

				i = 0;
				// indicator for token number - first is key, second is value.
				// In case there are more than 2 tokens the line is invalid

				value = NULL;
				while (pch != NULL){
					if (i == 0){
						key = pch;
					}
					if (i == 1){
						value = pch;
					}
					pch = strtok (NULL, "= \r\n\t\v\f");
					i++;
				}
				//printf("key = %s, val = %s\n", key, value);
				//fflush(NULL);

				if (i != 0){ // indicator that this is not a empty line
					// update value for current key
					update_msg = update_config_param(config,key,value,i);
					/*
					printf("right after exiting!!!\n");
					printf("spImagesDirectory %s\n", config->spImagesDirectory);
					printf("spImagesPrefix %s\n", config->spImagesPrefix);
					printf("spImagesSuffix %s\n", config->spImagesSuffix);
					printf("spNumOfImages %d\n", config->spNumOfImages);
					printf("spPCADimension %d\n", config->spPCADimension);
					printf("spPCAFilename %s\n", config->spPCAFilename);
					printf("spNumOfFeatures %d\n", config->spNumOfFeatures);
					printf("spExtractionMode %d\n", config->spExtractionMode);
					printf("spNumOfSimilarImages %d\n", config->spNumOfSimilarImages);
					printf("spKDTreeSplitMethod %d\n", config->spKDTreeSplitMethod);
					printf("spKNN %d\n", config->spKNN);
					printf("spMinimalGUI %d\n", config->spMinimalGUI);
					printf("spLoggerLevel %d\n", config->spLoggerLevel);
					printf("spLoggerFilename %s\n\n\n\n", config->spLoggerFilename);
					*/
					if (update_msg == SP_CONFIG_INVALID_ARGUMENT){
						printf("File: %s\nLine: %d\nMessage: Invalid configuration line\n",filename,line_cnt);
						*msg = SP_CONFIG_INVALID_STRING;
						spConfigDestroy(config); // free all previous allocation
						fclose(fp);
						return NULL;
					}
					else if (update_msg == SP_CONFIG_INVALID_STRING){
						printf("File: %s\nLine: %d\nMessage: Invalid value - constraint not met\n",filename,line_cnt);
						*msg = SP_CONFIG_INVALID_STRING;
						spConfigDestroy(config); // free all previous allocation
						fclose(fp);
						return NULL;
					}
					else if (update_msg == SP_CONFIG_INVALID_INTEGER){
						printf("File: %s\nLine: %d\nMessage: Invalid value - constraint not met\n",filename,line_cnt);
						*msg = SP_CONFIG_INVALID_INTEGER;
						spConfigDestroy(config); // free all previous allocation
						fclose(fp);
						return NULL;
					}
					else if (update_msg == SP_CONFIG_ALLOC_FAIL){
						printf("File: %s\nLine: %d\nMessage: Allocation Failure",filename,line_cnt);
						*msg = SP_CONFIG_ALLOC_FAIL;
						spConfigDestroy(config); // free all previous allocation
						fclose(fp);
						return NULL;
					}
				}
			}
		}
		line_cnt++;
	}// end of outer while

	// check all fields have values
	is_valid = check_config_values(config);

	if (is_valid != 0){
		if (is_valid == 1){
			printf("File: %s\nLine: %d\nMessage: Parameter spImagesDirectory is not set\n",filename,line_cnt);
			*msg = SP_CONFIG_MISSING_DIR;
		}
		if (is_valid == 2){
			printf("File: %s\nLine: %d\nMessage: Parameter spImagesPrefix is not set\n",filename,line_cnt);
			*msg = SP_CONFIG_MISSING_PREFIX;
		}
		if (is_valid == 3){
			printf("File: %s\nLine: %d\nMessage: Parameter spImagesSuffix is not set\n",filename,line_cnt);
			*msg = SP_CONFIG_MISSING_SUFFIX;
		}
		if (is_valid == 4){
			printf("File: %s\nLine: %d\nMessage: Parameter spNumOfImages is not set\n",filename,line_cnt);
			*msg = SP_CONFIG_MISSING_NUM_IMAGES;
		}
		spConfigDestroy(config);
		fclose(fp);
		return NULL;
	}

	// printing all fields assigned

	//printf("finishied!!!\n");
	//fflush(NULL);
	/*
	printf("spImagesDirectory %s\n", config->spImagesDirectory);
	printf("spImagesPrefix %s\n", config->spImagesPrefix);
	printf("spImagesSuffix %s\n", config->spImagesSuffix);
	printf("spNumOfImages %d\n", config->spNumOfImages);
	printf("spPCADimension %d\n", config->spPCADimension);
	printf("spPCAFilename %s\n", config->spPCAFilename);
	printf("spNumOfFeatures %d\n", config->spNumOfFeatures);
	printf("spExtractionMode %d\n", config->spExtractionMode);
	printf("spNumOfSimilarImages %d\n", config->spNumOfSimilarImages);
	printf("spKDTreeSplitMethod %d\n", config->spKDTreeSplitMethod);
	printf("spKNN %d\n", config->spKNN);
	printf("spMinimalGUI %d\n", config->spMinimalGUI);
	printf("spLoggerLevel %d\n", config->spLoggerLevel);
	printf("spLoggerFilename %s\n", config->spLoggerFilename);
	*/

	*msg = SP_CONFIG_SUCCESS;
	return config;
}





SP_CONFIG_MSG update_config_param(SPConfig config, char* key, char* value, int i){

	bool valid_whitespaces = true; // indicator for number of whitespaces in line according to strtok
	int int_value; // conversion of value to integer


	if (value == NULL){
		return SP_CONFIG_INVALID_ARGUMENT;
	}

	if (i > 2){ // if i > 2 we have too much whitespaces
		valid_whitespaces = false;
	}
	// find the field according to the key
	if (strcmp(key,"spImagesDirectory") == 0){
		if (valid_whitespaces){ // line is valid
			config->spImagesDirectory = (char*)malloc(strlen(value) + 1);
			if (config->spImagesDirectory == NULL){
				return SP_CONFIG_ALLOC_FAIL;
			}
			strncpy(config->spImagesDirectory,value,strlen(value));
			*(config->spImagesDirectory + strlen(value)) = '\0';
			//printf("dir in func = %s\n",config->spImagesDirectory);
			return SP_CONFIG_SUCCESS;
		}
		else{ // too many whitespaces
			return SP_CONFIG_INVALID_ARGUMENT;
		}
	}

	else if (strcmp(key,"spImagesPrefix") == 0){
		if (valid_whitespaces){ // line is valid
			config->spImagesPrefix = (char*)malloc(strlen(value) + 1);
			if (config->spImagesPrefix == NULL){
				return SP_CONFIG_ALLOC_FAIL;
			}

			strncpy(config->spImagesPrefix,value,strlen(value));
			*(config->spImagesPrefix + strlen(value)) = '\0';
			//printf("prefix in func = %s\n",config->spImagesPrefix);
			return SP_CONFIG_SUCCESS;
		}
		else{ // value is invalid - too many whitespaces
			return SP_CONFIG_INVALID_ARGUMENT;
		}
	}

	else if (strcmp(key,"spImagesSuffix") == 0){
		if (valid_whitespaces){ // line is valid
			// check values constraints
			if (strcmp(value,".jpg") != 0 && strcmp(value,".png") != 0 &&
					strcmp(value,".bmp") != 0 && strcmp(value,".gif") != 0 ){
				return SP_CONFIG_INVALID_STRING;
			}
			else{ // value is valid
				config->spImagesSuffix = (char*)malloc(strlen(value) + 1);
				if (config->spImagesSuffix == NULL){
					return SP_CONFIG_ALLOC_FAIL;
				}
				strncpy(config->spImagesSuffix,value,strlen(value));
				*(config->spImagesSuffix + strlen(value)) = '\0';
				//printf("suffix in func = %s\n",config->spImagesSuffix);
				return SP_CONFIG_SUCCESS;
			}
		}
		else{ // too many whitespaces
			return SP_CONFIG_INVALID_ARGUMENT;
		}
	}

	else if (strcmp(key,"spNumOfImages") == 0){
		if (valid_whitespaces){ // line is valid
			int_value = atoi(value);
			if (int_value <= 0){ // constraint not met
				return SP_CONFIG_INVALID_INTEGER;
			}
			config->spNumOfImages = int_value;
			return SP_CONFIG_SUCCESS;
		}
		else{ // too many whitespaces
			return SP_CONFIG_INVALID_ARGUMENT;
		}
	}

	else if (strcmp(key,"spPCADimension") == 0){
		if (valid_whitespaces){ // line is valid
			int_value = atoi(value);
			if (int_value < 10 || int_value > 28){ // constraint not met
				return SP_CONFIG_INVALID_INTEGER;
			}
			config->spPCADimension = int_value;
			return SP_CONFIG_SUCCESS;
		}
		else{// too many whitespaces
			return SP_CONFIG_INVALID_ARGUMENT;
		}
	}

	else if (strcmp(key,"spPCAFilename") == 0){
		if (valid_whitespaces){ // line is valid
			// no need in memory allocation - was already allocated when assigned default value
			free(config->spPCAFilename);
			config->spPCAFilename = (char*)malloc(strlen(value) + 1);
			if (config->spPCAFilename == NULL){
				return SP_CONFIG_ALLOC_FAIL;
			}
			strncpy(config->spPCAFilename,value,strlen(value));
			*(config->spPCAFilename + strlen(value)) = '\0';
			//printf("pca filename in func = %s\n", config->spPCAFilename);
			return SP_CONFIG_SUCCESS;
		}
		else{ // value is invalid - too many whitespaces
			return SP_CONFIG_INVALID_ARGUMENT;
		}
	}

	else if (strcmp(key,"spNumOfFeatures") == 0){
		if (valid_whitespaces){ // line is valid
			int_value = atoi(value);
			if (int_value <= 0){ // constraint not met
				return SP_CONFIG_INVALID_INTEGER;
			}
			config->spNumOfFeatures = int_value;
			return SP_CONFIG_SUCCESS;
		}
		else{ // too many whitespaces
			return SP_CONFIG_INVALID_ARGUMENT;
		}
	}

	else if (strcmp(key,"spExtractionMode") == 0){
		if (valid_whitespaces){
			if (strcmp(value,"true") == 0){
				config->spExtractionMode = true;
				return SP_CONFIG_SUCCESS;
			}
			else if (strcmp(value,"false") == 0){
				config->spExtractionMode = false;
				return SP_CONFIG_SUCCESS;
			}
			else{
				return SP_CONFIG_INVALID_STRING;
			}
		}
		else{// too many whitespaces
			return SP_CONFIG_INVALID_ARGUMENT;
		}
	}

	else if (strcmp(key,"spNumOfSimilarImages") == 0){
		if (valid_whitespaces){ // line is valid
			int_value = atoi(value);
			if (int_value <= 0){ // constraint not met
				return SP_CONFIG_INVALID_INTEGER;
			}
			config->spNumOfSimilarImages = int_value;
			return SP_CONFIG_SUCCESS;
		}
		else{ // too many whitespaces
			return SP_CONFIG_INVALID_ARGUMENT;
		}
	}

	else if (strcmp(key,"spKDTreeSplitMethod") == 0){
		if (valid_whitespaces){ // line is valid
			if (strcmp(value,"RANDOM") == 0){
				config->spKDTreeSplitMethod = RANDOM;
				return SP_CONFIG_SUCCESS;
			}
			else if (strcmp(value,"MAX_SPREAD") == 0){
				config->spKDTreeSplitMethod = MAX_SPREAD;
				return SP_CONFIG_SUCCESS;
			}
			else if (strcmp(value,"INCREMENTAL") == 0){
				config->spKDTreeSplitMethod = INCREMENTAL;
				return SP_CONFIG_SUCCESS;
			}
			else { //different value
				return SP_CONFIG_INVALID_STRING;
			}
		}
		else { // too many whitespaces
			return SP_CONFIG_INVALID_ARGUMENT;
		}
	}

	else if (strcmp(key,"spKNN") == 0){
		if (valid_whitespaces){ // line is valid
			int_value = atoi(value);
			if (int_value <= 0){ // constraint not met
				return SP_CONFIG_INVALID_INTEGER;
			}
			config->spKNN = int_value;
			return SP_CONFIG_SUCCESS;
		}
		else{ // too many whitespaces
			return SP_CONFIG_INVALID_ARGUMENT;
		}
	}

	else if (strcmp(key,"spMinimalGUI") == 0){
		if (valid_whitespaces){ // line is valid
			if (strcmp(value,"false") == 0){
				config->spMinimalGUI = false;
				return SP_CONFIG_SUCCESS;
			}
			else if (strcmp(value,"true") == 0){
				config->spMinimalGUI = true;
				return SP_CONFIG_SUCCESS;
			}
			else{ // constraints not met
				return SP_CONFIG_INVALID_STRING;
			}
		}
		else {// too many whitespaces
			return SP_CONFIG_INVALID_ARGUMENT;
		}
	}

	else if (strcmp(key,"spLoggerLevel") == 0){
		if (valid_whitespaces){ // line is valid
			int_value = atoi(value);
			if (int_value == 1){
				config->spLoggerLevel = SP_LOGGER_ERROR_LEVEL;
				return SP_CONFIG_SUCCESS;
			}
			else if (int_value == 2){
				config->spLoggerLevel = SP_LOGGER_WARNING_ERROR_LEVEL;
				return SP_CONFIG_SUCCESS;
			}
			else if (int_value == 3){
				config->spLoggerLevel = SP_LOGGER_INFO_WARNING_ERROR_LEVEL;
				return SP_CONFIG_SUCCESS;
			}
			else if (int_value == 4){
				config->spLoggerLevel = SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL;
				return SP_CONFIG_SUCCESS;
			}
			else{ // invalid value
				return SP_CONFIG_INVALID_INTEGER;
			}
		}
		else { // invalid line - too many whitespaces
			return SP_CONFIG_INVALID_ARGUMENT;
		}
	}

	else if (strcmp(key, "spLoggerFilename") == 0){
		if (valid_whitespaces){ // line is valid
			free(config->spLoggerFilename);
			config->spLoggerFilename = (char*)malloc(strlen(value) + 1);
			if (config->spLoggerFilename == NULL){
				return SP_CONFIG_ALLOC_FAIL;
			}
			strncpy(config->spLoggerFilename,value,strlen(value));
			*(config->spLoggerFilename + strlen(value)) = '\0';
			return SP_CONFIG_SUCCESS;
		}
		else{ // value is invalid - too many whitespaces
			return SP_CONFIG_INVALID_ARGUMENT;
		}
	}

	else{ // key is none of the fields
		return SP_CONFIG_INVALID_ARGUMENT;
	}
}

int check_config_values(SPConfig config){
	if (config->spImagesDirectory == NULL){
		return 1;
	}
	if (config->spImagesPrefix == NULL){
		return 2;
	}
	if (config->spImagesSuffix == NULL){
		return 3;
	}
	if (config->spNumOfImages == -1){
		return 4;
	}
	return 0;
}


bool spConfigIsExtractionMode(const SPConfig config, SP_CONFIG_MSG* msg){
	assert(msg != NULL);

	if (config == NULL){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return false;
	}
	// valid input
	*msg = SP_CONFIG_SUCCESS;
	return config->spExtractionMode;
}

bool spConfigMinimalGui(const SPConfig config, SP_CONFIG_MSG* msg){
	assert(msg != NULL);

	if (config == NULL){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return false;
	}
	// valid input
	*msg = SP_CONFIG_SUCCESS;
	return config->spMinimalGUI;
}

int spConfigGetNumOfImages(const SPConfig config, SP_CONFIG_MSG* msg){
	assert(msg != NULL);

	if (config == NULL){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spNumOfImages;
}

int spConfigGetNumOfFeatures(const SPConfig config, SP_CONFIG_MSG* msg){
	assert(msg != NULL);

	if (config == NULL){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spNumOfFeatures;
}

int spConfigGetPCADim(const SPConfig config, SP_CONFIG_MSG* msg){
	assert(msg != NULL);

	if (config == NULL){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spPCADimension;
}

SP_CONFIG_MSG spConfigGetImagePath(char* imagePath, const SPConfig config, int index){
	char curr_img[MAX_LINE_LEN];

	//printf("before all\n");
	fflush(NULL);
	// check input validity
	if (imagePath == NULL || config == NULL){
		return SP_CONFIG_INVALID_ARGUMENT;
	}
	if (index >= config->spNumOfImages || index < 0){
		return SP_CONFIG_INDEX_OUT_OF_RANGE;
	}


	// concatenate fields into one path and store it in imagePath (will be malloced before call)
	memset(imagePath, 0x00, MAX_LINE_LEN); // clean memory on stack
	//printf("before dir %s\n",imagePath);
	fflush(NULL);
	strncpy(imagePath, config->spImagesDirectory, strlen(config->spImagesDirectory));
	//printf("after dir %s\n",imagePath);
	fflush(NULL);
	strncat(imagePath, config->spImagesPrefix, strlen(config->spImagesPrefix));
	//printf("after pref %s\n",imagePath);
	fflush(NULL);
	sprintf(curr_img, "%d", index);
	//printf("after sprintf curr_img = %s\n",curr_img);
	strncat(imagePath, curr_img, strlen(curr_img));	// check if need to change to strlen() -1
	//printf("after idx %s\n",imagePath);
	fflush(NULL);
	strncat(imagePath, config->spImagesSuffix, strlen(config->spImagesSuffix));
	//printf("after suffix %s\n",imagePath);
	fflush(NULL);
	imagePath[strlen(imagePath)] = '\0'; // to keep imagePath a valid string
	//printf("final %s\n",imagePath);
	fflush(NULL);

	return SP_CONFIG_SUCCESS;
}

SP_CONFIG_MSG spConfigGetFeatsPath(char* featsPath, const SPConfig config, int index){
	char curr_img[MAX_LINE_LEN];

	// check input validity
	if (featsPath == NULL || config == NULL){
		return SP_CONFIG_INVALID_ARGUMENT;
	}
	if (index >= config->spNumOfImages || index < 0){
		return SP_CONFIG_INDEX_OUT_OF_RANGE;
	}


	// concatenate fields into one path and store it in imagePath (will be malloced before call)
	memset(featsPath, 0x00, MAX_LINE_LEN); // clean memory on stack
	strncpy(featsPath, config->spImagesDirectory, strlen(config->spImagesDirectory));
	strncat(featsPath, config->spImagesPrefix, strlen(config->spImagesPrefix));
	sprintf(curr_img, "%d", index);
	strncat(featsPath, curr_img, strlen(curr_img));
	strncat(featsPath, ".feats", strlen(".feats"));
	featsPath[strlen(featsPath)] = '\0'; // to keep imagePath a valid string

	return SP_CONFIG_SUCCESS;

}

SP_CONFIG_MSG spConfigGetPCAPath(char* pcaPath, const SPConfig config){

	if (pcaPath == NULL ||config == NULL){
		return SP_CONFIG_INVALID_ARGUMENT;
	}
	// concatenate fields into one path and store it in imagePath (will be malloced before call)
	memset(pcaPath, 0x00, MAX_LINE_LEN); // clean memory on stack
	strncpy(pcaPath, config->spImagesDirectory, strlen(config->spImagesDirectory));
	strncat(pcaPath, config->spPCAFilename, strlen(config->spPCAFilename));
	pcaPath[strlen(pcaPath)] = '\0';

	return SP_CONFIG_SUCCESS;
}

void spConfigDestroy(SPConfig config){
	// this is generic destroy which is used to free previous allocations
	if (config == NULL){
		return;
	}

	if (config->spImagesDirectory != NULL){
		free(config->spImagesDirectory);
	}
	if (config->spImagesPrefix != NULL){
		free(config->spImagesPrefix);
	}
	if (config->spImagesSuffix != NULL){
		free(config->spImagesSuffix);
	}
	if (config->spLoggerFilename != NULL){
		free(config->spLoggerFilename);
	}
	if (config->spPCAFilename != NULL){
		free(config->spPCAFilename);
	}

	// free config
	free(config);
}

bool IsEmptyLine(char line[MAX_LINE_LEN]){

	for(int i = 0; i< strlen(line); i++){
		if(line[i] != ' ' && line[i] != '\r' && line[i] != '\n' && line[i] != '\t' && line[i] != '\f'
				&& line[i] != '\v'){
			return false;
		}
	}
	return true;
}

int SPConfigGetNumOfSimilarImages(SPConfig config, SP_CONFIG_MSG* msg){

	if (config == NULL){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spNumOfSimilarImages;
}

char* SPConfigGetLoggerFilename(SPConfig config, SP_CONFIG_MSG* msg){

	char *result;

	if (config == NULL){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return NULL;
	}
	result = (char *)malloc(sizeof(char) * strlen(config->spLoggerFilename));
	if (result == NULL){
		*msg = SP_CONFIG_ALLOC_FAIL;
		return NULL;
	}
	*msg = SP_CONFIG_SUCCESS;
	return result;
}

SP_LOGGER_LEVEL SPConfigGetLoggerLevel(SPConfig config, SP_CONFIG_MSG* msg){

	if (config == NULL){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spLoggerLevel;
}

SP_KDTREE_SPLIT_METHOD SPConfigGetSplitMethod(SPConfig config, SP_CONFIG_MSG* msg){

	if (config == NULL){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spKDTreeSplitMethod;
}

int SPConfigGetSpknn(SPConfig config, SP_CONFIG_MSG* msg){

	if (config == NULL){
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spKNN;
}

