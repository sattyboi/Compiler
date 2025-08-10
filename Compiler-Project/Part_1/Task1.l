%{
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_VARS 100

/* Variable table to store variable names and their declaration status */
typedef struct {
    char name[50];
    int declared;
} Variable;

Variable varTable[MAX_VARS];
int varCount = 0;
int inVarDecl = 0;

/* Keywords */
char *keywords[] = {
    "int", "char", "if", "else", "while", "for", "main", "begin", "end", 
    "print", "scan", "program", "VarDecl", "inc", "dec"
};

/* Utility functions */
int isKeyword(char *str) {
    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
        if (strcmp(str, keywords[i]) == 0) return 1;
    }
    return 0;
}

int isValidIdentifier(char *str) {
    int len = strlen(str);
    int underscore_count = 0;
    if (!(str[0] >= 'a' && str[0] <= 'z')) return 0;
    for (int i = 0; i < len; i++) {
        if (str[i] == '_') underscore_count++;
        if (underscore_count > 1) return 0;
        if (!(isalnum(str[i]) || str[i] == '_')) return 0;
    }
    return 1;
}

void addVariable(char *name) {
    for (int i = 0; i < varCount; i++) {
        if (strcmp(varTable[i].name, name) == 0) {
            printf("Error: Variable '%s' is defined more than once.\n", name);
            return;
        }
    }
    strcpy(varTable[varCount].name, name);
    varTable[varCount].declared = inVarDecl;
    varCount++;
}

%}

%%

"begin"                { printf("%-12s Keyword\n", yytext); }
"program"              { printf("%-12s Keyword\n", yytext); }
"begin VarDecl:"       { inVarDecl = 1; printf("%-12s Keyword\n", "begin"); printf("%-12s Keyword\n", "VarDecl"); printf("%-12s Separator\n", ":"); }
"end VarDecl"         { inVarDecl = 0; printf("%-12s Keyword\n", "end"); printf("%-12s Keyword\n", "VarDecl"); }
"print"               { if (inVarDecl) { printf("%-12s Error: Keyword is used as an identifier\n", yytext); } else { printf("%-12s Keyword\n", yytext); } }
"scan"                { if (inVarDecl) { printf("%-12s Error: Keyword is used as an identifier\n", yytext); } else { printf("%-12s Keyword\n", yytext); } }
"if"                  { printf("%-12s Keyword\n", yytext); }
"else"                { printf("%-12s Keyword\n", yytext); }
"while"               { printf("%-12s Keyword\n", yytext); }
"for"                 { printf("%-12s Keyword\n", yytext); }
"int"                 { printf("%-12s Keyword\n", yytext); }
"char"                { printf("%-12s Keyword\n", yytext); }

[a-z][a-z0-9_]*        {
    if (isKeyword(yytext)) {
        printf("Error: Keyword '%s' used as a variable name.\n", yytext);
    } else if (!isValidIdentifier(yytext)) {
        printf("%-12s Error: invalid identifier\n", yytext);
    } else {
        if (!inVarDecl) {
            printf("Error: Variable '%s' used before declaration.\n", yytext);
        } else {
            addVariable(yytext);
        }
        printf("%-12s Identifier\n", yytext);
    }
}

"'"[ -~]"'"         { printf("%-12s CHAR_CONSTANT\n", yytext); }
"\""[^\"]*"\"" { printf("%-12s STRING_CONSTANT\n", yytext); }
[0-9]+,\ (2|8|10)    { printf("%-12s INTEGER_CONSTANT\n", yytext); }
[0-9]+,\ [0-9]*     { printf("%-12s Error: invalid integer constant\n", yytext); }

[\+\-\*/%]          { printf("%-12s Arithmetic operator\n", yytext); }
":=" | "\\+=" | "\\-=" | "\\*=" | "/=" | "%=" { printf("%-12s Assignment operator\n", yytext); }

[(){};:,<>]            { printf("%-12s Separator\n", yytext); }

[ \t\n]+             { /* Ignore whitespace */ }
"//".*               { /* Ignore single-line comments */ }
"/*"[^*]*"*"*([^*/][^*]*"*"*)*"*/" { /* Ignore multi-line comments */ }

.                      { printf("UNKNOWN token (%s)\n", yytext); }

%%

int main() {
    yylex();
    return 0;
}

