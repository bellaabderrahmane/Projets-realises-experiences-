/* tree.c */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
#include "../obj/le_parser.h"
extern YYSTYPE yylval;
extern int yylex();
extern int lineno;       /* from lexer */
static const char *StringFromLabel[] = {
    "Prog",
    "DeclVars",
    "Declarateur",
    "Declarateurs",
    "DeclFoncts",
    "DeclFonct",
    "EnTeteFonct",
    "Parametres",
    "ListTypVar",
    "Corps",
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
    "Array",
    "RETURn"
};


Node *makeNode(label_t label) {
    Node *node = malloc(sizeof(Node));
    if (!node) {
        printf("Run out of memory\n");
        exit(1);
    }
    node->label = label;
    node->firstChild = node->nextSibling = NULL;
    node->lineno = lineno;
    return node;
}

//une fonction qui recupère le type
void setNodeType(Node *node, const char *type) {
    if (node != NULL && type != NULL) {
        strcpy(node->data.type, type);
    }
}

//une fonction qui recupère l'identifiant
void setNodeIdentifier(Node *node, const char *identifiant) {
    if (node != NULL && identifiant != NULL) {
        strcpy(node->data.identifiant, identifiant);
    }
}

//une fonction qui recupère NUM

void setNumVal(Node *node, int numval) {
    if (node != NULL) {
        node->data.numval = numval;
    }
}

//une fonction qui recupère Char

void setCharVal(Node *node, char charval) {
    if (node != NULL) {
        node->data.charval = charval;
    }
}
//une fonction qui recupère comparaison

void setNodeCompar(Node *node, const char *comparaison) {
    if (node != NULL && comparaison != NULL) {
        strcpy(node->data.comparaison, comparaison);
    }
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
  
  static bool rightmost[128]; // tells if node is rightmost sibling
  static int depth = 0;       // depth of current node
  for (int i = 1; i < depth; i++) { // 2502 = vertical line
    printf(rightmost[i] ? "    " : "\u2502   ");
  }
  if (depth > 0) { // 2514 = L form; 2500 = horizontal line; 251c = vertical line and right horiz 
    printf(rightmost[depth] ? "\u2514\u2500\u2500 " : "\u251c\u2500\u2500 ");
  }

  switch ((enum yytokentype)node->label) {//Switch pour gèrer  tout les types des tokens
    case TYPE:
        printf("%s", node->data.type);
        break;
    case IDENT:
        printf("%s ", node->data.identifiant);
        break;
    case NUM:
        printf("%d ", node->data.numval);
        break;
    case ADDSUB:
    case CHARACTER:
    case DIVSTAR:
        printf("%c ", node->data.charval);
        break;
    case EQ:
    case ORDER:
        printf("%s ", node->data.comparaison);
        break;
    case IF:
        printf("if ");
        break;
    case ELSE:
        printf("else ");
        break;
    case RETURN:
        printf("return ");
        break;
    case VOID:
        printf("void ");
        break;
    case WHILE:
        printf("while ");
        break;
    case AND:
        printf("&& ");
        break;
    case OR:
        printf("|| ");
        break;
    case EQUALS:
        printf("="); 
        break;
     case EXCLAM:
        printf("!");
        break; 
    default:
        break;
}


  if (node != NULL && StringFromLabel[node->label] !=NULL) {
        printf("%s", StringFromLabel[node->label]); 
    }   

  
  printf("\n");
  depth++;
  for (Node *child = node->firstChild; child != NULL; child = child->nextSibling) {
    rightmost[depth] = (child->nextSibling) ? false : true;
    printTree(child);
  }
  depth--;
}











void addVarToTable(TABLE_VN **table, char *type, char *name) {
    TABLE_VN *newNode = (TABLE_VN *)malloc(sizeof(TABLE_VN));
    newNode->data.Type = strdup(type);
    newNode->data.Name = strdup(name);
    newNode->next = *table;
    *table = newNode;
}

// Function to process the children of DeclVars node
void processDeclVars(Node *declVarsNode, TABLE_VN **table) {
    Node *child = declVarsNode->firstChild;
    while (child != NULL) {
        if (child->label == Declarateurs) {
            Node *declarateur = child->firstChild;

            while (declarateur != NULL) {
                if (declarateur->label == Array) {
                    Node *arrayChild = declarateur->firstChild;
                    char *arrayName = arrayChild->data.identifiant;
                    arrayChild = arrayChild->nextSibling;
                    if (arrayChild->data.numval > 0) {
                        addVarToTable(table, "Array", arrayName);
                    }else if( arrayChild->data.numval <= 0 ){
                        printf("non valid table size \n");
                        exit(2) ;
                    }
                } else {
                    addVarToTable(table, "Ident", declarateur->data.identifiant);
                }

                declarateur = declarateur->nextSibling;
            }
        }
        child = child->nextSibling;
    }
}


