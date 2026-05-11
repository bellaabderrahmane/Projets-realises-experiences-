/* tree.h */

typedef enum {
  /*les tokens*/
  Ident,
  Num,
  Character,
  Type,
  Void,
  If,
  Else,
  Return,
  While,
  Addsub,
  Divstar,
  Eq,
  Order,
  And,
  Or,
  Non,
  Affectation,
  /*les non-terminal*/
  Prog,
  DeclVars,
  Declarateurs,
  DeclFoncts,
  DeclFonct,
  EnTeteFonct,
  Parametres,
  ListTypVar,
  Corps,
  SuiteInstr,
  Instr,
  LValue,
  Exp,
  Arguments,
  TB,
  FB,
  M,
  E,
  T,
  F,
  ListExp

  
  /* list all other node labels, if any */
  /* The list must coincide with the string array in tree.c */
  /* To avoid listing them twice, see https://stackoverflow.com/a/10966395 */
} label_t;


typedef enum {
    IDENTIFIER,
    NUMBER,
    BYTE,
    COMP,
    NO_VALUE 
} ValueType;

 
typedef struct Node {
  label_t label;
  struct Node *firstChild, *nextSibling;
  int lineno;
  ValueType valueType;
   union {
        char ident[50];
        int number;
        char byte;
        char comp[3];
    } value;
} Node;

Node *makeNode(label_t label);
void addSibling(Node *node, Node *sibling);
void addChild(Node *parent, Node *child);
void deleteTree(Node*node);
void printTree(Node *node,int option);

#define FIRSTCHILD(node) node->firstChild
#define SECONDCHILD(node) node->firstChild->nextSibling
#define THIRDCHILD(node) node->firstChild->nextSibling->nextSibling
