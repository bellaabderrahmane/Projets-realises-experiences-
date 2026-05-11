/* tree.c */
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
extern int lineno;       /* from lexer */
 
static const char *StringFromLabel[] = {
  /*les tokens*/
  "Ident",
  "Num",
  "Character",
  "Type",
  "Void",
  "If",
  "Else",
  "Return",
  "While",
  "Addsub",
  "Divstar",
  "Eq",
  "Order",
  "And",
  "Or",
  "Non",
  "Affectation",
  /*les non-terminal*/
  "Prog",
  "DeclVars",
  "Declarateurs",
  "DeclFoncts",
  "DeclFonct",
  "EnTeteFonct",
  "Parametres",
  "ListTypVar",
  "Corps",
  "SuiteInstr",
  "Instr",
  "LValue",
  "Exp",
  "Arguments",
  "TB",
  "FB",
  "M",
  "E",
  "T",
  "F",
  "ListExp"
  /* list all other node labels, if any */
  /* The list must coincide with the label_t enum in tree.h */
  /* To avoid listing them twice, see https://stackoverflow.com/a/10966395 */
};

Node *makeNode(label_t label) {
  Node *node = malloc(sizeof(Node));
  if (!node) {
    printf("Run out of memory\n");
    exit(1);
  }
  node->label = label;
  node-> firstChild = node->nextSibling = NULL;
  node->lineno=lineno;
  node->valueType = NO_VALUE;
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


void printNodeValue(Node *node) {
    switch (node->valueType) {
        case IDENTIFIER:
            printf("%s", node->value.ident);
            break;
        case NUMBER:
            printf("%d", node->value.number);
            break;
        case BYTE:
            if(strchr("+-/=*%'!", node->value.byte) == NULL){
              printf("'%c'", node->value.byte);
            }else{
              printf("%c", node->value.byte);
            }
            break;
        case COMP:
            printf("%s", node->value.comp);
            break;
        case NO_VALUE:
            break;
    }
}


void printTree(Node *node,int option) {
  static bool rightmost[128]; // tells if node is rightmost sibling
  static int depth = 0;       // depth of current node
  for (int i = 1; i < depth; i++) { // 2502 = vertical line
    printf(rightmost[i] ? "    " : "\u2502   ");
  }
  if (depth > 0) { // 2514 = L form; 2500 = horizontal line; 251c = vertical line and right horiz 
    printf(rightmost[depth] ? "\u2514\u2500\u2500 " : "\u251c\u2500\u2500 ");
  }

  if ( node->valueType == NO_VALUE || option == 0){
      printf("%s", StringFromLabel[node->label]);
  }else{
      printNodeValue(node);
  }
  printf("\n");
  depth++;
  for (Node *child = node->firstChild; child != NULL; child = child->nextSibling) {
    rightmost[depth] = (child->nextSibling) ? false : true;
    printTree(child,option);
  }
  depth--;
}
