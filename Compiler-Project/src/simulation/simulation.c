#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

// Structure to store a single Variable entry
typedef struct Symbol {
    char name[100];
    int int_value;
    char char_value;
    int is_char; // 1 if char, 0 else
    int assigned; // 1 if assigned, 0 else
    struct Symbol* next;
} Symbol;

// Symbol Table
Symbol* symbol_table = NULL;

// Function to convert Integer constant to Decimal
int convertToDecimal(int value, int base){
    if(base != 2 && base != 8 && base != 10){
        printf("Base encountered: %d, expected values: 2, 8, 10\n", base);
        exit(EXIT_FAILURE);
    }
    int temp = value;
    int result = 0;
    int multiplier = 1;
    while(value > 0){
        int digit = value % 10;
        if(digit >= base){
            printf("Expected digit < base %d, in the integer (%d, %d)\n", base, temp, base);
            exit(EXIT_FAILURE);
        }
        result += digit * multiplier;
        multiplier *= base;
        value /= 10;
    }
    return result;
}

// Lookup variables
Symbol* lookupSymbol(const char* name){
    Symbol* current = symbol_table;
    while(current){
        if (strcmp(current->name, name) == 0) return current;
        current = current->next;
    }
    return NULL;
}

// Utility to add/update variable in symbol table
void updateSymbolTable(char* name, int is_char, int int_val, char char_val, int assigned) {
    Symbol* current = symbol_table;
    while (current){
        if(strcmp(current->name, name) == 0){
            if(is_char){
                current->char_value = char_val;
            }else{
                current->int_value = int_val;
            }
            current->assigned = assigned;
            return;
        }
        current = current->next;
    }
    // New Variable
    Symbol* new_symbol = (Symbol*)malloc(sizeof(Symbol));
    strcpy(new_symbol->name, name);
    new_symbol->int_value = int_val;
    new_symbol->char_value = char_val;
    new_symbol->is_char = is_char;
    new_symbol->assigned = assigned;
    new_symbol->next = symbol_table;
    symbol_table = new_symbol;
}

// Utility to declare variable (without assigning)
void declareSymbol(char* name, int is_char){
    updateSymbolTable(name, is_char, 0, '\0', 0);
}

// Utility to scan input
void scanSymbol(Symbol* sym) {
    if(sym->is_char){
        char input;
        if(scanf(" %c", &input) != 2){
            printf("Error: Invalid input for char\n");
            exit(EXIT_FAILURE);
        }
        sym->char_value = input;
    }else{
        int input, base;
        if(scanf("(%d, %d)", &input, &base) != 2){
            printf("Error: Invalid input format for int. Expected (value, base)\n");
            printf("%d %d ", input, base);
            exit(EXIT_FAILURE);
        }
        sym->int_value = convertToDecimal(input, base);
    }
    sym->assigned = 1;
}

// Function to evaluate expressions recursively
int evaluateExpression(ASTNode* node){
    if(!node) return 0;

    switch(node->type){
        case NODE_NUMBER:
            return convertToDecimal(node->data.integer.value, node->data.integer.base);

        case NODE_VAR:
            Symbol* sym = lookupSymbol(node->data.identifier);
            if(!sym){
                fprintf(stderr, "Variable %s not declared!\n", node->data.identifier);
                exit(EXIT_FAILURE);
            }
            if(sym->is_char){
                fprintf(stderr, "Type Error: Cannot use char variable '%s' in arithmetic expression!\n", sym->name);
                exit(EXIT_FAILURE);
            }
            return sym->int_value;

        case NODE_OP:
            int left = evaluateExpression(node->data.operator.left);
            int right = evaluateExpression(node->data.operator.right);
            if(strcmp(node->data.operator.operator, "+") == 0) return left + right;
            if(strcmp(node->data.operator.operator, "-") == 0) return left - right;
            if(strcmp(node->data.operator.operator, "*") == 0) return left * right;
            if(strcmp(node->data.operator.operator, "/") == 0) return left / right;
            if(strcmp(node->data.operator.operator, "%") == 0) return left % right;
            fprintf(stderr, "Unknown operator: %s\n", node->data.operator.operator);
            exit(EXIT_FAILURE);

        default:
            fprintf(stderr, "Unknown expression type!\n");
            exit(EXIT_FAILURE);
    }
}