void printTable(TABLE_VN *table) {
    TABLE_VN *current = table;
    printf("[  " ) ;
    while (current != NULL) {
        printf("( %s )  ",  current->data.Name);
        current = current->next;
    }
    printf(" ] \n " ) ;
}



void iterateAndUpdate(Node *node, TABLE_FN **globa_t) {
    if (node == NULL) // Base case: if the node is NULL, return
        return;
    Node *current = node ;

    if ( node != NULL && node->label != Array) {
        // If the label is not Type or Array, store the value of the first child in globa_t
        if (node != NULL && node->data.identifiant != NULL) {
           // printf(" %s   ddddddddddd \n"  ,node->nextSibling->data.identifiant ) ;
            addVarToTable(&(*globa_t)->data.P_table, node->data.identifiant, node->nextSibling->data.identifiant) ;   
        }
        current = node->nextSibling->nextSibling ;
    }else {
        current = node->nextSibling ;

    }

    // Recursively call the function for the first child and then for the next sibling
    iterateAndUpdate(current, globa_t);
}



void addVarToFUNCT(TABLE_FN **table, char *type, char *name,Node * node) {
    TABLE_FN *newNode = (TABLE_FN *)malloc(sizeof(TABLE_FN));
    newNode->data.Type = strdup(type);
    newNode->data.Name = strdup(name);
    if( node->label == ListTypVar){          
        iterateAndUpdate(node->firstChild, &newNode) ;  
    }
    newNode->next = *table;
    *table = newNode;
}


void printTableFF(TABLE_FN *table) {
    TABLE_FN *current = table;
    printf("funct  -------- :  \n" ) ;
    while (current != NULL) {
        printf("( %s )  \n",  current->data.Name);
        printTable(current->data.P_table) ;
        printTable(current->data.L_table) ;
        current = current->next;
    }
    printf(" ------------- \n " ) ;
}


void checkmain(TABLE_FN *table) {
     
    if(table == NULL){
        return ;
    }
    TABLE_FN *current = table;
    if( strcmp(current->data.Name, "main") != 0 || strcmp(current->data.Type, "int") != 0 ){
       printf("main not found\n");
       exit(2); 
    }
}



void checkDuplicateNames(TABLE_VN *table) {
    TABLE_VN *current = table;
    // Iterate through the table
    while (current != NULL) {
        TABLE_VN *runner = current->next;
        // Check if current node's name matches any subsequent node's name
        while (runner != NULL) {
            if (strcmp(current->data.Name, runner->data.Name) == 0) {
                fprintf(stderr, "Error: Duplicate name '%s' found\n", current->data.Name);
                exit(2);
            }
            runner = runner->next;
        }
        current = current->next;
    }
}


void checkD(TABLE_FN *table) {
    TABLE_FN *current = table;

    // Iterate through the table
    while (current != NULL) {
        TABLE_FN *runner = current->next;
        // Check if current node's name matches any subsequent node's name
        while (runner != NULL) {
            if (strcmp(current->data.Name, runner->data.Name) == 0) {
                fprintf(stderr, "Error: Duplicate name '%s' found\n", current->data.Name);
                exit(2);
            }
            runner = runner->next;
        }
        current = current->next;
    }
}


void checkNameConflict(TABLE_FN *fonca_t, TABLE_VN *globa_t) {
    TABLE_FN *current_fonca = fonca_t;

    // Iterate through fonca_t
    while (current_fonca != NULL) {
        TABLE_VN *current_globa = globa_t;

        // Iterate through globa_t for each node in fonca_t
        while (current_globa != NULL) {
            if (strcmp(current_fonca->data.Name, current_globa->data.Name) == 0) {
                fprintf(stderr, "Error: Name conflict detected between '%s' in fonca_t and globa_t\n", current_fonca->data.Name);
                exit(2);
            }
            current_globa = current_globa->next;
        }

        current_fonca = current_fonca->next;
    }
}






void checkDuplica(TABLE_VN *globa_t, TABLE_VN *tabl1) {
    TABLE_VN *current_globa = globa_t;
    
    while (current_globa != NULL) {
        TABLE_VN *current_tabl1 = tabl1;
        
        while (current_tabl1 != NULL) {
            // Compare the names of the current nodes in globa_t and tabl1
            if (strcmp(current_globa->data.Name, current_tabl1->data.Name) == 0) {
                // If a match is found, exit with error code 2
                fprintf(stderr, "Error: Duplicate name found: %s\n", current_globa->data.Name);
                exit(2);
            }
            
            current_tabl1 = current_tabl1->next;
        }
        
        current_globa = current_globa->next;
    }
}

void FunctionNames(TABLE_FN *fonca_t) {
    TABLE_FN *current = fonca_t;
    
    while (current != NULL) {
        checkDuplicateNames(current->data.L_table);   //check dupe
        checkDuplicateNames(current->data.P_table);   //check dupe
         
        checkDuplica(current->data.L_table, current->data.P_table);
        current = current->next;
    }
}






