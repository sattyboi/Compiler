#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Function to create an AST Node
ASTNode* createASTNode() {
    ASTNode* node = (ASTNode*)malloc(sizeof(ASTNode));
    if (!node) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    memset(node, 0, sizeof(ASTNode)); // Initialize everything to zero
    return node;
}

// Function to create a number node in the AST
ASTNode* createNumberNode(int value, int base) {
    // printf("Creating NUMBER node: %d, %d\n", value, base);
    ASTNode* node = createASTNode();
    node->type = NODE_NUMBER;
    node->data.integer.value = value;
    node->data.integer.base = base;
    return node;
}

// Function to create character node
ASTNode* createCharacterNode(char value) {
    // printf("Creating character node: %c\n", value);
    ASTNode* node = createASTNode();
    node->type = NODE_CHAR;
    node->data.value = value;
    return node;
}

// Function to create a variable node in AST
ASTNode* createVariable(char* value) {
    // printf("Creating Variable node: %s\n", value);
    ASTNode* node = createASTNode();
    node->type = NODE_VAR;
    node->data.identifier = strdup(value);
    return node;
}

// Function to create a statements block node in AST
ASTNode* createStatementsNode() {
    // printf("Creating Statements node\n");
    ASTNode* node = createASTNode();
    node->type = NODE_STMTS;
    node->data.statements.count = 0;

    for (int i = 0; i < 100; i++) {
        node->data.statements.statements[i] = NULL;
    }
    return node;
}

// Function to add statements in statements block node
ASTNode* addStatement(ASTNode* stmts, ASTNode* stmt) {
    if (!stmts || stmts->type != NODE_STMTS) {
        fprintf(stderr, "Invalid statements node\n");
        return stmts;
    }
    
    if (stmts->data.statements.count >= 100) {
        fprintf(stderr, "Maximum statement limit reached\n");
        return stmts;
    }
    
    stmts->data.statements.statements[stmts->data.statements.count++] = stmt;
    return stmts;
}

// Function to create a for loop node in AST
ASTNode* createForLoopNode(NodeType type, ASTNode* id, ASTNode* init, ASTNode* limit, ASTNode* update, ASTNode* stmts) {
    // printf("Creating ForLoop Node: %d\n", type);
    ASTNode* node = createASTNode();
    node->type = type;
    node->data.for_loop_block.init = init;
    node->data.for_loop_block.limit = limit;
    node->data.for_loop_block.update = update;
    node->data.for_loop_block.stmts = stmts;
    return node;
}

// Function to create simple if or while loop Node
ASTNode* createIfOrWhileLoopNode(NodeType type, ASTNode* condition, ASTNode* stmts) {
    // printf("Creating if statement / while loop Node: %d\n", type);
    ASTNode* node = createASTNode();
    node->type = type;
    node->data.if_while_block.condition = condition;
    node->data.if_while_block.stmts = stmts;
    return node;
}

// Function to create if else ladder Node
ASTNode* createIfElseLadderNode(NodeType type, ASTNode* condition, ASTNode* stmts, ASTNode* elsepart) {
    // printf("Creating if else Node: %d\n", type);
    ASTNode* node = createASTNode();
    node->type = type;
    node->data.if_else_block.condition = condition;
    node->data.if_else_block.stmts = stmts;
    node->data.if_else_block.else_part = elsepart;
    return node;
}

// Function to create an operator node with left and right children
ASTNode* createOperatorNode(NodeType type, ASTNode* left, ASTNode* right, char* operator) {
    // printf("Creating OPERATOR node: %s\n", operator);
    ASTNode* node = createASTNode();
    node->type = type;
    node->data.operator.left = left;
    node->data.operator.right = right;
    node->data.operator.operator = strdup(operator);
    return node;
}

// Function to create a Program node with VarDecl and stmt block
ASTNode* createProgramNode(NodeType type, ASTNode* VarDecl, ASTNode* StmtBlock) {
    // printf("Creating Program node: %d\n", type);
    ASTNode* node = createASTNode();
    node->type = type;
    node->data.program.varDecl = VarDecl;
    node->data.program.stmtblock = StmtBlock;
    return node;
}

