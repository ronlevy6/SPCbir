#include "../SPConfig.h"
#include "../SPLogger.h"
#include "../SPPoint.h"
#include "unit_test_util.h"
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../main_aux.h"

#define EPS 0.001
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


bool dequal(double a, double b, double epsilon){
	return fabs(a-b) < epsilon;
}


bool readFeaturesFromFileTest(){
	SP_CONFIG_MSG msg;
	SPConfig config;
	SPPoint *res_point, point;
	double file_data[60];
	int cnt = 0, cnt2;

	config = spConfigCreate("preprocess_config.txt", &msg);
	ASSERT_TRUE(msg == 10);
	//res_point = readFeaturesFromFile(config);
	res_point = readFeaturesFromFile(config, &cnt2);

	for(int i = 0; i < 5; i++){
		point = res_point[i];
		for (int j = 0; j < spPointGetDimension(point); j++){
			file_data[cnt] = spPointGetAxisCoor(point, j);
			cnt++;
		}
	}
	ASSERT_TRUE(cnt == 52);
	ASSERT_TRUE(cnt2 == 5);


	ASSERT_TRUE(dequal(file_data[0],1.2,EPS));
	ASSERT_TRUE(dequal(file_data[1],3.7,EPS));
	ASSERT_TRUE(dequal(file_data[2],18.3,EPS));
	ASSERT_TRUE(dequal(file_data[3],1.0,EPS));
	ASSERT_TRUE(dequal(file_data[4],2.2,EPS));
	ASSERT_TRUE(dequal(file_data[5],5.5,EPS));
	ASSERT_TRUE(dequal(file_data[6],6.8,EPS));
	ASSERT_TRUE(dequal(file_data[7],12.3,EPS));
	ASSERT_TRUE(dequal(file_data[8],0.0,EPS));
	ASSERT_TRUE(dequal(file_data[9],2.234,EPS));
	ASSERT_TRUE(dequal(file_data[10],1.23,EPS));
	ASSERT_TRUE(dequal(file_data[11],33.7,EPS));
	ASSERT_TRUE(dequal(file_data[12],1.3,EPS));
	ASSERT_TRUE(dequal(file_data[13],4.0,EPS));
	ASSERT_TRUE(dequal(file_data[14],2.2,EPS));
	ASSERT_TRUE(dequal(file_data[15],24.5,EPS));
	ASSERT_TRUE(dequal(file_data[16],16.18,EPS));
	ASSERT_TRUE(dequal(file_data[17],2.3,EPS));
	ASSERT_TRUE(dequal(file_data[18],0.01,EPS));
	ASSERT_TRUE(dequal(file_data[19],21.34,EPS));
	ASSERT_TRUE(dequal(file_data[20],11.2,EPS));
	ASSERT_TRUE(dequal(file_data[21],0.7,EPS));
	ASSERT_TRUE(dequal(file_data[22],55.3,EPS));
	ASSERT_TRUE(dequal(file_data[23],1.02,EPS));
	ASSERT_TRUE(dequal(file_data[24],1.2,EPS));
	ASSERT_TRUE(dequal(file_data[25],2.5,EPS));
	ASSERT_TRUE(dequal(file_data[26],9.8,EPS));
	ASSERT_TRUE(dequal(file_data[27],1.3,EPS));
	ASSERT_TRUE(dequal(file_data[28],7.3,EPS));
	ASSERT_TRUE(dequal(file_data[29],2.0,EPS));
	ASSERT_TRUE(dequal(file_data[30],1.2,EPS));
	ASSERT_TRUE(dequal(file_data[31],3.7,EPS));
	ASSERT_TRUE(dequal(file_data[32],18.3,EPS));
	ASSERT_TRUE(dequal(file_data[33],1.0,EPS));
	ASSERT_TRUE(dequal(file_data[34],2.2,EPS));
	ASSERT_TRUE(dequal(file_data[35],5.5,EPS));
	ASSERT_TRUE(dequal(file_data[36],6.8,EPS));
	ASSERT_TRUE(dequal(file_data[37],12.3,EPS));
	ASSERT_TRUE(dequal(file_data[38],0.0,EPS));
	ASSERT_TRUE(dequal(file_data[39],2.234,EPS));
	ASSERT_TRUE(dequal(file_data[40],11.2345,EPS));
	ASSERT_TRUE(dequal(file_data[41],1.2,EPS));
	ASSERT_TRUE(dequal(file_data[42],0.7,EPS));
	ASSERT_TRUE(dequal(file_data[43],55.3,EPS));
	ASSERT_TRUE(dequal(file_data[44],1.02,EPS));
	ASSERT_TRUE(dequal(file_data[45],1.2,EPS));
	ASSERT_TRUE(dequal(file_data[46],2.5,EPS));
	ASSERT_TRUE(dequal(file_data[47],9.8,EPS));
	ASSERT_TRUE(dequal(file_data[48],1.3,EPS));
	ASSERT_TRUE(dequal(file_data[49],7.3,EPS));
	ASSERT_TRUE(dequal(file_data[50],2.0,EPS));
	ASSERT_TRUE(dequal(file_data[51],12.343,EPS));

	spConfigDestroy(config);
	destroyPointsArray(res_point, 5);

	return true;
}
/*
 * The function writeFeaturesToFile moved to main.cpp
bool writeFeaturesToFileTest(){
	SP_CONFIG_MSG msg;
	SPConfig config;

	config = spConfigCreate("preprocess_config.txt", &msg);
	ASSERT_TRUE(msg == SP_CONFIG_SUCCESS);
	// write into a new file
	writeFeaturesToFile(config);

	// this test will be ran twice:
	// first run - create a new feats file
	// second run - replace the two images in the directory
	// compare two runs.
	return true;

}
*/

/*
int main(){
	// #######################################################################
	//both tests won't work together because the read files were created by us
	// #######################################################################
	RUN_TEST(readFeaturesFromFileTest);
	//RUN_TEST(writeFeaturesToFileTest);
}

*/
