CC = gcc
CFLAGS = -ansi -pedantic -Wall
ifdef DEBUG
	CFLAG +=  -g
endif
PROG_NAME = assembler

TEST_DIR = tests
TEST_DIR_IN = $(TEST_DIR)/tests_in
TEST_DIR_IN_WARN = $(TEST_DIR_IN)/warnings
TEST_DIR_IN_ERR = $(TEST_DIR_IN)/error
TEST_DIR_IN_OK = $(TEST_DIR_IN)/ok

TEST_DIR_OUT = $(TEST_DIR)/tests_out

BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin

OBJECTS = assembler.o lexer.o trie.o vector.o project_common.o preprocessor.o output_unit.o main.o 

OK_TESTS := $(basename $(shell find $(TEST_DIR_IN_GOOD) -name '*.as')) 

.PHONY: clean build_env

$(PROG_NAME): build_env $(OBJECTS)
	$(CC) $(CFLAG) $(OBJ_DIR)/*.o -o $(BIN_DIR)/$@ 

clean:
	rm -rf $(BUILD_DIR)
	rm -f $(TEST_DIR_OUT)/* $(TEST_DIR_OUT)/*.am $(TEST_DIR_OUT)/*.ent $(TEST_DIR_OUT)/*.ext $(TEST_DIR_OUT)/*.ob

run_tests: $(PROG_NAME)
	$(BIN_DIR)/$(PROG_NAME) $(OK_TESTS) -o $(TEST_DIR_OUT)/*

build_env:
	mkdir -p $(BUILD_DIR)
	mkdir -p $(OBJ_DIR)
	mkdir -p $(BIN_DIR)



assembler.o: assembler/assembler.c assembler/assembler.h \
 assembler/../lexer/lexer.h assembler/../data_structures/vector.h \
 assembler/../data_structures/trie.h \
 assembler/../preprocessor/preprocessor.h \
 assembler/../project_common/project_common.h \
 assembler/../project_common/../data_structures/vector.h \
 assembler/../project_common/../data_structures/trie.h \
 assembler/../output_unit/output_unit.h \
 assembler/../output_unit/../project_common/project_common.h

lexer.o: lexer/lexer.c lexer/lexer.h lexer/../data_structures/trie.h

preprocessor.o: preprocessor/preprocessor.c preprocessor/preprocessor.h \
 preprocessor/../data_structures/vector.h \
 preprocessor/../data_structures/trie.h

trie.o: data_structures/trie.c data_structures/trie.h

vector.o: data_structures/vector.c data_structures/vector.h

main.o: main.c assembler/assembler.h

output_unit.o: output_unit/output_unit.c output_unit/output_unit.h \
 output_unit/../project_common/project_common.h \
 output_unit/../project_common/../data_structures/vector.h \
 output_unit/../project_common/../data_structures/trie.h

project_common.o: project_common/project_common.c \
 project_common/project_common.h \
 project_common/../data_structures/vector.h \
 project_common/../data_structures/trie.h

 %.o:
	$(CC) $(CFLAG) -c $< -o $(OBJ_DIR)/$@ 
