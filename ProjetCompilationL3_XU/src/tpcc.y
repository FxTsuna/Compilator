%{
/* as.y */
/* Syntaxe du TPC pour le projet d'analyse syntaxique de 2020-2021*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include "abstract-tree.h"
#include "table-des-symboles.h"
int yylex();
void yyerror(char* s);
extern int lineno;
extern int charno;
extern char* currentLine;
Node* glob = NULL;
char* file_nasm;
FILE* file_name = NULL;

%}

%union {
    Node *node;
    char Identifier[64];
    char CharLiteral;
    int IntLiteral;
    char Assign;
    char Compar[2];
    char Type[64];
    char Operator;
    char Instructor[64];
    char Not;
}

%type <node> Prog TypesVars TypePossible Declarateurs DeclChamps DeclFoncts DeclFonct EnTeteFonct Parametres ListTypVar Corps DeclVars SuiteInstr Instr Exp TB FB M E T F LValue Arguments ListExp
%token <Instructor> IF ELSE WHILE RETURN VOID PRINT READE READC 
%token <Type> SIMPLETYPE STRUCT
%token <IntLiteral> NUM
%token <Identifier> IDENT 
%token <Compar> AND OR ORDER EQ
%token <Operator> ADDSUB DIVSTAR
%token <CharLiteral> CHARACTER
%type <Assign> '='
%type <Not> '!'

%precedence ')'
%precedence ELSE

%%

Prog:  TypesVars DeclFoncts {$$ = makeNode(Prog); addChild($$, $1); addChild($$, $2);glob = $$;}
    ;
TypesVars:
        TypesVars TypePossible Declarateurs ';' {$$ = $2; addSibling($$, $1); addChild($$, $3);}
    |   TypesVars STRUCT IDENT '{' DeclChamps '}' ';' {Node* n = makeNode(Type); strcpy (n->u.identifier, $2); strcat (n->u.identifier, " "); strcat (n->u.identifier, $3); $$ = n; addSibling($$, $1);addChild($$, $5); }
    |   {$$ = NULL;}
    ;
TypePossible:
      SIMPLETYPE  { Node* n = makeNode(Type); strcpy (n->u.identifier, $1); $$ = n;}
    | STRUCT IDENT { Node* n = makeNode(Type); strcpy (n->u.identifier, $1); strcat (n->u.identifier, " "); strcat (n->u.identifier, $2); $$ = n;}
    ;
Declarateurs:
       Declarateurs ',' IDENT {$$ = $1; Node* n = makeNode(Identifier); strcpy(n->u.identifier, $3); addSibling($$, n);}
    |  IDENT {Node* n = makeNode(Identifier); strcpy(n->u.identifier, $1); $$ = n;}
    ;
DeclChamps :
      DeclChamps SIMPLETYPE Declarateurs ';' {Node* n = makeNode(Type); strcpy(n->u.identifier, $2); $$ = n ; addChild($$, $3); addSibling($$, $1);}
    | SIMPLETYPE Declarateurs ';' {Node* n = makeNode(Type); strcpy(n->u.identifier, $1); $$ = n; addChild($$, $2);}
    ;
DeclFoncts:
       DeclFoncts DeclFonct {$$ = $1; addChild($$, $2);}
    |  DeclFonct {$$ = makeNode(DeclFoncts); addChild($$, $1);}
    ;
DeclFonct:
        EnTeteFonct Corps {$$ = makeNode(DeclFonct); addChild($$, $1); addChild($$,$2);}
    ;
EnTeteFonct:
       TypePossible IDENT '(' Parametres ')' {$$ = makeNode(EnTeteFonct); addChild($$, $1); Node* n = makeNode(Identifier); strcpy(n->u.identifier, $2); addChild($$, n); addChild($$,$4);}
    |  VOID IDENT '(' Parametres ')' {$$ = makeNode(EnTeteFonct); Node* n = makeNode(Instructor); strcpy(n->u.identifier, $1); addChild($$, n); Node* m = makeNode(Identifier); strcpy(m->u.identifier, $2); addChild($$, m); addChild($$, $4);}
    ;
Parametres:
       VOID {$$ = makeNode(Parametres); Node* n = makeNode(Instructor); strcpy(n->u.identifier, $1); addChild($$, n);}
    |  ListTypVar {$$ = makeNode(Parametres); addChild($$, $1);}
    ;
ListTypVar:
       ListTypVar ',' TypePossible IDENT {$$ = $3; addSibling($$, $1); Node* m = makeNode(Identifier); strcpy(m->u.identifier, $4); addChild($$, m);}
    |  TypePossible IDENT {$$ = $1; Node* m = makeNode(Identifier); strcpy(m->u.identifier, $2); addChild($$, m);}
    ;
Corps:
       '{' DeclVars SuiteInstr '}' {$$ = makeNode(Corps); addChild($$,$2); addChild($$,$3);}
    ;
DeclVars:
       DeclVars TypePossible Declarateurs ';' {$$ = $2; addSibling($$,$1); addChild($$, $3);}
    | {$$ = NULL;}
    ;

SuiteInstr:
      SuiteInstr Instr {$$ = ($1 ? $1 : $2); if ($1) addSibling($$, $2);}
    | {$$ = NULL;}
    ;
Instr:
       LValue '=' Exp ';'  {Node* n = makeNode(Assign); $$ = n; addChild($$,$1); addChild($$,$3);}
    |  READE '(' IDENT ')' ';' {Node* n = makeNode(Instructor); strcpy(n->u.identifier, $1); $$ = n; Node* m = makeNode(Identifier); strcpy(m->u.identifier, $3); addChild($$, m);}
    |  READC '(' IDENT ')' ';' {Node* n = makeNode(Instructor); strcpy(n->u.identifier, $1); $$ = n; Node* m = makeNode(Identifier); strcpy(m->u.identifier, $3); addChild($$, m);}
    |  PRINT '(' Exp ')' ';' {Node* n = makeNode(Instructor); strcpy(n->u.identifier, $1); $$ = n; addChild($$, $3);}
    |  IF '(' Exp ')' Instr {Node* n = makeNode(Instructor); strcpy(n->u.identifier, $1); $$ = n; addChild($$, $3); addChild($$, $5);}
    |  IF '(' Exp ')' Instr ELSE Instr {Node* n = makeNode(Instructor); strcpy(n->u.identifier, $1); $$ = n; addChild($$,$3); addChild($$,$5); Node* m = makeNode(Instructor); strcpy(m->u.identifier, $6); addChild($$, m); addChild(m,$7); }
    |  WHILE '(' Exp ')' Instr {Node* n = makeNode(Instructor); strcpy(n->u.identifier, $1); $$ = n; addChild($$, $3); addChild($$,$5);}
    |  Exp ';' {$$ = $1;}
    |  RETURN Exp ';'  {Node* n = makeNode(Instructor); strcpy(n->u.identifier, $1); $$ = n; addChild($$, $2);}
    |  RETURN ';' {Node* n = makeNode(Instructor); strcpy(n->u.identifier, $1); $$ = n;Node* m = makeNode(Instructor); strcpy(m->u.identifier, "void"); addChild($$, m);}
    |  '{' SuiteInstr '}'  {$$ = $2;}
    |  ';' {$$ = makeNode(Instr);}
    ;
Exp :  Exp OR TB {Node* n = makeNode(Compar); strcpy(n->u.identifier, $2); $$ = n ; addChild($$, $1); addChild($$, $3);}
    |  TB {$$ = $1;}
    ;
TB  :  TB AND FB {Node* n = makeNode(Compar); strcpy(n->u.identifier, $2); $$ = n; addChild($$, $1); addChild($$, $3);}
    |  FB {$$ = $1;}
    ;
FB  :  FB EQ M {Node* n = makeNode(Compar); strcpy(n->u.identifier, $2); $$ = n; addChild($$, $1); addChild($$, $3);}
    |  M {$$ = $1;}
    ;
M   :  M ORDER E {Node* n = makeNode(Compar); strcpy(n->u.identifier, $2); $$ = n; addChild($$, $1); addChild($$, $3);}
    |  E {$$ = $1;}
    ;
E   :  E ADDSUB T {Node* n = makeNode(Operator); n->u.character = $2; $$ = n; addChild($$, $1); addChild($$, $3);}
    |  T {$$ = $1;}
    ;    
T   :  T DIVSTAR F {Node* n = makeNode(Operator); n->u.character = $2; $$ = n; addChild($$, $1); addChild($$, $3);}
    |  F {$$ = $1;}
    ;
F   :  ADDSUB F     {Node* n = makeNode(Operator); n->u.character = $1; $$ = n; addChild($$, $2);}
    |  '!' F        {Node* n = makeNode(Not); $$ = n;addChild($$,$2);}        
    |  '(' Exp ')'  {$$ = $2;}
    |  NUM          {Node* n = makeNode(IntLiteral); n->u.integer = $1; $$ = n;}
    |  CHARACTER    {Node* n = makeNode(CharLiteral); n->u.character = $1; $$ = n;}
    |  LValue       {$$ = $1;}
    |  IDENT '(' Arguments  ')'  {Node* n = makeNode(Identifier); strcpy(n->u.identifier, $1); $$ = n; addChild($$, $3);}
    ;
LValue:
       IDENT {Node* n = makeNode(Identifier); strcpy(n->u.identifier, $1); $$ = n ;}
    |  IDENT '.' IDENT {$$ = makeNode(LValue); Node* n = makeNode(Identifier); strcpy(n->u.identifier, $1); addChild($$, n); Node* m = makeNode(Identifier); strcpy(m->u.identifier, $3); addChild($$, m);}
    ;
Arguments:
       ListExp {$$ = makeNode(Arguments); addChild($$, $1);}
    | {$$ = NULL;}
    ;
ListExp:
       ListExp ',' Exp {$$ = $1; addSibling($$, $3);}
    |  Exp {$$ = $1;}
    ;
%%

void yyerror(char* s) {
    fprintf(stderr, "%s", s); 
    fprintf(stderr, " :\n%s", currentLine);
    for(int i=0; i<charno-1; i++)
        fprintf(stderr, " ");
    fprintf(stderr, "^\n");
    if (charno == 0) 
        fprintf(stderr, "%s : near line %d in charactere 1\n\n", s , lineno);
    else
        fprintf(stderr, "%s : near line %d in charactere %d\n\n", s , lineno, charno);
}

int main(int argc, char *argv[]){
    int fd = open(argv[2], O_RDONLY);
    dup2(fd, STDIN_FILENO);
    file_nasm = (char *) malloc(256 *sizeof(char));
    if (file_nasm == NULL) {
        exit(0);
    }
    if (argc > 2) {
        strncpy(file_nasm, argv[2], strlen(argv[2]) - 3);
        strcat(file_nasm, "asm");
        file_name = fopen(file_nasm, "w");

    }
    if(yyparse())
        return 1;
    read_symbol_table(glob);
    check_main();
    if (strcmp(argv[1],"-t") == 0 || strcmp(argv[1], "--tree") == 0)
        printTree(glob);
    else if (strcmp(argv[1],"-s") == 0 || strcmp(argv[1], "--symtabs") == 0) {
        display_table();
    } else if (strcmp(argv[1],"-h") == 0 || strcmp(argv[1], "--help") == 0){
        display_help();
    }

    nasmStart(file_name);
    translate(file_name, glob);

    fclose(file_name);



    return 0;
}