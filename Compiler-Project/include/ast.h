#ifndef AST_H
#define AST_H

typedef enum {
    NODE_PROG,     // For program begin
    NODE_VARDEC,   // For each var decl
    NODE_STMTS,    // For each statements
    NODE_FOR, 
    NODE_WHILE,
    NODE_IF_ELSE,
    NODE_IF,
    NODE_NUMBER,
    NODE_CHAR,
    NODE_ASSIGN, // = += -= *= /= %=
    NODE_INC, 
    NODE_DEC, 
    NODE_OP, // + - * / %
    NODE_RELOP, // > < <= >= <> ==
    NODE_VAR,
    NODE_SCAN,
    NODE_PRINT
} NodeType;

typedef struct ll {
    char* string;
    struct ll* next;
}ll;

typedef struct ASTNode {
    NodeType type;
    union {
        // basic constants character and integer
        struct {
            int value;
            int base;
        } integer; 

        char value;
        
        // statements like Assignment, Relop, Operations, conditions
        struct {
            struct ASTNode* left;
            struct ASTNode* right;
            char* operator;
        } operator;
        
        // program Node
        struct {
            struct ASTNode* varDecl; 
            struct ASTNode* stmtblock;
        } program; 

        // stores variable declarations as a linked list of declarations
        struct {
            char* type;
            struct ASTNode* variable;
            struct ASTNode* next;
        } var_list;

        // statements
        struct {
            struct ASTNode* statements[100]; // Assuming a maximum of 1000 statements in the statement block
            int count; 
        } statements;

        // if-else block
        struct {
            struct ASTNode* condition;
            struct ASTNode* stmts;
            struct ASTNode* else_part;
        } if_else_block; 

        // simple if block or while block
        struct {
            struct ASTNode* condition;
            struct ASTNode* stmts;
        } if_while_block;

        // for loop block
        struct {
            struct ASTNode* init;
            struct ASTNode* limit;
            struct ASTNode* update;
            struct ASTNode* stmts;
        } for_loop_block;

        // print and scan statements
        struct {
            char* keyword;
            char* string;
            ll* args;
            int count;
        }print_scan_stmt;

        // Variable Name
        char* identifier;

    } data;
} ASTNode;



// Basic node creation
ASTNode* createASTNode();
ASTNode* createNumberNode(int value, int base);
ASTNode* createCharacterNode(char value);
ASTNode* createVariable(char* value);

// Complex node creation
ASTNode* createStatementsNode();
ASTNode* addStatement(ASTNode* stmts, ASTNode* stmt);
ll* createArgList(char* arg, ll* next);
ASTNode* createPrintOrScanNode(NodeType type, char* keyword, char* string, ll* args, int count);
ASTNode* createForLoopNode(NodeType type, ASTNode* id, ASTNode* init, ASTNode* limit, ASTNode* update, ASTNode* stmts);
ASTNode* createIfOrWhileLoopNode(NodeType type, ASTNode* condition, ASTNode* stmts);
ASTNode* createIfElseLadderNode(NodeType type, ASTNode* condition, ASTNode* stmts, ASTNode* elsepart);
ASTNode* createOperatorNode(NodeType type, ASTNode* left, ASTNode* right, char* operator);
ASTNode* createVarDeclNode(NodeType type, char* dtype, ASTNode* variable, ASTNode* next);
ASTNode* createProgramNode(NodeType type, ASTNode* VarDecl, ASTNode* StmtBlock);

// AST operations
void printAST(ASTNode* node);
void freeAST(ASTNode* node);
void printASTHelper(ASTNode* node, int indent);

#endif // AST_H