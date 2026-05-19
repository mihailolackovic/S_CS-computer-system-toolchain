%{

#include <stdio.h>
#include "keywords.h"

%}

%option header-file="misc/lexer.h"

digit [0-9]
letter_lower [a-z]
letter_upper [A-Z]
letter_all {letter_lower}|{letter_upper}
alfanumeric {letter_all}|{digit}
whitespaces [ \r\t]
word {letter_all}({alfanumeric}|_)*
number {digit}+
hex_number 0[Xx]{alfanumeric}+

%%

"#"[^\n]* ;
{whitespaces}+  ;
\n+  {printf("NEWLINE\n"); }

"," { printf("COMMA\n"); }
"%" { printf("IMD_REG\n"); }
"$" { printf("IMD_VAL\n"); }
"[" { printf("IDR_open\n"); }
"]" { printf("IDR_close\n"); }

"+" { printf("ADD\n"); }
"-" { printf("SUB\n"); }
"*" { printf("MUL\n"); }
"/" { printf("DIV\n"); }
"(" { printf("OPEN\n"); }
")" { printf("CLOSE\n"); }


"."{word}+ { printf("%s - DIRECTIVE\n", yytext); }

{word}: { printf("%s - LABEL\n", yytext); }

{number}  { printf("%s - DEC_NUMBER\n", yytext); }
{hex_number}  { printf("%s - HEX_NUMBER\n", yytext); }

{word} { if(lookup_opcode(yytext) == TYPE_OPCODE) printf("%s - OPCODE\n", yytext);  else printf("%s - SYMBOL\n", yytext); }
%%