// Function to create a VarDecl node
ASTNode* createVarDeclNode(NodeType type, char* dtype, ASTNode* variable, ASTNode* next) {
    // printf("Creating Variable Declaration node: %d\n", type);
    ASTNode* node = createASTNode();
    node->type = type;
    node->data.var_list.type = strdup(dtype); 
    node->data.var_list.variable = variable;
    node->data.var_list.next = next;
    return node;
}

// Function to create Argument list for print or scan statements
ll* createArgList(char* arg, ll* next) {
    ll* node = (ll*)malloc(sizeof(ll)); 
    if (!node) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    node->string = strdup(arg);
    node->next = next;
    return node;
}

// Function for creating AST node for print or scan statements
ASTNode* createPrintOrScanNode(NodeType type, char* keyword, char* string, ll* args, int count) {
    ASTNode* node = createASTNode();
    node->type = type;
    node->data.print_scan_stmt.keyword = keyword;
    node->data.print_scan_stmt.string = string;
    node->data.print_scan_stmt.args = args;
    node->data.print_scan_stmt.count = count;
    // printf("%d\n", count);
    return node;
}

// Function to print the AST in a readable format
void printIndent(int indent) {
    for (int i = 0; i < indent; i++) {
        printf(" ");
    }
}

void printAST(ASTNode* node) {
    printASTHelper(node, 0);
}

void printASTHelper(ASTNode* node, int indent) {
    if (node == NULL) return;
    
    switch (node->type) {
        case NODE_NUMBER:
            printf("(%d %d)", node->data.integer.value, node->data.integer.base);
            break;
            
        case NODE_CHAR:
            printf("'%c'", node->data.value);
            break;
            
        case NODE_VAR:
            printf("%s", node->data.identifier);
            break;
            
        case NODE_OP:
        case NODE_RELOP:
        case NODE_ASSIGN:
            printf("(%s ", node->data.operator.operator);
            printASTHelper(node->data.operator.left, indent + 2);
            printf(" ");
            printASTHelper(node->data.operator.right, indent + 2);
            printf(")");
            break;
        case NODE_INC:
        case NODE_DEC:
            printf("(%s ", node->data.operator.operator);
            printASTHelper(node->data.operator.left, indent + 2);
            printf(")");
            break;
            
        case NODE_PROG:
            printf("(\n");
            printIndent(indent + 2);
            printASTHelper(node->data.program.varDecl, indent + 2);
            printf("\n");
            printIndent(indent + 2);
            printASTHelper(node->data.program.stmtblock, indent + 2);
            printf("\n");
            printIndent(indent);
            printf(")\n");
            break;
            
        case NODE_VARDEC: {
            printf("(\n");
            ASTNode* temp = node;
            while (temp != NULL) {
                printIndent(indent + 2);
                printf("(");
                printASTHelper(temp->data.var_list.variable, indent + 4);
                printf(" %s)\n", temp->data.var_list.type);
                temp = temp->data.var_list.next;
            }
            printIndent(indent);
            printf(")");
            break;
        }
            
        case NODE_STMTS:
            printf("(\n");
            for (int i = 0; i < node->data.statements.count ; i++) {
                printIndent(indent + 2);
                printASTHelper(node->data.statements.statements[i], indent + 2);
                printf("\n");
            }
            printIndent(indent);
            printf(")");
            break;
        
        case NODE_PRINT:
        case NODE_SCAN: {
            printf("(%s %s", node->data.print_scan_stmt.keyword, node->data.print_scan_stmt.string);
            ll* current = node->data.print_scan_stmt.args;
            while (current != NULL) {
                printf(" %s", current->string);
                current = current->next;
            }
            printf(")");
            break;
        }

        case NODE_IF:
            printf("(if\n");
            printIndent(indent + 2);
            printf("(");
            printASTHelper(node->data.if_while_block.condition, indent + 4);
            printf(")\n");
            printIndent(indent + 2);
            printf("(");
            printASTHelper(node->data.if_while_block.stmts, indent + 4);
            printf(")\n");
            printIndent(indent);
            printf(")");
            break;
            
        case NODE_IF_ELSE:
            printf("(if\n");
            printIndent(indent + 2);
            printf("(");
            printASTHelper(node->data.if_else_block.condition, indent + 4);
            printf(")\n");
            printIndent(indent + 2);
            printf("(");
            printASTHelper(node->data.if_else_block.stmts, indent + 4);
            printf(")\n");
            printIndent(indent + 2);
            printf("(else ");
            printASTHelper(node->data.if_else_block.else_part, indent + 4);
            printf(")\n");
            printIndent(indent);
            printf(")");
            break;
            
        case NODE_WHILE:
            printf("(while\n");
            printIndent(indent + 2);
            printf("(");
            printASTHelper(node->data.if_while_block.condition, indent + 4);
            printf(")\n");
            printIndent(indent + 2);
            printf("(");
            printASTHelper(node->data.if_while_block.stmts, indent + 4);
            printf(")\n");
            printIndent(indent);
            printf(")");
            break;
            
        case NODE_FOR:
            printf("(for\n");
            printIndent(indent + 2);
            printf("(");
            printASTHelper(node->data.for_loop_block.init, indent + 4);
            printf(")\n");
            printIndent(indent + 2);
            printf("(");
            printASTHelper(node->data.for_loop_block.limit, indent + 4);
            printf(")\n");
            printIndent(indent + 2);
            printf("(");
            printASTHelper(node->data.for_loop_block.update, indent + 4);
            printf(")\n");
            printIndent(indent + 2);
            printf("(");
            printASTHelper(node->data.for_loop_block.stmts, indent + 4);
            printf(")\n");
            printIndent(indent);
            printf(")");
            break;
            
        default:
            printf("(UNKNOWN NODE TYPE)");
            break;
    }
}


