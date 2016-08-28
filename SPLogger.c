#include "SPLogger.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

//File open mode
#define SP_LOGGER_OPEN_MODE "w"

// Global variable holding the logger
SPLogger logger = NULL;

SP_LOGGER_MSG print_data(const char* title,const char* msg, const char* file,
		const char* function, const int line);


struct sp_logger_t {
	FILE* outputChannel; //The logger file
	bool isStdOut; //Indicates if the logger is stdout
	SP_LOGGER_LEVEL level; //Indicates the level
};

SP_LOGGER_MSG spLoggerCreate(const char* filename, SP_LOGGER_LEVEL level) {
	if (logger != NULL) { //Already defined
		return SP_LOGGER_DEFINED;
	}
	logger = (SPLogger) malloc(sizeof(*logger));
	if (logger == NULL) { //Allocation failure
		return SP_LOGGER_OUT_OF_MEMORY;
	}
	logger->level = level; //Set the level of the logger
	if (filename == NULL) { //In case the filename is not set use stdout
		logger->outputChannel = stdout;
		logger->isStdOut = true;
	} else { //Otherwise open the file in write mode
		logger->outputChannel = fopen(filename, SP_LOGGER_OPEN_MODE);
		if (logger->outputChannel == NULL) { //Open failed
			free(logger);
			logger = NULL;
			return SP_LOGGER_CANNOT_OPEN_FILE;
		}
		logger->isStdOut = false;
	}
	return SP_LOGGER_SUCCESS;
}

void spLoggerDestroy() {
	if (!logger) {
		return;
	}
	if (!logger->isStdOut) {//Close file only if not stdout
		fclose(logger->outputChannel);
	}
	free(logger);//free allocation
	logger = NULL;
}

/*
 * This method returns SP_LOGGER_LEVEL of the program
 * in order to determine if massage should be printed *
 */
SP_LOGGER_LEVEL get_logger_level(){
	SP_LOGGER_LEVEL curr_level = logger->level;
	return curr_level;
}


/*
 * This method prints error massages. More information can be found in the header file.
 * The logging level check is done before calling the function
 */
SP_LOGGER_MSG spLoggerPrintError(const char* msg, const char* file,
		const char* function, const int line){
	int log_level;

	// Checking the logger is defined
	if (logger == NULL){
		return SP_LOGGER_UNDIFINED;
	}
	// Check log level to know if needs to print log
	log_level = get_logger_level();

	if (log_level >= SP_LOGGER_ERROR_LEVEL){
		SP_LOGGER_MSG print_res = print_data("---ERROR---", msg, file, function,line);
		return print_res;
	}
	else{
		//should not write to log
		return SP_LOGGER_SUCCESS;
	}
}

/*
 * This method prints warning massages. More information can be found in the header file.
 * The logging level check is done before calling the function
 */
SP_LOGGER_MSG spLoggerPrintWarning(const char* msg, const char* file,
		const char* function, const int line){
	SP_LOGGER_LEVEL log_level;

	// Checking the logger is defined
	if (logger == NULL){
		return SP_LOGGER_UNDIFINED;
	}

	// Check log level to know if needs to print log
	log_level = get_logger_level();

	if (log_level >= SP_LOGGER_WARNING_ERROR_LEVEL){
		SP_LOGGER_MSG print_res = print_data("---WARNING---",msg,file,function,line);
		return print_res;
	}
	else{
		//should not write the log
		return SP_LOGGER_SUCCESS; // Need to see what to do with it!!!!!!!!
	}
}

/*
 * This method prints debug massages. More information can be found in the header file.
 * The logging level check is done before calling the function
 */
SP_LOGGER_MSG spLoggerPrintDebug(const char* msg, const char* file,
		const char* function, const int line){

	int log_level;

	// Checking the logger is defined
	if (logger == NULL){
		return SP_LOGGER_UNDIFINED;
	}

	// Check log level to know if needs to print log
	log_level = get_logger_level();

	if (log_level >= SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL){
		SP_LOGGER_MSG print_res = print_data("---DEBUG---",msg,file,function,line);
		return print_res;
	}
	else{
		//should not write the log
		return SP_LOGGER_SUCCESS;
	}
}

