
#include "table-des-symboles.h"

extern int lineno;
extern int charno;
TableSymbol global_table_symbol; 
TableSymbolLoc local_table_symbol;
char copyTypes[64];

void read_glo_var(Node* tree) {

 	Node* var;
    char copyType[64];
    if(tree -> kind == Type){ 
      	strcpy(copyType,tree->u.identifier);
      	if(strcmp(tree -> u.identifier,"struct") == 0){
        	add_symbol_global(tree, tree->u.identifier, copyType);
      	}
      	else{
        	for(var = tree -> firstChild ; var; var = var->nextSibling){
          		if(var -> kind == Identifier){
            		add_symbol_global(var, var -> u.identifier,"struct");
          		}
        	}
      	}
    }
}	


void read_loc_var(Node *tree) {
    Node* var;
    Node* type;
    char copyType[64];
    for(type = tree -> firstChild ; type; type = type->nextSibling){
      if(type -> kind == Type){ 
        strcpy(copyType,type->u.identifier);
          for(var = type -> firstChild ; var; var = var->nextSibling){
            if(var -> kind == Identifier){
              add_symbol_loc(var, var -> u.identifier,copyType);
            }
          }
      }
    }	
	local_table_symbol.loc->size++;
}

void add_symbol_global(Node* node, char name[64], char type[64]) {
	int count;
	for (count = 0; count < global_table_symbol.size; count++) {
		if (!strcmp(global_table_symbol.entries[count].name, name)) {
    		fprintf(stderr,"Erreur de semantique, redefinition %s vers la ligne %d caractere %d\n", name, node->lineno + 1, node->charno+1);
      		exit(2);
    	}
  	} if (global_table_symbol.size > 256) { 
    	fprintf(stderr, "Nombre maximum de variable excéder\n");
    	exit(3);
  	}
  	strcpy(global_table_symbol.entries[global_table_symbol.size].name, name);
  	strcpy(global_table_symbol.entries[global_table_symbol.size].type,type);
  	global_table_symbol.entries[global_table_symbol.size].size = size_types(type);
  	global_table_symbol.entries[global_table_symbol.size].addr = global_table_symbol.size * 8;
	global_table_symbol.size++;	
}

void add_symbol_loc(Node* node,char name[64], char type[64]) {
  	int count;
  	for (count = 0; count < local_table_symbol.size; count++) {
    	if (!strcmp(local_table_symbol.loc[local_table_symbol.loc->size].local[count].name, name)) {
      		fprintf(stderr, "Erreur de semantique, redefinition %s vers la ligne %d caractere %d\n",name, node->lineno+1, node->charno+1);
      		exit(2);
    	}
  	}
  	if (local_table_symbol.size > 256) {
    	fprintf(stderr, "Nombre maximum de variable excéder\n");
    	exit(3);
  	}
  	strcpy(local_table_symbol.loc[local_table_symbol.loc->size].local[local_table_symbol.size].name, name);
  	strcpy(local_table_symbol.loc[local_table_symbol.loc->size].local[local_table_symbol.size].type,type);
  	local_table_symbol.loc[local_table_symbol.loc->size].local[local_table_symbol.size].size = size_types(type);
  	local_table_symbol.loc[local_table_symbol.loc->size].local[local_table_symbol.size].addr = local_table_symbol.size * 8;
  	local_table_symbol.size++;
  	local_table_symbol.loc[local_table_symbol.loc->size].size_local = local_table_symbol.size;	
}

void add_symbol_fct(Node* node,char name[64], char type[64]) {
  	int count;
  	local_table_symbol.size = 0;
  	local_table_symbol.loc->size_arg = 0;
  	for (count = 0; count < local_table_symbol.loc->size; count++) {
   		if (!strcmp(local_table_symbol.loc[count].name, name)) {
      		fprintf(stderr, "Erreur de semantique, redefinition de fonction %s vers la ligne %d caractere %d\n", name, node->lineno+1, node->charno+1);      
      		exit(2);
    	}
  	}
  	if (local_table_symbol.loc->size > 256) {
    	fprintf(stderr, "Nombre maximum de fonction excéder\n");
    	exit(3);
  	}
  	strcpy(local_table_symbol.loc[local_table_symbol.loc->size].name, name);
  	strcpy(local_table_symbol.loc[local_table_symbol.loc->size].return_type,type);	
}