// Functions to free the memory allocated for the AST
void freeLL(ll* head) {
    while (head) {
        ll* temp = head;
        head = head->next;
        free(temp->string);
        free(temp);
    }
}

void freeAST(ASTNode* node) {
    if (!node) return;

    switch (node->type) {
        case NODE_PROG:
            // printf("test Prog\n");
            freeAST(node->data.program.varDecl); 
            freeAST(node->data.program.stmtblock);
            break;
        
        case NODE_VARDEC:
            // printf("test vardecl\n");
            free(node->data.var_list.type);
            freeAST(node->data.var_list.variable); 
            freeAST(node->data.var_list.next);
            break;

        case NODE_STMTS:
            // printf("test stmts\n");
            for (int i = 0; i < node->data.statements.count; i++) {
                freeAST(node->data.statements.statements[i]); 
            }
            break;

        case NODE_FOR:
            // printf("test for\n");
            freeAST(node->data.for_loop_block.init); 
            freeAST(node->data.for_loop_block.limit); 
            freeAST(node->data.for_loop_block.update); 
            freeAST(node->data.for_loop_block.stmts);
            break;

        case NODE_WHILE:
            // printf("test while\n");
            freeAST(node->data.if_while_block.condition); 
            freeAST(node->data.if_while_block.stmts); 
            break;

        case NODE_IF_ELSE:
            // printf("test ifelse\n");
            freeAST(node->data.if_else_block.condition); 
            freeAST(node->data.if_else_block.stmts); 
            freeAST(node->data.if_else_block.else_part); 
            break;

        case NODE_IF:
            // printf("test if\n");
            freeAST(node->data.if_while_block.condition); 
            freeAST(node->data.if_while_block.stmts); 
            break;

        case NODE_NUMBER:
        case NODE_CHAR:
            break;

        case NODE_ASSIGN:
        case NODE_INC:
        case NODE_DEC:
        case NODE_OP:
        case NODE_RELOP:
            // printf("test opera\n");
            freeAST(node->data.operator.left); 
            freeAST(node->data.operator.right); 
            free(node->data.operator.operator);
            break;

        case NODE_VAR:
            // printf("test var\n");
            free(node->data.identifier);
            break;

        case NODE_SCAN:
        case NODE_PRINT:
            // printf("test Printscan\n");
            free(node->data.print_scan_stmt.string); 
            freeLL(node->data.print_scan_stmt.args);
            break;

        default:
            fprintf(stderr, "Unknown AST Node Type: %d\n", node->type);
            break;
    }

    free(node);
}