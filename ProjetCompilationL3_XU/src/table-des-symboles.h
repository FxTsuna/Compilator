#ifndef __SYMBOL__
#define __SYMBOL__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "abstract-tree.h"

typedef struct tsentry {
	char name[64];
	char type[64];
	size_t size;
	size_t addr;
} TSEntry;

typedef struct tablesymbol {
	TSEntry entries[256];
	int size;
}TableSymbol;

typedef struct tableloc {
	char name[64];
	char return_type[64];
	TSEntry param[256];
	TSEntry local[256];
	int size_param;
	int size_local;
	int size;
	int size_arg;
} TableLoc;

typedef struct tablesymbolloc {
	TableLoc loc[256];
	int size;
} TableSymbolLoc;

void read_glo_var(Node* tree);
void read_loc_var(Node *tree);
void add_symbol_global(Node* node, char name[64], char type[64]);
void add_symbol_loc(Node* node, char name[64], char type[64]);
void add_symbol_fct(Node* node, char name[64], char type[64]);
void add_symbol_arg(Node* node, char name[64], char type[64]);
size_t size_types(char type[]);
void read_symbol_table(Node *tree);
void display_global_table();
void display_local_table();
void decl_read (Node* t);
void display_help();
int check_main();
void display_table();
void check_type(Node* node, char check_type[64], char wanted_type[64]);
int check_var(Node* node,char* nom_fct,char* ident,int lineno);
int returnIndice(char* nom_fct,char* ident);
void nasmDebutFunc(char* ident,FILE* file);
void nasmFinFunc(FILE* file);
void nasmFuncMainFin(FILE* file);
void nasmMainRet(FILE* file);
void nasmLocVar(FILE* file,char* nom_fct);
void nasmGlobVar(FILE* file);
void nasmStart(FILE* file);
void translate (FILE *file, Node *node);

#endif