%{
  /* lexer.l */
  /* Compatible avec parser.y */
  #include <stdlib.h>
  #include <stdio.h>
  #include "abstract-tree.h"
  #include "tpcc.tab.h"
  #define MAXNAME 30
  int lineno=1;
  int charno=0;
 char* currentLine = NULL;
%}

%option noinput nounput noyywrap
%x SHORT_COMMENT LONG_COMMENT
%%
^.*\n 				{currentLine = (char*)malloc(sizeof(char)*1000);strcpy(currentLine, yytext); REJECT;}
[ \t\r]+			{charno += yyleng;}
\n					{charno=0; lineno++;}
"/*"				{ charno += yyleng;BEGIN LONG_COMMENT; }
"//"				{ charno += yyleng;BEGIN SHORT_COMMENT; }
&&					{ charno += yyleng; strcpy(yylval.Compar , yytext); return AND; }
"||"				{ charno += yyleng; strcpy(yylval.Compar , yytext);return OR; }
"+"|-				{ charno += yyleng; yylval.Operator = yytext[0] ;return ADDSUB; }
"*"|"/"|"%"			{ charno += yyleng; yylval.Operator = yytext[0] ;return DIVSTAR; }
"<"|"<="|">"|>=		{ charno += yyleng;	strcpy(yylval.Compar,yytext); return ORDER; }
==|!=				{ charno += yyleng; strcpy(yylval.Compar ,yytext); return EQ; }
int					{ charno += yyleng; strcpy(yylval.Type , yytext); return SIMPLETYPE;}
char				{ charno += yyleng; strcpy(yylval.Type , yytext); return SIMPLETYPE;}
print				{ charno += yyleng; strcpy(yylval.Instructor , yytext); return PRINT; }
readc				{ charno += yyleng; strcpy(yylval.Instructor , yytext); return READC; }
reade				{ charno += yyleng; strcpy(yylval.Instructor , yytext); return READE; }
void				{ charno += yyleng; strcpy(yylval.Instructor , yytext); return VOID; }
struct				{ charno += yyleng; strcpy(yylval.Type , yytext); return STRUCT; }
if					{ charno += yyleng; strcpy(yylval.Instructor , yytext); return IF; }
else				{ charno += yyleng; strcpy(yylval.Instructor , yytext); return ELSE; }
while				{ charno += yyleng; strcpy(yylval.Instructor , yytext); return WHILE; }
return				{ charno += yyleng; strcpy(yylval.Instructor , yytext); return RETURN; }
[a-zA-Z_][a-zA-Z0-9_]*	{ charno += yyleng; strcpy(yylval.Identifier, yytext); return IDENT; }
[0-9]+				{ charno += yyleng;  yylval.IntLiteral = atoi(yytext) ; return NUM;}
'\\?.'				{ charno += yyleng; yylval.CharLiteral = yytext[1] ; return CHARACTER; }
.					{ charno += yyleng; return yytext[0];}
<LONG_COMMENT>"*/"		{ BEGIN INITIAL; charno += yyleng; }
<LONG_COMMENT,SHORT_COMMENT>.		{charno += yyleng;}
<LONG_COMMENT>\n		{lineno++;charno=0;}
<SHORT_COMMENT>\n		{BEGIN INITIAL; lineno++;charno=0;}
%%

