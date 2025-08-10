#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "ast.h"

// Symbol Table Entry
typedef struct Symbol {
    char name[100];
    int int_value;
    char char_value;
    int is_char; // 1 if variable is char, 0 if int
    int assigned; // 1 if value is assigned
    struct Symbol* next;
} Symbol;

// Global Symbol Table
extern Symbol* symbol_table;

// Symbol Table Functions
void updateSymbolTable(char* name, int is_char, int int_val, char char_val);
void declareSymbol(char* name, int is_char);
int getIntValue(char* name);

// AST Evaluation Functions
int evaluateExpression(ASTNode* node);
int evaluateCondition(ASTNode* node);
void evaluateAST(ASTNode* node);

// Utility to Print Symbol Table
void printSymbolTable();

#endif // SYMBOL_TABLE_H
