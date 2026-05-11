/* tree.h */
#ifndef TREE_H
#define TREE_H
typedef enum {
  Prog,
  DeclVars,
  Declarateur,
  Declarateurs,
  DeclFoncts,
  DeclFonct,
  EnTeteFonct,
  Parametres,
  ListTypVar,
  Corps,
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
  Array,
  RETURn
} label_t;



typedef struct Node {
  label_t label;
  struct Node *firstChild, *nextSibling;
  int lineno;
  union{
    int numval;
    char charval;
    char identifiant[20];
    char type[20];
    char comparaison[5];
  }data;//structure pour stocker les donnèes 

} Node;

Node *makeNode(label_t label);
void setNodeType(Node *node, const char *type);
void setNodeIdentifier(Node *node, const char *identifiant);
void setNumVal(Node *node, int numval);
void setCharVal(Node *node, char charval);
void setNodeCompar(Node *node, const char *comparaison);
void addSibling(Node *node, Node *sibling);
void addChild(Node *parent, Node *child);
void deleteTree(Node*node);
void printTree(Node *node);




// #-----------------------------------------------

typedef struct {
    char *Type;
    char *Name;
} VAR_N;   //node de variable
 

typedef struct TABLE_VN {
    VAR_N data;
    struct TABLE_VN * next;
} TABLE_VN;   //table des variables




typedef struct {
    char *Type;
    char *Name;
    TABLE_VN *P_table;
    TABLE_VN *L_table;
    TABLE_VN *C_table;
    int Return   ;
} FONCT_N;    //node de fonction



typedef struct TABLE_FN {
    FONCT_N data;
    struct TABLE_FN* next;
} TABLE_FN;   //Table de fonctions  (forme dune liste cahinee)




int Tree_run(Node* node,TABLE_VN ** globa_t ,TABLE_FN ** fonca_t ) ;
void checkmain(TABLE_FN *table);
void checkDuplicateNames(TABLE_VN *table) ;
void checkD(TABLE_FN *table);
void checkNameConflict(TABLE_FN *fonca_t, TABLE_VN *globa_t) ;
void FunctionNames(TABLE_FN *fonca_t) ;





#define FIRSTCHILD(node) node->firstChild
#define SECONDCHILD(node) node->firstChild->nextSibling
#define THIRDCHILD(node) node->firstChild->nextSibling->nextSibling
#endif // TREE_H