void add_symbol_arg(Node* node, char name[64], char type[64]) {
  	int count;
  	for (count = 0; count < local_table_symbol.loc->size_arg; count++) {
    	if (!strcmp(local_table_symbol.loc[local_table_symbol.loc->size].param[count].name, name)) {
      		fprintf(stderr, "Erreur de semantique, redefinition d'argument %s vers la ligne %d caractere %d\n" , name, node->lineno+1, node->charno+1);
      		exit(2);
    	}
  	}
  	if (local_table_symbol.loc->size_arg > 256) {
    	fprintf(stderr, "Nombre maximum d'argument excéder\n");
    	exit(3);
  	}
  	strcpy(local_table_symbol.loc[local_table_symbol.loc->size].param[local_table_symbol.loc->size_arg].name, name);
  	strcpy(local_table_symbol.loc[local_table_symbol.loc->size].param[local_table_symbol.loc->size_arg].type,type);
  	local_table_symbol.loc[local_table_symbol.loc->size].param[local_table_symbol.loc->size_arg].size = size_types(type);
  	local_table_symbol.loc[local_table_symbol.loc->size].param[local_table_symbol.loc->size_arg].addr = local_table_symbol.loc->size_arg;
  	local_table_symbol.loc->size_arg++;
  	local_table_symbol.loc[local_table_symbol.loc->size].size_param = local_table_symbol.loc->size_arg;	
}

size_t size_types(char type[]){
	if (strcmp(type, "int") == 0){
		return sizeof(int);
	} else if (strcmp(type, "char") == 0){
		return sizeof(char);
	} else if (strcmp(type, "struct") == 0){
		return sizeof(double);
	} else
		return 0;
}

void read_symbol_table(Node *tree) {
	Node* t;
    Node* type;
    Node* var;
    Node* func;
    Node* term;
    Node* decl;
    char copyType[64];
    global_table_symbol.size = 0;
	local_table_symbol.size = 0;
	local_table_symbol.loc->size = 0;
	local_table_symbol.loc->size_arg = 0;

    for(t = tree->firstChild; t; t = t->nextSibling){
        read_glo_var(t);
        if(t -> kind == DeclFoncts){
            for(type = t -> firstChild ; type; type = type->nextSibling){
                for(var = type -> firstChild ; var; var = var->nextSibling){
                    if(var -> kind == EnTeteFonct){
                        for(func = var -> firstChild ; func; func = func -> nextSibling){
                            if(func -> kind == Type){
                                strcpy(copyType,func ->u.identifier);
                            }
                            if(func -> kind == Identifier){
                                add_symbol_fct(func, func -> u.identifier,copyType);
                            }
                            if(func -> kind == Parametres){
                                for(decl = func -> firstChild ; decl; decl = decl->nextSibling){
                                    if(decl -> kind == Type){
                                        strcpy(copyType,decl ->u.identifier);
                                        term = decl ->firstChild;
                                        if(term -> kind == Identifier){
                                            add_symbol_arg(term, term -> u.identifier,copyType);
                                        }
                                    }                         
                                }
                            }	
                        }
                        printf("\n");
                    } else if(var->kind == Corps){
                        read_loc_var(var);
                    }
                }
            }
        }
    }
}

int check_main(){
	int i = 0;
	int exist = 0;
  	for(i = 0; i < local_table_symbol.loc->size; i++){
    	if(strcmp(local_table_symbol.loc[i].name, "main") == 0){
    		exist = 1;
    	}
  	}
  	if (exist == 0){
  		fprintf(stderr,"WARNING : Il n'y a pas de fonction main\n");
  	}
  	return 0;
}	

void display_global_table() {
	int count;
  	printf("Table globale : \n\n");
  	for (count = 0; count < global_table_symbol.size; count++) {
    	printf("%s : %s\n", global_table_symbol.entries[count].name, global_table_symbol.entries[count].type);
  	}	
}

