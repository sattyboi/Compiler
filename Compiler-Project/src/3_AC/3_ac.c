#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"

int tempCount = 1;
int labelCount = 1;

// Function to create new temporary variables
char* newTemp() {
    char* temp = (char*)malloc(10);
    sprintf(temp, "t%d", tempCount++);
    return temp;
}

// Function to create new labels
char* newLabel() {
    char* label = (char*)malloc(10);
    sprintf(label, "L%d", labelCount++);
    return label;
}

// Function to generate 3AC recursively
char* generate3AC(ASTNode* node) {
    if (node == NULL) return NULL;

    switch (node->type) {
        case NODE_NUMBER: {
            char* temp = newTemp();
            // printf("// NUMBER NODE: %d (base %d)\n", node->data.integer.value, node->data.integer.base);
            printf("%s := (%d, %d)\n", temp, node->data.integer.value, node->data.integer.base);
            return temp;
        }

        case NODE_CHAR: {
            char* temp = newTemp();
            // printf("// CHAR NODE: '%c'\n", node->data.value);
            printf("%s := '%c'\n", temp, node->data.value);
            return temp;
        }

        case NODE_VAR: {
            // printf("// VARIABLE NODE: %s\n", node->data.identifier);
            return strdup(node->data.identifier);
        }

        case NODE_OP:
        case NODE_RELOP: {
            char* left = generate3AC(node->data.operator.left);
            char right[32];
        
            if (node->data.operator.right->type == NODE_CHAR) {
                char* temp = newTemp();
                sprintf(right, "'%c'", node->data.operator.right->data.value);
                printf("%s := %s %s %s\n", temp, left, node->data.operator.operator, right);
                return temp;
            } else if (node->data.operator.right->type == NODE_NUMBER) {
                char* temp = newTemp();
                sprintf(right, "(%d, %d)", node->data.operator.right->data.integer.value, node->data.operator.right->data.integer.base); 
                printf("%s := %s %s %s\n", temp, left, node->data.operator.operator, right);
                return temp;
            } else {
                char* rightExp = generate3AC(node->data.operator.right);
                char* temp = newTemp();
                printf("%s := %s %s %s\n", temp, left, node->data.operator.operator, rightExp);
                return temp;
            }
        }        
        case NODE_ASSIGN: {
            char* left = generate3AC(node->data.operator.left);
            char right[32];
            if(strcmp(node->data.operator.operator, ":=") != 0){
                if (node->data.operator.right->type == NODE_CHAR) {
                    sprintf(right, "'%c'", node->data.operator.right->data.value);
                    printf("%s := %s %c %s\n", left, left, node->data.operator.operator[0], right);
                    return left;
                } else if (node->data.operator.right->type == NODE_NUMBER) {
                    sprintf(right, "(%d, %d)", node->data.operator.right->data.integer.value, node->data.operator.right->data.integer.base); 
                    printf("%s := %s %c %s\n", left, left, node->data.operator.operator[0], right);
                    return left;
                } else {
                    char* rightExp = generate3AC(node->data.operator.right);
                    printf("%s := %s %c %s\n", left, left, node->data.operator.operator[0], rightExp);
                    return left;
                }
            }else{
                if (node->data.operator.right->type == NODE_CHAR) {
                    sprintf(right, "'%c'", node->data.operator.right->data.value);
                    printf("%s := %s\n", left, right);
                    return left;
                } else if (node->data.operator.right->type == NODE_NUMBER) {
                    sprintf(right, "(%d, %d)", node->data.operator.right->data.integer.value, node->data.operator.right->data.integer.base); 
                    printf("%s := %s\n", left, right);
                    return left;
                } else {
                    char* rightExp = generate3AC(node->data.operator.right);
                    printf("%s := %s\n", left, rightExp);
                    return left;
                }
            }
        }        

        case NODE_IF: {
            // printf("// IF STATEMENT\n");
            char* condition = generate3AC(node->data.if_while_block.condition);
            char* labelEnd = newLabel();

            printf("if %s == 0 goto %s\n", condition, labelEnd);
            generate3AC(node->data.if_while_block.stmts);
            printf("%s:\n", labelEnd);
            return NULL;
        }

        case NODE_IF_ELSE: {
            // printf("// IF-ELSE STATEMENT\n");
            char* condition = generate3AC(node->data.if_else_block.condition);

            char* labelFalse = newLabel();
            char* labelEnd = newLabel();

            printf("if %s == 0 goto %s\n", condition, labelFalse);
            generate3AC(node->data.if_else_block.stmts);
            printf("goto %s\n", labelEnd);
            printf("%s:\n", labelFalse);
            generate3AC(node->data.if_else_block.else_part);
            printf("%s:\n", labelEnd);
            return NULL;
        }

        case NODE_WHILE: {
            // printf("// WHILE LOOP\n");
            char* labelStart = newLabel();
            char* labelEnd = newLabel();

            printf("%s:\n", labelStart);
            char* condition = generate3AC(node->data.if_while_block.condition);
            printf("if %s == 0 goto %s\n", condition, labelEnd);

            generate3AC(node->data.if_while_block.stmts);
            printf("goto %s\n", labelStart);
            printf("%s:\n", labelEnd);
            return NULL;
        }

        case NODE_FOR: {
            // printf("// FOR LOOP\n");
            char* labelStart = newLabel();
            char* labelEnd = newLabel();
            
            ASTNode* u = node->data.for_loop_block.update->data.operator.left;
            ASTNode* i = node->data.for_loop_block.init->data.operator.left;
            char* iord = node->data.for_loop_block.update->data.operator.operator; 

            generate3AC(node->data.for_loop_block.init);
            printf("%s:\n", labelStart);
            char* condition = generate3AC(node->data.for_loop_block.limit);
            char* update = newTemp();
            char* check = newTemp();
            printf("%s := (%d, %d)\n", update, u->data.integer.value, u->data.integer.base);
            printf("%s := %s > %s\n", check, i->data.identifier, condition );
            printf("if %s == 1 goto %s\n", check, labelEnd);
            generate3AC(node->data.for_loop_block.stmts);

            char* updation = newTemp();
            if(strcmp(iord, "inc")== 0){
                printf("%s := %s + %s\n",updation, i->data.identifier, update);
            }else{
                printf("%s := %s - %s\n",updation, i->data.identifier, update);
            }
            printf("%s := %s\n", i->data.identifier,updation);
            printf("goto %s\n", labelStart);
            printf("%s:\n", labelEnd);
            return NULL;
        }

        case NODE_STMTS: {
            // printf("// STATEMENTS BLOCK\n");
            for (int i = 0; i < node->data.statements.count; i++) {
                generate3AC(node->data.statements.statements[i]);
            }
            return NULL;
        }

        case NODE_PROG: {
            // printf("// PROGRAM START\n");
            generate3AC(node->data.program.varDecl);
            generate3AC(node->data.program.stmtblock);
            // printf("// PROGRAM END\n");
            return NULL;
        }

        default:
            // printf("// UNKNOWN NODE TYPE\n");
            return NULL;
    }
}

// Main function for testing
// int main() {
//     // Example: you can create any AST here manually
//     ASTNode* a = createVariable("a");
//     ASTNode* b = createVariable("b");
//     ASTNode* five = createNumberNode(5, 10);
//     ASTNode* assign = createOperatorNode(NODE_ASSIGN, a, five, "=");
//     ASTNode* stmts = createStatementsNode();
//     addStatement(stmts, assign);

//     ASTNode* prog = createProgramNode(NODE_PROG, NULL, stmts);

//     printf("=== AST PRINT ===\n");
//     printAST(prog);
//     printf("\n=== 3AC OUTPUT ===\n");
//     generate3AC(prog);

//     freeAST(prog);
//     return 0;
// }
