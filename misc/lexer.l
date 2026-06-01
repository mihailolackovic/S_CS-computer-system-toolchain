%{

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "keywords.h"

#include "parser.tab.h"

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
string_parts {whitespaces}*{word}{whitespaces}*
string {string_parts}+

%%
\"{string}\" { yylval.str = strdup(yytext); return STRING; }

"#"[^\n]* ;
{whitespaces}+  ;
\n+ { return END_LINE; }

"," { return SEPERATOR; }
"%" { return IND_REG; }
"$" { return IND_VAL; }
"[" { return IDN_OPEN; }
"]" { return IDN_CLOSE; }

"+" { return ADD; }
"-" { return SUB; }
"*" { return MUL; }
"/" { return DIV; }
"(" { return BRAKET_OPEN; }
")" { return BRAKET_CLOSE; }


"."{word}+ { yylval.str = strdup(yytext); return DIRECTIVE; }

{word}: { yylval.str = strdup(yytext); return LABEL; }

{number}  { yylval.number = atoi(yytext); return NUMBER; }
{hex_number}  { yylval.number = strtol(yytext, NULL, 16); return NUMBER; }

{word} {
  switch(lookup_identifier(yytext)){
    case TYPE_OPCODE:
      yylval.str = strdup(yytext);
      return OP_CODE;
    case TYPE_SYMBOL:
      yylval.str = strdup(yytext);
      return SYMBOL;
    case TYPE_REG:
      yylval.str = strdup(yytext);
      return REGISTER;
    case ERRORTYPE:
      break;
  }
}
%%