void display_local_table() {
	int count;
	int count_param;
	int count_local;
	printf("\nTable locale :\n\n");
	for (count = 0; count < local_table_symbol.loc->size; count++) {
	    printf("   Fonction : %s, type de retour de la fonction : %s \n", local_table_symbol.loc[count].name, local_table_symbol.loc[count].return_type);
	    printf("\n   Paramètres :\n\n");
	    for (count_param = 0; count_param < local_table_symbol.loc[count].size_param ; count_param++){
    		printf("     %s : %s\n", local_table_symbol.loc[count].param[count_param].name, local_table_symbol.loc[count].param[count_param].type);
  		}
	    printf("\n   Local : \n\n");
	    for (count_local = 0; count_local < local_table_symbol.loc[count].size_local ; count_local++){
      		printf("     %s : %s\n",local_table_symbol.loc[count].local[count_local].name, local_table_symbol.loc[count].local[count_local].type);
  		}
		printf("\n");
	}	
}

void display_table() {
	display_global_table();
	display_local_table();
}

void display_help() {
	printf("Usage: ./tpcc [OPTIONS] [filename] \n");
	printf("Options:\n");
	printf("-t: Affiche l'arbre abstrait du fichier\n");
	printf("-s: Affiche le table des symboles du fichier\n");
	printf("-h: Affiche ce message\n");
}

/* ---------------CHECK---------------*/


void check_type(Node* node, char check_type[64], char wanted_type[64]) {

  char check[64];
  strcpy(check, check_type);

  int i,j,k,l,m,n;

  for(i = 0; i < local_table_symbol.loc->size; i++){
    for(j = 0; j < local_table_symbol.size; j++){
      if(strcmp(check_type, local_table_symbol.loc[i].local[j].name) == 0){
        strcpy(check, local_table_symbol.loc[i].local[j].type);
      }
    }
  }

  for(l = 0; l < local_table_symbol.loc->size; l++){
    for(m = 0; m < local_table_symbol.loc->size_arg; m++){
      if(strcmp(check_type,local_table_symbol.loc[l].param[m].name) == 0){
        strcpy(check,local_table_symbol.loc[l].param[m].type);
      }
    }
  }

  for(k = 0; k < global_table_symbol.size; k++){
    if(strcmp(global_table_symbol.entries[k].name, check_type) == 0){
        strcpy(check, global_table_symbol.entries[k].type);
    }
  }

  for(n = 0; n < local_table_symbol.loc->size; n++){
    if(strcmp(local_table_symbol.loc[n].name,check_type) == 0){
        strcpy(check,local_table_symbol.loc[n].return_type);
    }
  }
  if(strcmp(check,wanted_type) == 0){
    return;
  }
  fprintf(stderr, "Type voulu : %s mais on a eu : %s vers la ligne %d caractere %d\n", wanted_type, check, node->lineno + 1, node->charno + 1);
}

int check_var(Node* node,char* nom_fct,char* ident,int lineno){
  int i,j,k,l;

  if(strcmp(ident, nom_fct) == 0){
      return 0;
  }

  for(k = 0; k < global_table_symbol.size; k++){
    if(strcmp(global_table_symbol.entries[k].name, ident) == 0){
      return 0;
    }
  }

  for(i = 0; i < local_table_symbol.loc->size; i++){
    if(strcmp(local_table_symbol.loc[i].name, nom_fct) == 0){
      for(j = 0; j < local_table_symbol.loc[i].size_local; j++){
        if(strcmp(local_table_symbol.loc[i].local[j].name,ident) == 0){
          return 1;
        }
      }
      for(l = 0; l < local_table_symbol.loc[i].size_param; l++){
        if(strcmp(local_table_symbol.loc[i].param[l].name,ident) == 0){
          return 3;
        }
      }
    }
    if(strcmp(local_table_symbol.loc[i].name,ident) == 0){
      return 0;
    }
  }
  fprintf(stderr,"La variable %s n'existe pas ligne : %d !\n\n",ident, lineno);
  return 2;
}

