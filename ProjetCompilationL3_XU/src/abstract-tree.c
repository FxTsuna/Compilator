/* abstract-tree.c */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "abstract-tree.h"
extern int lineno;       /* from lexer */
extern int charno;

static const char *StringFromKind[] = {
  "Prog",
  "TypesVars",
  "TypePossible", 
  "Declarateurs", 
  "DeclChamps", 
  "DeclFoncts", 
  "DeclFonct", 
  "EnTeteFonct", 
  "Parametres", 
  "Corps", 
  "DeclVars", 
  "SuiteInstr", 
  "Instr", 
  "Exp", 
  "TB", 
  "FB", 
  "M", 
  "E", 
  "T", 
  "F", 
  "LValue", 
  "Arguments", 
  "ListExp",
  "Instructor",
  "Type",
  "Compar",
  "Operator",
  "IntLiteral",
  "CharLiteral",
  "Identifier",
  "Assign",
  "Not"
};

Node *makeNode(Kind kind) {
  Node *node = malloc(sizeof(Node));
  if (!node) {
    printf("Run out of memory\n");
    exit(1);
  }
  node->kind = kind;
  node-> firstChild = node->nextSibling = NULL;
  node->lineno=lineno;
  node->charno=charno;
  return node;
}

void addSibling(Node *node, Node *sibling) {
  Node *curr = node;
  while (curr->nextSibling != NULL) {
    curr = curr->nextSibling;
  }
  curr->nextSibling = sibling;
}

void addChild(Node *parent, Node *child) {
  if (parent->firstChild == NULL) {
    parent->firstChild = child;
  }
  else {
    addSibling(parent->firstChild, child);
  }
}

void deleteTree(Node *node) {
  if (node->firstChild) {
    deleteTree(node->firstChild);
  }
  if (node->nextSibling) {
    deleteTree(node->nextSibling);
  }
  free(node);
}

void printTree(Node *node) {
  static bool rightmost[128]; // current node is rightmost sibling
  static int depth = 0;       // depth of current node
  for (int i = 1; i < depth; i++) { // 2502 = vertical line
    printf(rightmost[i] ? "    " : "\u2502   ");
  }
  if (depth > 0) { // 2514 = up and right; 2500 = horiz; 251c = vertical and right 
    printf(rightmost[depth] ? "\u2514\u2500\u2500 " : "\u251c\u2500\u2500 ");
  }
  printf("%s", StringFromKind[node->kind]);
  switch (node->kind) {
    case IntLiteral: printf(": %d", node->u.integer); break;
    case CharLiteral: printf(": '%c'", node->u.character); break;
    case Identifier: printf(": %s", node->u.identifier); break;
    case Instructor: printf(": %s", node->u.identifier ); break;
    case Type: printf(": %s", node->u.identifier); break;
    case Assign: printf(": '=' "); break;
    case Compar: printf(": %s", node->u.identifier); break;
    case Operator: printf(": %s", node->u.identifier); break;
    default: break;
  }
  printf("\n");
  depth++;
  for (Node *child = node->firstChild; child != NULL; child = child->nextSibling) {
    rightmost[depth] = (child->nextSibling) ? false : true;
    printTree(child);
  }
  depth--;
}
