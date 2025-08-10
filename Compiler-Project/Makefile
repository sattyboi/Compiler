# Paths
SRC_DIR = src
INCLUDE_DIR = include
BUILD_DIR = build
TEST_DIR = ../Test

# Flex and Bison sources
FLEX_SRC = $(SRC_DIR)/parser/parser.l
BISON_SRC = $(SRC_DIR)/parser/parser.y

# Generated files
BISON_OUTPUT = $(BUILD_DIR)/parser.tab.c
BISON_HEADER = $(BUILD_DIR)/parser.tab.h
FLEX_OUTPUT = $(BUILD_DIR)/parser.yy.c

# Source files
AST_SRC = $(SRC_DIR)/ast/ast.c
AC_SRC = $(SRC_DIR)/3_AC/3_ac.c
SIM_SRC = $(SRC_DIR)/simulation/simulation.c

# Object files
AST_OBJ = $(BUILD_DIR)/ast.o
AC_OBJ = $(BUILD_DIR)/3_ac.o
SIM_OBJ = $(BUILD_DIR)/simulation.o
PARSER_OBJ = $(BUILD_DIR)/parser.tab.o
LEXER_OBJ = $(BUILD_DIR)/parser.yy.o

OBJS = $(AST_OBJ) $(AC_OBJ) $(SIM_OBJ) $(PARSER_OBJ) $(LEXER_OBJ)

# Compiler settings
CC = gcc
CWARN = -Wall
CFLAGS = -g -I$(INCLUDE_DIR) -I$(BUILD_DIR)

# Final executable
TARGET = $(BUILD_DIR)/compiler_sim

# Default target
all: $(TARGET)

# Build the final executable
$(TARGET): $(OBJS) | $(BUILD_DIR)
	$(CC) $(CWARN) $(CFLAGS) -o $@ $(OBJS)

# Build AST object
$(AST_OBJ): $(AST_SRC) | $(BUILD_DIR)
	$(CC) $(CWARN) $(CFLAGS) -c $< -o $@

# Build 3AC object
$(AC_OBJ): $(AC_SRC) | $(BUILD_DIR)
	$(CC) $(CWARN) $(CFLAGS) -c $< -o $@

# Build Simulation object
$(SIM_OBJ): $(SIM_SRC) | $(BUILD_DIR)
	$(CC) $(CWARN) $(CFLAGS) -c $< -o $@

# Special rules for Flex and Bison
$(BISON_OUTPUT) $(BISON_HEADER): $(BISON_SRC) | $(BUILD_DIR)
	bison -d -o $(BISON_OUTPUT) $(BISON_SRC)

$(FLEX_OUTPUT): $(FLEX_SRC) $(BISON_HEADER) | $(BUILD_DIR)
	flex -o $(FLEX_OUTPUT) $(FLEX_SRC)

# Compile parser
$(PARSER_OBJ): $(BISON_OUTPUT) | $(BUILD_DIR)
	$(CC) $(CWARN) $(CFLAGS) -c $< -o $@

# Compile lexer
$(LEXER_OBJ): $(FLEX_OUTPUT) | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create build directory if it does not exist
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

# Clean build
clean:
	rm -rf $(BUILD_DIR)

# Run the program with file input
run: all
	@if [ -z "$(file)" ]; then \
		echo "Error: file variable not set. Usage: make run file=yourfilename"; \
		exit 1; \
	fi
	./$(TARGET) $(TEST_DIR)/$(file).txt

.PHONY: all clean run