int returnIndice(char* nom_fct,char* ident){
  int i,j;
  for(i = 0; i < local_table_symbol.loc->size; i++){
    if(strcmp(local_table_symbol.loc[i].name,nom_fct) == 0){
      for(j = 0; j < local_table_symbol.loc[i].size_local; j++){
        if(strcmp(local_table_symbol.loc[i].local[j].name,ident) == 0){
          return local_table_symbol.loc[i].size_local - j;
        }
      }
    }
  }
  return 0;
}


void nasmDebutFunc(char* ident,FILE* file){
  fprintf(file,"%s :\n",ident);
  fprintf(file,"push rbp\nmov rbp,rsp\n");
}

void nasmFinFunc(FILE* file) {
  fprintf(file,"pop rax\nmov rsp, rbp\npop rbp\nret \n\n");
}

void nasmFuncMainFin(FILE* file) {
  fprintf(file,"mov rsp, rbp\npop rbp\n");
} 

void nasmMainRet(FILE* file) {
  fprintf(file,"pop rdi \nmov rax,60\nsyscall\n");
}

void nasmLocVar(FILE* file,char* nom_fct){
  int i;
  for(i = 0; i < local_table_symbol.loc->size; i++){
    if(strcmp(local_table_symbol.loc[i].name,nom_fct) == 0){
      fprintf(file,"sub rsp, %d\n",8*local_table_symbol.loc[i].size_local);
    }
  }
}

void nasmGlobVar(FILE* file){
  fprintf(file,"section .bss\n");
  for(int i=0; i < global_table_symbol.size;i++){
    fprintf(file,"\t%s : resq 1 \n",global_table_symbol.entries[i].name);
  }
}

