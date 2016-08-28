CC = gcc
OBJS = sp_pkdarray_unit_test.o SPConfig.o SPLogger.o SPPoint.o main_aux.o SPBPriorityQueue.o SPListElement.o SPList.o
EXEC = sp_pkdarray_unit_test
TESTS_DIR = ./unit_test
COMP_FLAG = -std=c99 -Wall -Wextra \
-Werror -pedantic-errors

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@

sp_pkdarray_unit_test.o: unit_test/sp_pkdarray_unit_test.c \
 unit_test/../SPConfig.h unit_test/../SPLogger.h unit_test/../SPPoint.h \
 unit_test/unit_test_util.h unit_test/../main_aux.h unit_test/../SPBPriorityQueue.h unit_test/../SPListElement.h unit_test/../SPList.h
	$(CC) $(COMP_FLAG) -c $(TESTS_DIR)/$*.c
SPConfig.o: SPConfig.c SPConfig.h SPLogger.h
	$(CC) $(COMP_FLAG) -c $*.c	
SPLogger.o: SPLogger.c SPLogger.h 
	$(CC) $(COMP_FLAG) -c $*.c
SPListElement.o: SPListElement.c SPListElement.h
	$(CC) $(COMP_FLAG) -c $*.c
SPBPriorityQueue.o: SPBPriorityQueue.c SPBPriorityQueue.h SPListElement.h SPList.h
	$(CC) $(COMP_FLAG) -c $*.c
main_aux.o: main_aux.c main_aux.h SPLogger.h SPPoint.h SPConfig.h SPBPriorityQueue.h SPListElement.h
	$(CC) $(COMP_FLAG) -c $*.c
SPPoint.o: SPPoint.c SPPoint.h 
	$(CC) $(COMP_FLAG) -c $*.c
SPList.o: SPList.c SPList.h SPListElement.h
	$(CC) $(C_COMP_FLAG) -c $*.c
clean:
	rm -f $(OBJS) $(EXEC)
