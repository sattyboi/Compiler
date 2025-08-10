/* parser.y - Bison file for the language grammar */

%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"
extern FILE *yyin;
extern int yylineno;
extern char* yytext;
int yyparse();
void yyerror(const char *s);
int yylex();
ASTNode* root;
%}

%code requires { 
    #include "ast.h"
    #include "3_ac.h"
    #include "simulation.h"
}

%union {
    char* str;
    int num;
    char c;
    ASTNode* ast;
    struct {
        int val;
        int base;
    } pair;
    ll * l;
    struct {
        char* str;
        int inc; // inc if 1
    } incdec;
    struct {
        ll* l;
        int count;
    } arglist;
}

%define parse.error verbose
/* Terminals from the lexer */
%token <str>  STRINGCONST 
%token <c> CHARCONST
%token <pair> INTCONST
%token <ast> ID

%token BEGI END PROGRAM VARDECL
%token <str> PRINT SCAN IF ELSE WHILE FOR INT CHAR
%token TO THEN DO NUM
%token <str> INC DEC
%token <str> ADD SUB MUL DIV MOD
%token <str> ASSIGN ADD_ASSIGN SUB_ASSIGN MUL_ASSIGN DIV_ASSIGN MOD_ASSIGN
%token <str> GT LT GE LE EQ NE

%token LPAREN RPAREN LBRACKET RBRACKET SEMICOLON COLON COMMA


%left ADD SUB
%left MUL DIV MOD

%type <str> Type AssignOp RelOp
%type <incdec> ForIncDec
%type <arglist> ExpList IdList ScanArgs  PrintArgs
%type <ast> Program VarDeclBlock VarDeclList VarDecl StmtBlock BlockStmt Stmt AssignStmt IfStmt WhileStmt ForStmt PrintStmt ScanStmt Exp Condition Var

%%

Program         : BEGI PROGRAM COLON VarDeclBlock StmtBlock END PROGRAM     {   
                                                                                $$ = createProgramNode(NODE_PROG,$4, $5);
                                                                                root = $$; 
                                                                            }
                ;

VarDeclBlock    : BEGI VARDECL COLON VarDeclList END VARDECL {$$ = $4;}
                ;

VarDeclList     : VarDecl VarDeclList {$1->data.var_list.next = $2; $$ = $1;}
                | /* empty */ {$$ = NULL;}
                ;

VarDecl         : LPAREN Var COMMA Type RPAREN SEMICOLON {$$ = createVarDeclNode(NODE_VARDEC, $4, $2, NULL);  }
                ;

Var             : ID {$$ = $1;}
                | ID LBRACKET NUM RBRACKET {$$ = $1;} // Find a way to store the number and pass it to simulation.h
                ;

Type            : INT {$$ = $1;}
                | CHAR {$$ = $1;}
                ;

StmtBlock       : /* empty */ { $$ = createStatementsNode();}
                | StmtBlock Stmt { $$ = addStatement($1, $2);}
                ;

BlockStmt       : BEGI StmtBlock END {$$ = $2;}
                ;

Stmt            : AssignStmt {$$ = $1;}
                | IfStmt {$$ = $1;}
                | WhileStmt {$$ = $1;}
                | ForStmt {$$ = $1;}
                | PrintStmt {$$ = $1;}
                | ScanStmt {$$ = $1;}
                ;

AssignStmt      : ID AssignOp Exp SEMICOLON {$$ = createOperatorNode(NODE_ASSIGN, $1, $3, $2);} // add array assign below as needed
                ;

AssignOp        : ASSIGN {$$ = $1;}
                | ADD_ASSIGN {$$ = $1;}
                | SUB_ASSIGN {$$ = $1;}
                | MUL_ASSIGN {$$ = $1;}
                | DIV_ASSIGN {$$ = $1;}
                | MOD_ASSIGN {$$ = $1;} 
                ;

IfStmt		    : IF LPAREN Condition RPAREN BlockStmt SEMICOLON   {$$ = createIfOrWhileLoopNode(NODE_IF, $3, $5);}      
                | IF LPAREN Condition RPAREN BlockStmt ELSE BlockStmt SEMICOLON  {$$ = createIfElseLadderNode(NODE_IF_ELSE, $3, $5, $7);} 
                ;
                
WhileStmt	    : WHILE LPAREN Condition RPAREN DO BlockStmt SEMICOLON {$$ = createIfOrWhileLoopNode(NODE_WHILE, $3, $6);}
         	    ;