void nasmStart(FILE* file){	
  nasmGlobVar(file);
  fprintf(file,"section .text\nglobal main\n\n");
}
void translate(FILE *file, Node *node){
  Node* node1;
  Node* node2;
  Node* node3;
  Node* node4;
  Node* node5;
  Node* node6;
  Node* node7;
  Node* node8;
  Node* node9;
  Node* node10;
  Node* node11;
  if(node == NULL){
    exit(1);
  }
  switch (node -> kind){
    case Prog:
      if(FIRSTCHILD(node) != NULL){
        for(node1 = node->firstChild;node1;node1 = node1->nextSibling){
          translate(file,node1);
        }
      }
      break;
    case DeclFoncts:
      if(FIRSTCHILD(node) != NULL){
        for(node2 = node->firstChild;node2;node2 = node2->nextSibling){
          translate(file,node2);
        }
      } 
      break;
    case DeclFonct:
      if(FIRSTCHILD(node) != NULL){
        for(node3 = node->firstChild;node3;node3 = node3->nextSibling){
          translate(file,node3);
        }
      }
      break;
    case EnTeteFonct:
        if(FIRSTCHILD(node) != NULL){
          for(node4 = node->firstChild;node4;node4 = node4->nextSibling){
            if(node4 -> kind == Identifier){
              strcpy(copyTypes,node4->u.identifier);
              nasmDebutFunc(copyTypes,file);
            }
            translate(file,node4);
          }
        }
        nasmLocVar(file,copyTypes);
        break;
    case Corps:
        if(FIRSTCHILD(node) != NULL){
          for(node5 = node->firstChild;node5;node5= node5->nextSibling){
            translate(file,node5);
          }
        }
        break;
    case Operator :
      if(FIRSTCHILD(node) != NULL){
        switch(node -> u.character){
          case '+' :
            if(FIRSTCHILD(node) -> kind == Identifier){
              check_type(node, FIRSTCHILD(node)->u.identifier, "int");
              fprintf(file,"mov rax,QWORD[rbp - (8*%d)]\npush rax\n",returnIndice(copyTypes,FIRSTCHILD(node)->u.identifier));
              translate(file,FIRSTCHILD(node));
              if(SECONDCHILD(node) != NULL){
                if(SECONDCHILD(node) -> kind == Identifier){
                  check_type(node, SECONDCHILD(node)->u.identifier,"int");
                  fprintf(file,"mov rax,QWORD[rbp - (8*%d)]\npush rax\n",returnIndice(copyTypes,SECONDCHILD(node)->u.identifier));
                  translate(file,SECONDCHILD(node));
                }
                else if(SECONDCHILD(node) -> kind == CharLiteral){
                  fprintf(stderr,"warning error : oparation with char");
                }
                else{
                  translate(file,SECONDCHILD(node));
                }
              }
            }
            else if(FIRSTCHILD(node) -> kind == CharLiteral){
              fprintf(stderr,"warning error : oparation with char");
            }
            else{
              translate(file,FIRSTCHILD(node));
              translate(file,SECONDCHILD(node));
            }
            fprintf(file, ";E + T\npop rcx\npop rax\nadd rax,rcx\npush rax\n");
            break;
          case '-' :
            if(FIRSTCHILD(node) -> kind == Identifier){
              check_type(node, FIRSTCHILD(node)->u.identifier, "int");
              fprintf(file,"mov rax,QWORD[rbp - (8*%d)]\npush rax\n",returnIndice(copyTypes,FIRSTCHILD(node)->u.identifier));
              translate(file,FIRSTCHILD(node));
              if(SECONDCHILD(node) != NULL){
                if(SECONDCHILD(node) -> kind == Identifier){
                  check_type(node, SECONDCHILD(node)->u.identifier,"int");
                  fprintf(file,"mov rax,QWORD[rbp - (8*%d)]\npush rax\n",returnIndice(copyTypes,SECONDCHILD(node)->u.identifier));
                  translate(file,SECONDCHILD(node));
                }
                else if(SECONDCHILD(node) -> kind == CharLiteral){
                  fprintf(stderr,"warning error : oparation with char");
                }
                else{
                  translate(file,SECONDCHILD(node));
                }
              }
            }
            else if(FIRSTCHILD(node) -> kind == CharLiteral){
              fprintf(stderr,"warning error : oparation with char");
            }
            else{
              translate(file,FIRSTCHILD(node));
              translate(file,SECONDCHILD(node));
            }
            fprintf(file, ";E + T\npop rcx\npop rax\nadd rax,rcx\npush rax\n");

            break;
          case '*':
            if(FIRSTCHILD(node) -> kind == Identifier){
              check_type(node, FIRSTCHILD(node)->u.identifier, "int");
              fprintf(file,"mov rax,QWORD[rbp - (8*%d)]\npush rax\n",returnIndice(copyTypes,FIRSTCHILD(node)->u.identifier));
              translate(file,FIRSTCHILD(node));
              if(SECONDCHILD(node) != NULL){
                if(SECONDCHILD(node) -> kind == Identifier){
                  check_type(node, SECONDCHILD(node)->u.identifier,"int");
                  fprintf(file,"mov rax,QWORD[rbp - (8*%d)]\npush rax\n",returnIndice(copyTypes,SECONDCHILD(node)->u.identifier));
                  translate(file,SECONDCHILD(node));
                }
                else if(SECONDCHILD(node) -> kind == CharLiteral){
                  fprintf(stderr,"warning error : oparation with char");
                }
                else{
                  translate(file,SECONDCHILD(node));
                }
              }
              fprintf(file, ";E - T\npop rcx\npop rax\nsub rax,rcx\npush rax\n");
            }
            else if(FIRSTCHILD(node) -> kind == CharLiteral){
              fprintf(stderr,"warning error : oparation with char");
            }
            else{
              translate(file,FIRSTCHILD(node));
              translate(file,SECONDCHILD(node));
            }
            fprintf(file, ";E * T\npop rcx\npop rax\nimul rax,rcx\npush rax\n");
            break;
          case '/':
          case '%':
            if(FIRSTCHILD(node) -> kind == Identifier){
              check_type(node, FIRSTCHILD(node)->u.identifier, "int");
              fprintf(file,"mov rax,QWORD[rbp - (8*%d)]\npush rax\n",returnIndice(copyTypes,FIRSTCHILD(node)->u.identifier));
              translate(file,FIRSTCHILD(node));
              if(SECONDCHILD(node) != NULL){
                if(SECONDCHILD(node) -> kind == Identifier){
                  check_type(node, SECONDCHILD(node)->u.identifier,"int");
                  fprintf(file,"mov rax,QWORD[rbp - (8*%d)]\npush rax\n",returnIndice(copyTypes,SECONDCHILD(node)->u.identifier));
                  translate(file,SECONDCHILD(node));
                }
                else if(SECONDCHILD(node) -> kind == CharLiteral){
                  fprintf(stderr,"warning error : oparation with char");
                }
                else{
                  translate(file,SECONDCHILD(node));
                }
              }
            }
            else if(FIRSTCHILD(node) -> kind == CharLiteral){
              fprintf(stderr,"warning error : oparation with char");
            }
            else{
              translate(file,FIRSTCHILD(node));
              translate(file,SECONDCHILD(node));
            }
            fprintf(file, ";E / T\npop rax\npop rcx\nxor rdx,rdx\nidiv rcx\n");
            fprintf(file, "push %s\n", node->u.character == '/' ? "rax" : "rdx");
            break;
          default:
            exit(1);
        }
      }
    break;

    case Identifier :
      check_var(node,copyTypes,node->u.identifier,node->lineno);
      if(node->firstChild != NULL && node->nextSibling->kind == Parametres){
        nasmDebutFunc(node->u.identifier,file);
        translate(file,node->nextSibling);
        nasmFinFunc(file);
      }
      break;

    case IntLiteral :
      fprintf(file, ";IntLiteral\n");			
      fprintf(file,"push %d\n", node->u.integer);
      break;

    case CharLiteral :
      fprintf(file, "\t;CharLiteral\n");			
      fprintf(file,"push %c\n", node->u.character);
      break;

    case Not :
      if(FIRSTCHILD(node) != NULL){
        translate(file,FIRSTCHILD(node));
      }
      fprintf(file, ";Not");
      fprintf(file, "pop rax\nxor rax,1\npush rax\n");
      break;

    case Assign :
      if(FIRSTCHILD(node) -> kind == Operator){
        translate(file,FIRSTCHILD(node));
      }
      if(SECONDCHILD(node)->kind == Operator){
        translate(file,SECONDCHILD(node));
      }
      if(check_var(SECONDCHILD(node),copyTypes,SECONDCHILD(node)->u.identifier,SECONDCHILD(node)->lineno) == 1){
        fprintf(file,"mov rax, QWORD[rbp - (8*%d)]\n",returnIndice(copyTypes,SECONDCHILD(node)->u.identifier));
      }
      else if(check_var(SECONDCHILD(node),copyTypes,SECONDCHILD(node)->u.identifier,SECONDCHILD(node)->lineno) == 0){
        fprintf(file,"mov rax, QWORD[%s]\n",SECONDCHILD(node)->u.identifier);
      }
      else if(SECONDCHILD(node) -> kind == IntLiteral){
        translate(file,SECONDCHILD(node));
        if(check_var(FIRSTCHILD(node),copyTypes,FIRSTCHILD(node)->u.identifier,FIRSTCHILD(node)->lineno) == 0){
          fprintf(file,"mov QWORD[%s], %d\n",FIRSTCHILD(node)->u.identifier,SECONDCHILD(node)->u.integer);
        }
        else if(check_var(FIRSTCHILD(node),copyTypes,FIRSTCHILD(node)->u.identifier,FIRSTCHILD(node)->lineno) == 1){
          fprintf(file,"pop rax\nmov QWORD[rbp - (8*%d)],rax\n",returnIndice(copyTypes,FIRSTCHILD(node)->u.identifier));
        }
      }
      else if(SECONDCHILD(node) -> kind == CharLiteral){
        translate(file,SECONDCHILD(node));
        if(check_var(FIRSTCHILD(node),copyTypes,FIRSTCHILD(node)->u.identifier,FIRSTCHILD(node)->lineno) == 0){
          fprintf(file,"mov QWORD[%s], %d\n",FIRSTCHILD(node)->u.identifier,SECONDCHILD(node)->u.character);
        }
        else if(check_var(FIRSTCHILD(node),copyTypes,FIRSTCHILD(node)->u.identifier,FIRSTCHILD(node)->lineno) == 1){
          fprintf(file,"pop rax\nmov QWORD[rbp - (8*%d)],rax\n",returnIndice(copyTypes,FIRSTCHILD(node)->u.identifier));
        }
      }

    break;

    case Parametres:
      if(FIRSTCHILD(node) != NULL){
        for(node6 = node->firstChild;node6;node6 = node6->nextSibling){
          translate(file,node6);
        }
      }
      break;


    case Arguments:
      if(FIRSTCHILD(node) != NULL){
        for(node7 = node->firstChild;node7;node7 = node7->nextSibling){
          translate(file,node7);
        }
      }
      break;
    
    case Instructor:
      if(FIRSTCHILD(node) != NULL){
        if(strcmp(node -> u.identifier,"if") == 0){
          if(FIRSTCHILD(node) != NULL){
            for(node8 = node->firstChild;node8;node8 = node8->nextSibling){
              translate(file,node8);
            }
          }
        }
          
        else if(strcmp(node -> u.identifier,"else") == 0){
          for(node9 = node->firstChild;node9;node9 = node9->nextSibling){
            translate(file,node9);
          }
        }

        else if(strcmp(node -> u.identifier,"return") == 0){
          if(strcmp(copyTypes,"main") == 0){
            nasmFuncMainFin(file);
          } 
          translate(file,FIRSTCHILD(node));
          if(strcmp(copyTypes,"main") == 0){
            nasmMainRet(file);
          } 
          else{
            nasmFinFunc(file);
          }
        }

        else if(strcmp(node -> u.identifier,"readc") == 0){
          translate(file,FIRSTCHILD(node));
        }

        else if(strcmp(node -> u.identifier,"reade") == 0){
          translate(file,FIRSTCHILD(node));
        }

        else if(strcmp(node -> u.identifier,"while") == 0){
          for(node10 = node->firstChild;node10;node10 = node10->nextSibling){
            translate(file,node10);
          }
        }

        else if(strcmp(node -> u.identifier,"print") == 0){
          translate(file,FIRSTCHILD(node));
        }
        else {
          exit(1);
        }
      }

    break;

    case Compar :
      if(FIRSTCHILD(node) != NULL){
        if(strcmp(node -> u.identifier,"&&") == 0){
          translate(file,FIRSTCHILD(node));
          translate(file,SECONDCHILD(node));
        }
          
        else if(strcmp(node -> u.identifier,"||") == 0){
          translate(file,FIRSTCHILD(node));
          translate(file,SECONDCHILD(node));
        }

        else if(strcmp(node -> u.identifier,"==") == 0){
          translate(file,FIRSTCHILD(node));
          translate(file,SECONDCHILD(node));
        }

        else if(strcmp(node -> u.identifier,"!=") == 0){
          translate(file,FIRSTCHILD(node));
          translate(file,SECONDCHILD(node));
        }

        else if(strcmp(node -> u.identifier,">") == 0){
          translate(file,FIRSTCHILD(node));
          translate(file,SECONDCHILD(node));
        }

        else if(strcmp(node -> u.identifier,"<") == 0){
          translate(file,FIRSTCHILD(node));
          translate(file,SECONDCHILD(node));
        }

        else if(strcmp(node -> u.identifier,">=") == 0){
          translate(file,FIRSTCHILD(node));
          translate(file,SECONDCHILD(node));
        }
        else if(strcmp(node -> u.identifier,"<=") == 0){
          translate(file,FIRSTCHILD(node));
          translate(file,SECONDCHILD(node));
        }

        else{
          exit(1);
        }
      }

    break;

    case Type:
      if(FIRSTCHILD(node) != NULL){
        for(node11 = node->firstChild;node11;node11 = node11->nextSibling){
          translate(file,node11);
        }
      }
      break;

    case LValue :
      if(FIRSTCHILD(node) != NULL){
        translate(file,FIRSTCHILD(node));
        translate(file,SECONDCHILD(node));
      }
      break;

    default:
      exit(1);
      break;
    }
}
