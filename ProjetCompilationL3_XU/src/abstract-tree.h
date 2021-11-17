/* abstract-tree.h */

#ifndef ABSTRACT_TREE
#define ABSTRACT_TREE

typedef enum {
  Prog,
  TypesVars,
  TypePossible, 
  Declarateurs, 
  DeclChamps, 
  DeclFoncts, 
  DeclFonct, 
  EnTeteFonct, 
  Parametres, 
  Corps, 
  DeclVars, 
  SuiteInstr, 
  Instr, 
  Exp, 
  TB, 
  FB, 
  M, 
  E, 
  T, 
  F, 
  LValue, 
  Arguments, 
  ListExp,
  Instructor,
  Type,
  Compar,
  Operator,
  IntLiteral,
  CharLiteral,
  Identifier,
  Assign,
  Not
} Kind;


typedef struct Node {
  Kind kind;
  union {
    int integer;
    char character;
    char identifier[64];
  } u;
  struct Node *firstChild, *nextSibling;
  int lineno;
  int charno;
} Node;

Node *makeNode(Kind kind);
void addSibling(Node *node, Node *sibling);
void addChild(Node *parent, Node *child);
void deleteTree(Node*node);
void printTree(Node *node);

#define FIRSTCHILD(node) node->firstChild
#define SECONDCHILD(node) node->firstChild->nextSibling
#define THIRDCHILD(node) node->firstChild->nextSibling->nextSibling

#endif