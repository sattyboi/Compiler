%{
#include "parser.tab.h" 
#include "ast.h"
#include <string.h>
#include <stdlib.h>
%}

%%
"begin"             { return BEGI; }
"end"               { return END; }
"program"           { return PROGRAM; }
"VarDecl"           { return VARDECL; }
"int"               { yylval.str = strdup(yytext); return INT; }
"char"              { yylval.str = strdup(yytext); return CHAR; }
"if"                { return IF; }
"then"              { return THEN; }
"else"              { return ELSE; }
"while"             { return WHILE; }
"do"                { return DO; }
"for"               { return FOR; }
"to"                { return TO; }
"print"             { return PRINT; }
"scan"              { return SCAN; }

"inc"               { yylval.str = strdup(yytext); return INC; }
"dec"               { yylval.str = strdup(yytext); return DEC; }

":="                { yylval.str = strdup(yytext); return ASSIGN; }
"+="                { yylval.str = strdup(yytext); return ADD_ASSIGN; }
"-="                { yylval.str = strdup(yytext); return SUB_ASSIGN; }
"*="                { yylval.str = strdup(yytext); return MUL_ASSIGN; }
"/="                { yylval.str = strdup(yytext); return DIV_ASSIGN; }
"%="                { yylval.str = strdup(yytext); return MOD_ASSIGN; }

">="                { yylval.str = strdup(yytext); return GE; }
"<="                { yylval.str = strdup(yytext); return LE; }
"<>"                { yylval.str = strdup(yytext); return NE; }
"="                 { yylval.str = strdup(yytext); return EQ; }
">"                 { yylval.str = strdup(yytext); return GT; }
"<"                 { yylval.str = strdup(yytext); return LT; }

"+"                 { yylval.str = strdup(yytext); return ADD; }
"-"                 { yylval.str = strdup(yytext); return SUB; }
"*"                 { yylval.str = strdup(yytext); return MUL; }
"/"                 { yylval.str = strdup(yytext); return DIV; }
"%"                 { yylval.str = strdup(yytext); return MOD; }


"("                 { return LPAREN; }
")"                 { return RPAREN; }
"["                 { return LBRACKET; }
"]"                 { return RBRACKET; }
";"                 { return SEMICOLON; }
":"                 { return COLON; }
","                 { return COMMA; }

[0-9]+              { return NUM; }
"("[ ]*[0-9]+[ ]*","[ ]*(2|8|10)[ ]*")"             {   
                                                        char* tok = strtok(yytext, "( ) ,"); 
                                                        yylval.pair.val = atoi(tok); 
                                                        tok = strtok(NULL, " ");
                                                        yylval.pair.base = atoi(tok); 
                                                        return INTCONST; 
                                                    }

"'"[ -~]"'"         { yylval.c = yytext[1]; return CHARCONST; }
["](.)*["]       { yylval.str = strdup(yytext); return STRINGCONST; }


[a-z][a-z0-9_]*     {
                        yylval.ast = createVariable(yytext); 
                        return ID; 
                    }

[ \t\n]+             { /* Ignore whitespace */ }
"//".*               { /* Ignore single-line comments */ }
"/*"[^*]*"*"*([^*/][^*]*"*"*)*"*/" { /* Ignore multi-line comments */ }

.                   { return yytext[0]; }

%%

int yywrap() {
    return 1;
}
