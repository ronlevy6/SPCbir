CC = gcc
OBJS = sp_config_unit_test.o SPConfig.o SPLogger.o
EXEC = sp_config_unit_test
TESTS_DIR = ./unit_test
COMP_FLAG = -std=c99 -Wall -Wextra \
-Werror -pedantic-errors

$(EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@

sp_config_unit_test.o: unit_test/sp_config_unit_test.c unit_test/../SPConfig.h unit_test/../SPLogger.h unit_test/unit_test_util.h
	$(CC) $(COMP_FLAG) -c $(TESTS_DIR)/$*.c
SPConfig.o: SPConfig.c SPConfig.h SPLogger.h
	$(CC) $(COMP_FLAG) -c $*.c	
SPLogger.o: SPLogger.c SPLogger.h 
	$(CC) $(COMP_FLAG) -c $*.c
clean:
	rm -f $(OBJS) $(EXEC)