/*
 * This method prints info massages. More information can be found in the header file.
 * The logging level check is done before calling the function
 */
SP_LOGGER_MSG spLoggerPrintInfo(const char* msg){

	int verify_write = 0;
	SP_LOGGER_LEVEL log_level;

	// Checking the logger is defined
	if (logger == NULL){
		return SP_LOGGER_UNDIFINED;
	}

	// Check msg is valid
	if (msg == NULL){
		return SP_LOGGER_INVAlID_ARGUMENT;
	}

	// Check log level to know if needs to print log
	log_level = get_logger_level();

	if (log_level >= SP_LOGGER_INFO_WARNING_ERROR_LEVEL){
		// needs to print data
		FILE* logger_file = logger->outputChannel;
		if (logger_file != stdout){
			// write message into a file
			verify_write = fprintf(logger_file, "---INFO---\n- message: %s\n", msg);
			fflush(logger_file);
			// check writing is OK
			if (verify_write < 0){
				return SP_LOGGER_WRITE_FAIL;
			}
		} // logger file is stdout
		else{
			printf("- message: %s\n", msg);
			fflush(NULL);
		}
		return SP_LOGGER_SUCCESS;
	}
	else{
		//should not write the log
		return SP_LOGGER_SUCCESS; // Need to see what to do with it!!!!!!!!#$(^&W$^(#&$^#$^
	}

}

/*
 * This method unites the error, warning and debug prints since their
 * printing format is the same, as written in the header file.
 * @param title		- The type of printing, ---TITLE---
 * @param msg     	- The message to printed
 * @param file    	- A string representing the filename in which spLoggerPrintWarning call occurred
 * @param function 	- A string representing the function name in which spLoggerPrintWarning call ocurred
 * @param line		- A string representing the line in which the spLoggerPrintWarning call occurred
 * @return
 *
 * SP_LOGGER_INVAlID_ARGUMENT	- If any of msg or file or function are null or line is negative
 * SP_LOGGER_WRITE_FAIL			- If write fail
 * SP_LOGGER_SUCCESS			- otherwise
 */
SP_LOGGER_MSG print_data(const char* title,const char* msg, const char* file,
		const char* function, const int line){

	int verify_write = 0;
	FILE* logger_file = logger->outputChannel;

	// check variables validity
	if (msg == NULL || file == NULL || function == NULL || line < 0){
		return SP_LOGGER_INVAlID_ARGUMENT;
	}

	if (logger_file != stdout){
		// write to file. Done in single command to prevent partial writing
		verify_write = fprintf(logger_file,"%s\n- file: %s\n- function: %s\n- line: %d\n- message: %s\n",
				title,file,function,line,msg);
		fflush(logger_file);
		// check fprintf worked properly
		if (verify_write < 0){
			return SP_LOGGER_WRITE_FAIL;
		}
	} // logger file is stdout
	else{
		printf("%s\n- file: %s\n- function: %s\n- line: %d\n- message: %s\n",
				title,file,function,line,msg);
		fflush(NULL);
	}

	return SP_LOGGER_SUCCESS;
}

/*
 * Prints current message
 */
SP_LOGGER_MSG spLoggerPrintMsg(const char* msg){

	int verify_write = 0;
	FILE* logger_file = logger->outputChannel;

	// Checking the logger is defined
	if (logger == NULL){
		return SP_LOGGER_UNDIFINED;
	}

	// Check msg isvalid
	if (msg == NULL){
		return SP_LOGGER_INVAlID_ARGUMENT;
	}

	if (logger_file != stdout){
		// write to file
		verify_write = fprintf(logger_file,"%s/n",msg);
		// check fprintf worked properly
		if (verify_write < 0){
			return SP_LOGGER_WRITE_FAIL;
		}
	}// logger file is stdout
	else{
		printf("%s/n",msg);
		fflush(NULL);
	}

	return SP_LOGGER_SUCCESS;
}