ForStmt         : FOR ID ASSIGN Exp TO Exp ForIncDec Exp DO BlockStmt SEMICOLON {$$ = createForLoopNode(NODE_FOR, $2, createOperatorNode(NODE_ASSIGN, $2, $4, $3), $6, createOperatorNode((($<incdec.inc>7 ==1)? NODE_INC : NODE_DEC), $8, NULL, $<incdec.str>7), $10);}
                ;

ForIncDec       : INC {$$.inc = 1; $$.str = $1;}
                | DEC {$$.str = $1; $$.inc = 0;}
                ;

PrintStmt       : PRINT LPAREN STRINGCONST PrintArgs RPAREN SEMICOLON {$$ = createPrintOrScanNode(NODE_PRINT, "print", $3, $<arglist.l>4, $<arglist.count>4); }
                ;

PrintArgs       : COMMA ExpList {$$ = $2;}
                | {$<arglist.l>$ = (ll*)NULL; $<arglist.count>$ = 0;}
                ;

ExpList         : Exp {$<arglist.l>$ = createArgList($1->data.identifier, NULL); $<arglist.count>$ = 1;}
                | Exp COMMA ExpList {$<arglist.l>$ = createArgList($1->data.identifier, $<arglist.l>3); $<arglist.count>$ = $<arglist.count>3 + 1;}
                ;

ScanStmt        : SCAN LPAREN STRINGCONST ScanArgs RPAREN SEMICOLON { $$ = createPrintOrScanNode(NODE_SCAN, "scan", $3, $<arglist.l>4, $<arglist.count>4);}
                ;

ScanArgs        : COMMA IdList {$$ = $2;}
                | {$<arglist.l>$ = (ll*)NULL; $<arglist.count>$ = 0;}
                ;

IdList          : ID {$<arglist.l>$ = createArgList($1->data.identifier, NULL); $<arglist.count>$ = 1;}
                | ID COMMA IdList {$<arglist.l>$ = createArgList($1->data.identifier, $<arglist.l>3); $<arglist.count>$ = $<arglist.count>3 + 1;}
                ;

Exp	  	        : ID {$$ =$1;}
                | CHARCONST {$$ = createCharacterNode($1);}
                | INTCONST { $$ = createNumberNode($<pair.val>1, $<pair.base>1);}
                | LPAREN Exp RPAREN {$$ = $2;}
                | Exp ADD Exp { $$ = createOperatorNode(NODE_OP, $1, $3, $2);}
                | Exp SUB Exp { $$ = createOperatorNode(NODE_OP, $1, $3, $2);}
                | Exp MUL Exp { $$ = createOperatorNode(NODE_OP, $1, $3, $2);}
                | Exp DIV Exp { $$ = createOperatorNode(NODE_OP, $1, $3, $2);}
                | Exp MOD Exp { $$ = createOperatorNode(NODE_OP, $1, $3, $2);}
                ;

Condition       : Exp RelOp Exp {$$ = createOperatorNode(NODE_RELOP, $1, $3, $2);}
                ;

RelOp           : GT {$$ = $1;}
                | LT {$$ = $1;}
                | GE {$$ = $1;}
                | LE {$$ = $1;}
                | EQ {$$ = $1;}
                | NE {$$ = $1;}
                ;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error : %s before token '%s'\n", s, yytext);
}

void inputLoop(){
    printf("1. AST\n2. 3 Address Code\n3. Simulation of code\n");
    printf("Select an option (1-3, 0 to exit): \n");
    int choice;
    scanf("%d", &choice);
    while(choice != 0){
        if(choice == 1){
            printf("---------------------\nAbstract Syntax Tree: \n---------------------\n");
            printAST(root); 
        }else if(choice == 2){
            printf("---------------\n3 Address Code: \n---------------\n");
            generate3AC(root);
        }else if(choice == 3){
            printf("-------------------------\nOutput of your test code:\n-------------------------\n"); 
            evaluateAST(root);
            printSymbolTable();
        }else{
            printf("Try a valid choice!\n");
        }
        printf("\nSelect an option (1-3, 0 to exit): \n");
        scanf("%d", &choice);
    } 
    freeAST(root);
}

int main(int argc, char *argv[]){
    if (argc != 2){
        fprintf(stderr, "Usage: %s <input file>\n", argv[0]);
        return 1;
    }
    yyin = fopen(argv[1], "r");
    if (!yyin){
        perror("Error opening file");
        return 1;
    }
    if (yyparse() == 0){
        printf("Input successfully parsed.\n");
        inputLoop();
    }
    fclose(yyin);
    return 0;
}
