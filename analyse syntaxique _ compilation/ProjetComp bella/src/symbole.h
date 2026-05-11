#ifndef SYMBOLE_H
#define SYMBOLE_H
#include <stdbool.h>
#include <stdio.h>


//Symbole : Structure qui représente un symbole avec son nom, type, taille et décalage dans la mémoire.
typedef struct Symbole {
    char* nom;
    char* type; 
    int taille;
    int offset;
} Symbole;


//Fonction : Structure décrivant une fonction, incluant son nom, type de retour, paramètres et variables locales avec leur compte respectif.
typedef struct Fonction {
    char* nom;
    char* typeRetour;
    Symbole* parametres; 
    int nombreParametres;
    Symbole* variablesLocales; 
    int nombreVariablesLocales;
} Fonction;


//TableDesSymboles : Structure regroupant toutes les variables globales et fonctions du programme, avec des compteurs pour chacune
typedef struct TableDesSymboles {
    Symbole* variablesGlobales;
    int nombreVariablesGlobales;
    Fonction* fonctions;
    int nombreFonctions;
} TableDesSymboles;





/*************************************Table des Symboles**************************************/
TableDesSymboles* creerTableDesSymboles();
void ajouterVariableGlobale(TableDesSymboles* table, const char* nom, const char* type);
void ajouterVariableGlobaleTableau(TableDesSymboles* table, const char* nom, const char* type, int taille);
void ajouterFonction(TableDesSymboles* table, Fonction* fonction);
void libererTableDesSymboles(TableDesSymboles* table);
void ajouterVariableLocale(Fonction* fonction, const char* nom, const char* type);
void ajouterVariableLocaleTableau(Fonction* fonction, const char* nom, const char* type, const char* taille) ;
void ajouterParametreAFonction(Fonction* fonction, const char* nom, const char* type) ;
Fonction * initialiserFonction(const char* nom, const char* typeRetour);
void TraiterGlobale(Node* courant, TableDesSymboles* table);
void TraiterEnTeteFonct(Node* noeudEnTete, Fonction* fonction, TableDesSymboles* table);
void TraiterListeTypeVar(Node* listTypeVarNode, Fonction* fonction);
void parcourirArbre(Node* racine, TableDesSymboles* table);
void afficheTableDesSymboles(TableDesSymboles *table);
void setOffset(TableDesSymboles *table);



/*****************************************************Assembleur*****************************/
FILE *open_functions_file() ;
FILE *open_asm_file();
void gen_if(FILE* fasm, Node* comp, Node* elem1, Node* elem2, Node* true_branch, TableDesSymboles* table);
void gen_expr(FILE* fasm,Node* iden, Node* expr, TableDesSymboles* table);
void genererBss(TableDesSymboles* table, FILE* fichier) ;
void parcourassembleur(FILE* fasm, Node* racine,TableDesSymboles* table);
void close_asm_file(FILE *fasm,TableDesSymboles* table) ;
#endif