// Function to evaluate conditions
int evaluateCondition(ASTNode* node){
    if(!node) return 0;

    if(node->type == NODE_RELOP){
        int left = evaluateExpression(node->data.operator.left);
        int right = evaluateExpression(node->data.operator.right);
        if(strcmp(node->data.operator.operator, "<") == 0) return left < right;
        if(strcmp(node->data.operator.operator, ">") == 0) return left > right;
        if(strcmp(node->data.operator.operator, "<=") == 0) return left <= right;
        if(strcmp(node->data.operator.operator, ">=") == 0) return left >= right;
        if(strcmp(node->data.operator.operator, "=") == 0) return left == right;
        if(strcmp(node->data.operator.operator, "<>") == 0) return left != right;
        fprintf(stderr, "Unknown relational operator: %s\n", node->data.operator.operator);
        exit(EXIT_FAILURE);
    }

    fprintf(stderr, "Invalid condition node!\n");
    exit(EXIT_FAILURE);
}

// Recursive function to evaluate the AST
void evaluateAST(ASTNode* node){
    if(!node) return;

    switch(node->type){
        case NODE_PROG:{
            evaluateAST(node->data.program.varDecl);
            evaluateAST(node->data.program.stmtblock);
            break;
        }
        case NODE_VARDEC:{
            ASTNode* temp = node;
            while(temp){
                declareSymbol(temp->data.var_list.variable->data.identifier, strcmp(temp->data.var_list.type, "char") == 0);
                temp = temp->data.var_list.next;
            }
            break;
        }
        case NODE_ASSIGN:{ 
            char* var_name = node->data.operator.left->data.identifier;
            Symbol* sym = lookupSymbol(var_name);
            if(!sym){
                printf("Error: Variable %s not declared\n", var_name);
                break;
            }
            if (node->data.operator.right->type == NODE_CHAR) {
                if (!sym->is_char) {
                    fprintf(stderr, "Type Error: Cannot assign char to int variable '%s'\n", var_name);
                    exit(EXIT_FAILURE);
                }
            } else {
                if (sym->is_char) {
                    fprintf(stderr, "Type Error: Cannot assign int to char variable '%s'\n", var_name);
                    exit(EXIT_FAILURE);
                }
            }    
            if(node->data.operator.right->type == NODE_CHAR){
                updateSymbolTable(var_name, 1, 0, node->data.operator.right->data.value, 1);
            }else{
                int val = evaluateExpression(node->data.operator.right);
        
                if(strcmp(node->data.operator.operator, ":=") == 0){
                    updateSymbolTable(var_name, 0, val, '\0', 1);
                }
                else if(strcmp(node->data.operator.operator, "+=") == 0){
                    sym->int_value += val;
                }
                else if(strcmp(node->data.operator.operator, "-=") == 0){
                    sym->int_value -= val;
                }
                else if(strcmp(node->data.operator.operator, "*=") == 0){
                    sym->int_value *= val;
                }
                else if(strcmp(node->data.operator.operator, "%=") == 0){
                    sym->int_value %= val;
                }
                else if(strcmp(node->data.operator.operator, "/=") == 0){
                    if(val == 0){
                        fprintf(stderr, "Error: Division by zero\n");
                        exit(EXIT_FAILURE);
                    }
                    sym->int_value /= val;
                }
                else{
                    fprintf(stderr, "Error: Unknown assignment operator %s\n", node->data.operator.operator);
                    exit(EXIT_FAILURE);
                }
            }
            break;
        }
        case NODE_STMTS:{
            for(int i = 0; i < node->data.statements.count; i++)
                evaluateAST(node->data.statements.statements[i]);
            break;
        }
        case NODE_IF:{
            if(evaluateCondition(node->data.if_while_block.condition))
                evaluateAST(node->data.if_while_block.stmts);
            break;
        }
        case NODE_IF_ELSE:{
            if(evaluateCondition(node->data.if_else_block.condition))
                evaluateAST(node->data.if_else_block.stmts);
            else
                evaluateAST(node->data.if_else_block.else_part);
            break;
        }
        case NODE_FOR:{
            evaluateAST(node->data.for_loop_block.init);
            char* var_name = node->data.for_loop_block.init->data.operator.left->data.identifier;
            Symbol* sym = lookupSymbol(var_name);
            if(!sym){
                printf("Error: Variable %s not declared\n", var_name);
                break;
            }
            ASTNode* n = node->data.for_loop_block.update->data.operator.left;

            if(node->data.for_loop_block.update->type == NODE_INC){
                while(sym->int_value < evaluateExpression(node->data.for_loop_block.limit)){      
                    evaluateAST(node->data.for_loop_block.stmts);
                    sym->int_value += convertToDecimal(n->data.integer.value, n->data.integer.base);
                }
                break;
            }
            else if(node->data.for_loop_block.update->type == NODE_DEC){
                while(sym->int_value > evaluateExpression(node->data.for_loop_block.limit)){      
                    evaluateAST(node->data.for_loop_block.stmts);
                    sym->int_value -= convertToDecimal(n->data.integer.value, n->data.integer.base);
                }
                break;
            }
        } 
        case NODE_WHILE:{
            while(evaluateCondition(node->data.if_while_block.condition))
                evaluateAST(node->data.if_while_block.stmts);
            break;
        }
        case NODE_PRINT:{
            const char* format = node->data.print_scan_stmt.string;
            ll* arg_node = node->data.print_scan_stmt.args;
            int arg_idx = 0;
            
            for (int i = 0; format[i] != '\0'; i++) {
                if (format[i] == '@') {
                    if (!arg_node) {
                        fprintf(stderr, "Error: Too few arguments provided for placeholders in print\n");
                        exit(EXIT_FAILURE);
                    }
            
                    Symbol* sym = lookupSymbol(arg_node->string);
                    if (!sym) {
                        fprintf(stderr, "Error: Variable %s not declared\n", arg_node->string);
                        exit(EXIT_FAILURE);
                    }
            
                    if (sym->is_char) {
                        printf("%c", sym->char_value);
                    } else {
                        printf("%d", sym->int_value);
                    }
            
                    arg_node = arg_node->next;
                    arg_idx++;
                } else {
                    putchar(format[i]);
                }
            }
            
            if (arg_idx < node->data.print_scan_stmt.count) {
                fprintf(stderr, "Error: Too many arguments passed to print\n");
                exit(EXIT_FAILURE);
            }
            
            printf("\n");
            break;
        }
        case NODE_SCAN:{
            const char* format = node->data.print_scan_stmt.string;
            ll* arg_node = node->data.print_scan_stmt.args;
            int arg_idx = 0;
            for (int i = 1; format[i] != '"'; i++) {
                if (format[i] == '@') {
                    if (!arg_node) {
                        fprintf(stderr, "Error: Too few arguments provided for placeholders in scan\n");
                        exit(EXIT_FAILURE);
                    }
                    Symbol* sym = lookupSymbol(arg_node->string);
                    if (!sym) {
                        printf("Error: Variable %s not declared\n", arg_node->string);
                        break;
                    }
                        
                    scanSymbol(sym);
                    arg_node = arg_node->next;
                    arg_idx++;
                }else{
                    char c;
                    scanf("%c", &c);
                    if(c != format[i]){
                        fprintf(stderr, "Scan format mismatch! Expected '%c', but got '%c'.\n", format[i], c);
                        exit(EXIT_FAILURE);
                    }
                }
            }

            if (arg_node != NULL) {
                fprintf(stderr, "Error: Too many arguments provided for placeholders in scan\n");
                exit(EXIT_FAILURE);
            }
            break;
        }
        default:
            break;
    }
}

// Function to print the symbol table
void printSymbolTable(){
    printf("\nSymbol Table:\n");
    printf("-------------------------------------\n");
    printf(" %-10s | %-6s | %-10s \n", "Name", "Type", "Value");
    printf("-------------------------------------\n");

    Symbol* current = symbol_table;
    while(current){
        if(current->is_char){
            if(current->assigned)
                printf(" %-10s | %-6s | '%c'      \n", current->name, "char", current->char_value);
            else
                printf(" %-10s | %-6s | (unassigned) \n", current->name, "char");
        }else{
            if(current->assigned)
                printf(" %-10s | %-6s | (%d, 10) \n", current->name, "int", current->int_value);
            else
                printf(" %-10s | %-6s | (unassigned) \n", current->name, "int");
        }
        current = current->next;
    }
    printf("-------------------------------------\n");
}