int getallch(Node* node ,TABLE_VN** table ,int * return_c) {
    if (node == NULL) {
        return 0;
    }

    

    if((enum yytokentype)node->label == RETURN){
        if(node->firstChild == NULL){
            *return_c = 1 ;
        }else if(node->firstChild != NULL){
            *return_c = 2 ;
        }   
    }
    
    if(node->label == Array && node->firstChild == NULL){
        //exit(2) ;   cyheck here
    }else if ((enum yytokentype)node->label == IDENT ) {
         addVarToTable(table, "ident", node->data.identifiant);;
    }

    Node* child = node->firstChild;
    while (child != NULL) {
        getallch(child, table ,return_c);
        child = child->nextSibling;
    }
    return 0;
}










int hasMatching(TABLE_VN* sNode, TABLE_VN* sNode1) {
    TABLE_VN* sCurrent = sNode;
    TABLE_VN* sCurrent1 = NULL;
    while (sCurrent != NULL) {
        sCurrent1 = sNode1;
        if (  (strlen(sCurrent->data.Name) != 0) && strcmp(sCurrent->data.Name, sCurrent1->data.Name) == 0) {
            return 2; // Found a matching name
        }
        sCurrent = sCurrent->next;
    }
    return 0; // No matching names found
}

int eeeeeeeeeeee(TABLE_VN* sNode, char * name1) {
    TABLE_VN* sCurrent = sNode;
    while (sCurrent != NULL) {
        if (  (strlen(sCurrent->data.Name) != 0) && strcmp(sCurrent->data.Name, name1) == 0) {
            //exit(-1) ;
            return 2; // Found a matching name
        }
        sCurrent = sCurrent->next;
    }
    return 0; // No matching names found
}
  

int Para_loca(TABLE_FN* fNode, TABLE_VN* sNode) {
    // Pointer to traverse the list
    TABLE_FN* current = fNode;
    // Traverse each node in the list
    
    while (current != NULL) {
        TABLE_VN* cTableNode = current->data.C_table;
        // Traverse each node in the C_table
        while (cTableNode != NULL) {
            // Check if the current C_table node exists in either L_table or P_table
            int para = !hasMatching(current->data.P_table, cTableNode);
            int loca = !hasMatching(current->data.L_table, cTableNode);
            if (  loca && para  && eeeeeeeeeeee(sNode, cTableNode->data.Name) != 2  ) {
                         exit(2);
            }
            cTableNode = cTableNode->next; // Move to the next node in C_table
        }
        current = current->next; 
    }
    return 0;
}











int Tree_run(Node* node,TABLE_VN ** globa_t ,TABLE_FN ** fonca_t ) {
    if (node == NULL) {
        return 0;
    }
    
    if (node->label == DeclVars) {
        
        processDeclVars(node, globa_t);
    } 


    if (node->label == DeclFoncts) {
    
        int return_c = 0 ;


        Node* loca = node->firstChild;
        
        while (loca != NULL) {





            

            if (loca->label == DeclFonct) {
                Node* firstChild = loca->firstChild ;
                Node* type_f = firstChild->firstChild;
                Node* name_f = type_f->nextSibling;
                
                Node* entete = name_f->nextSibling;
                addVarToFUNCT(fonca_t, type_f->data.identifiant, name_f->data.identifiant,entete) ;

                Node* corps = loca->firstChild->nextSibling;

                if(corps->firstChild != NULL && corps->firstChild->label == DeclVars){

                    processDeclVars(corps->firstChild, &(*fonca_t)->data.L_table);
                    
                    Node* Suite = corps->firstChild->nextSibling;

                    if( Suite != NULL &&  Suite->firstChild != NULL && Suite->label == SuiteInstr){
                        
                        getallch(Suite ,&(*fonca_t)->data.C_table , &return_c) ;
                        
                        
                        if((*fonca_t)->data.C_table != NULL){
                            int para = !hasMatching((*fonca_t)->data.P_table, (*fonca_t)->data.C_table);
                            int loca = !hasMatching((*fonca_t)->data.L_table, (*fonca_t)->data.C_table);
                            if (  loca && para    ) {
                                         exit(2);
                            }   
                       }
                        
                        
                    }

                }else if (  corps->firstChild != NULL && corps->firstChild->label == SuiteInstr ) {
                    getallch(corps->firstChild ,&(*fonca_t)->data.C_table ,&return_c) ;
                   
                    if((*fonca_t)->data.C_table != NULL){
                        int para = !hasMatching((*fonca_t)->data.P_table, (*fonca_t)->data.C_table);
                        int loca = !hasMatching((*fonca_t)->data.L_table, (*fonca_t)->data.C_table);
                        if (  loca && para    ) {
                                     exit(2);
                        }   
                    }
                    
                }
                


            }





        loca = loca->nextSibling;
        }

        
        
    

    }
    




    int result = Tree_run(node->nextSibling, globa_t ,fonca_t);
    return (result == 2) ? 2 : 0;
}