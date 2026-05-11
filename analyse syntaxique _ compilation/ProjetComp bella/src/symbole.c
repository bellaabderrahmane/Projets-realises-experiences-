#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "tree.h"
#include "symbole.h"
#include "../obj/le_parser.h"
extern YYSTYPE yylval;
extern int yylex();
extern int lineno;   






/*************************************Fonctions de gestion de la table des symboles et des fonctions*********************************************/


//Crée une table des symboles vide avec des listes initialisées à NULL et des compteurs à zéro
TableDesSymboles* creerTableDesSymboles() {
    TableDesSymboles* table = (TableDesSymboles*)malloc(sizeof(TableDesSymboles));
    table->variablesGlobales = NULL;
    table->nombreVariablesGlobales = 0;
    table->fonctions = NULL;
    table->nombreFonctions = 0;
    return table;
}

//Ajoute une variable globale à la table des symboles.
void ajouterVariableGlobale(TableDesSymboles* table, const char* nom, const char* type) {
    table->variablesGlobales = (Symbole*)realloc(table->variablesGlobales, (table->nombreVariablesGlobales + 1) * sizeof(Symbole));
    Symbole* nouvelleVariable = &table->variablesGlobales[table->nombreVariablesGlobales++];
    nouvelleVariable->nom = strdup(nom); 
    nouvelleVariable->type = strdup(type);
}

//Ajoute une variable globale de type tableau en spécifiant taille et type.
void ajouterVariableGlobaleTableau(TableDesSymboles* table, const char* nom, const char* type, int taille) {
    table->variablesGlobales = (Symbole*)realloc(table->variablesGlobales, (table->nombreVariablesGlobales + 1) * sizeof(Symbole));
    Symbole* nouvelleVariable = &table->variablesGlobales[table->nombreVariablesGlobales++];
    nouvelleVariable->nom = strdup(nom);  
    size_t newSize = strlen(type) + strlen("Array") + 1;
    nouvelleVariable->type = malloc(newSize);
    if (nouvelleVariable->type == NULL) {
    fprintf(stderr, "Erreur d'allocation de mémoire\n");
    return; 
    }
    strcpy(nouvelleVariable->type, type);
    nouvelleVariable->taille = taille;
    strcat(nouvelleVariable->type, " Array");

}


//Crée une structure Fonction avec des listes de paramètres et variables locales initialisées à NULL.
Fonction* initialiserFonction(const char* nom, const char* typeRetour) {
    Fonction* fonction = (Fonction*) malloc(sizeof(Fonction));
    if (fonction == NULL) {
        fprintf(stderr, "Erreur d'allocation mémoire pour la fonction.\n");
        return NULL;  
    }

    fonction->nom = nom ? strdup(nom) : NULL;  
    fonction->typeRetour = typeRetour ? strdup(typeRetour) : NULL;  
    fonction->parametres = NULL;
    fonction->nombreParametres = 0;
    fonction->variablesLocales = NULL;
    fonction->nombreVariablesLocales = 0;

    return fonction;
}



//Ajoute une variable locale à une fonction existante.
void ajouterVariableLocale(Fonction* fonction, const char* nom, const char* type) {
    if (fonction == NULL || nom == NULL || type == NULL) {
        printf("Erreur: paramètre invalide.\n");
        return;
    }

    Symbole* temp = realloc(fonction->variablesLocales, (fonction->nombreVariablesLocales + 1) * sizeof(Symbole));
    if (temp == NULL) {
        printf("Échec de l'allocation de mémoire pour ajouter une variable locale.\n");
        return;
    }
    fonction->variablesLocales = temp;
    Symbole* nouvelleVariable = &fonction->variablesLocales[fonction->nombreVariablesLocales++];
    nouvelleVariable->nom = strdup(nom);
    nouvelleVariable->type = strdup(type);
}



//Ajoute une variable locale de type tableau à une fonction.
void ajouterVariableLocaleTableau(Fonction* fonction, const char* nom, const char* type, const char* taille) {
    if (fonction == NULL || nom == NULL || type == NULL || taille == NULL) {
        printf("Erreur: paramètre invalide.\n");
        return;
    }

    char typeComplet[256]; 
    snprintf(typeComplet, sizeof(typeComplet), "%s Array", type);

    Symbole* temp = realloc(fonction->variablesLocales, (fonction->nombreVariablesLocales + 1) * sizeof(Symbole));
    if (temp == NULL) {
        printf("Échec de l'allocation de mémoire pour ajouter un tableau local.\n");
        return;
    }
    fonction->variablesLocales = temp;
    Symbole* nouvelleVariable = &fonction->variablesLocales[fonction->nombreVariablesLocales++];
    nouvelleVariable->nom = strdup(nom);
    nouvelleVariable->type = strdup(typeComplet);
    nouvelleVariable->taille = atoi(taille);
}





//Ajoute un paramètre à la définition d'une fonction.
void ajouterParametreAFonction(Fonction* fonction, const char* type, const char* nom) {
    if (fonction == NULL || type == NULL || nom == NULL) {
        printf("Paramètre invalide.\n");
        return;
    }

    Symbole* temp = realloc(fonction->parametres, (fonction->nombreParametres + 1) * sizeof(Symbole));
    if (temp == NULL) {
        printf("Échec de l'allocation de mémoire pour ajouter un paramètre.\n");
        return;
    }
    fonction->parametres = temp;
    int index = fonction->nombreParametres;
    fonction->parametres[index].nom = strdup(nom);
    fonction->parametres[index].type = strdup(type);
    fonction->nombreParametres++;
}




//Intègre une nouvelle fonction dans la table des symboles.
void ajouterFonction(TableDesSymboles* table, Fonction* fonction) {
    Fonction* temp = realloc(table->fonctions, (table->nombreFonctions + 1) * sizeof(Fonction));
    if (temp == NULL) {
        printf("Échec de l'allocation de mémoire pour les fonctions.\n");
        return;
    }
    table->fonctions = temp;
    Fonction* nouvelleFonction = &table->fonctions[table->nombreFonctions++];
    *nouvelleFonction = *fonction;  
}













/****************************************************************Fonctions de traitement de l'arbre abstrait***************************************************/



//Traite les déclarations de variables globales dans l'arbre abstrait et les ajoute à la table des symboles.
void TraiterGlobale(Node* noeudCourant, TableDesSymboles* table) {
    if (noeudCourant == NULL) {
        printf("Erreur: Noeud courant est NULL.\n");
        return;
    }
    if (noeudCourant->label == DeclVars) {
        Node* typeNode = noeudCourant->firstChild; 
        char* currentType = NULL;  
        while (typeNode != NULL) {
            if ((enum yytokentype)typeNode->label == TYPE) {
                currentType = typeNode->data.type;  
            } else if (typeNode->label == Declarateurs && currentType != NULL) {
                Node* declarateur = typeNode->firstChild;
                while (declarateur != NULL) {
                    if (declarateur->label == Array) {  
                        Node* nomNode = declarateur->firstChild;
                        Node* tailleNode = nomNode->nextSibling;
                        ajouterVariableGlobaleTableau(table, nomNode->data.identifiant, currentType, tailleNode->data.numval);
                    } else {
                        ajouterVariableGlobale(table, declarateur->data.identifiant, currentType);
                    }
                    declarateur = declarateur->nextSibling;
                }
            }
            typeNode = typeNode->nextSibling;  
        }
    } else {
    }
}




//Traite les déclarations de variables locales pour une fonction spécifique.
void TraiterLocale(Node* noeudCourant, Fonction* fonctionCourante) {
    if (noeudCourant == NULL) {
        printf("Erreur: Noeud courant est NULL.\n");
        return;
    }
    noeudCourant = noeudCourant -> firstChild;
    if (noeudCourant->label == DeclVars) {
        Node* typeNode = noeudCourant->firstChild;
        char* currentType = NULL;
        while (typeNode != NULL) {
            if ((enum yytokentype)typeNode->label == TYPE) {
                currentType = typeNode->data.type;
            } else if (typeNode->label == Declarateurs && currentType != NULL) {
                Node* declarateur = typeNode->firstChild;
                while (declarateur != NULL) {
                    if (declarateur->label == Array) {  
                        Node* nomNode = declarateur->firstChild;
                        Node* tailleNode = nomNode->nextSibling;
                        char tailleStr[9]; 
                        snprintf(tailleStr, sizeof(tailleStr), "%d", tailleNode->data.numval);
                        ajouterVariableLocaleTableau(fonctionCourante, nomNode->data.identifiant, currentType, tailleStr);
                    } else {
                        ajouterVariableLocale(fonctionCourante, declarateur->data.identifiant, currentType);
                    }
                    declarateur = declarateur->nextSibling;
                }
            }
            typeNode = typeNode->nextSibling;
        }
    } else {
       
    }
}




//Traite les listes de variables typées pour extraire les paramètres d'une fonction.
void TraiterListeTypeVar(Node* listTypeVarNode, Fonction* fonction) {
    Node* currentNode = listTypeVarNode->firstChild;
    while (currentNode != NULL) {
        char* currentType = currentNode->data.type;  
        Node* declarateurNode = currentNode->nextSibling; 
            char nomDeclarateur[128];  
            strcpy(nomDeclarateur, declarateurNode->data.identifiant);
            Node* nextNode = declarateurNode->nextSibling;
            if (nextNode != NULL && nextNode->label == Array) {
                char typeArray[128];
                snprintf(typeArray, sizeof(typeArray), "%s Array", currentType);
                ajouterParametreAFonction(fonction, typeArray, nomDeclarateur);
                currentNode = nextNode->nextSibling; 
            } else {
                ajouterParametreAFonction(fonction, currentType, nomDeclarateur);
                currentNode = declarateurNode->nextSibling; 
            }
        } 
    }








//Traite l'en-tête d'une fonction pour extraire les informations de base et les paramètres.
void TraiterEnTeteFonct(Node* noeudEnTete, Fonction* fonction, TableDesSymboles* table) {
    if (noeudEnTete == NULL || fonction == NULL) {
        printf("Erreur: Noeud d'en-tête ou structure de fonction est NULL.\n");
        return;
    }

    if (noeudEnTete->firstChild->label != EnTeteFonct) {
        printf("Erreur: Le noeud passé n'est pas un noeud EnTeteFonct.\n");
        return;
    }
    Node* EnTeteFonct = noeudEnTete->firstChild;
    Node* typeNode = EnTeteFonct->firstChild;
    Node* nomNode = typeNode->nextSibling;
    Node* listTypeVarNode = nomNode->nextSibling;
    if (listTypeVarNode == NULL || listTypeVarNode->label != ListTypVar) {
        return;
    }
    fonction->nom = strdup(nomNode->data.identifiant);
    fonction->typeRetour = strdup(typeNode->data.type);
    TraiterListeTypeVar(listTypeVarNode, fonction);
}













/**************************************Fonctions pour le parcours de l'arbre et la gestion des décalages mémoire***************************************/

/*Parcourt l'arbre abstrait syntaxique pour traiter les déclarations de variables et de fonctions.*/
void parcourirArbre(Node* racine, TableDesSymboles* table) {
    if (racine == NULL) return;

    Node* noeudCourant = racine->firstChild;
    while (noeudCourant != NULL) {
        switch (noeudCourant->label) {
            case DeclVars:
            {
                TraiterGlobale(noeudCourant, table); // Traitement des variables globales
            } 
                break;
            case DeclFoncts:
            {
                Node* declFonctNode = noeudCourant->firstChild;
                while (declFonctNode != NULL) {
                    if (declFonctNode->label == DeclFonct) {
                        Fonction* nouvelleFonction = (Fonction*) malloc(sizeof(Fonction));
                        if (nouvelleFonction == NULL) {
                            printf("Erreur: Échec de l'allocation mémoire pour une nouvelle fonction.\n");
                        } else {
                            nouvelleFonction->nom = NULL;
                            nouvelleFonction->typeRetour = NULL;
                            nouvelleFonction->parametres = NULL;
                            nouvelleFonction->nombreParametres = 0;
                            nouvelleFonction->variablesLocales = NULL;
                            nouvelleFonction->nombreVariablesLocales = 0;
                            
                            TraiterEnTeteFonct(declFonctNode, nouvelleFonction, table);
                            Node* corpsNode = declFonctNode->firstChild->nextSibling;
                            TraiterLocale(corpsNode, nouvelleFonction);
                            ajouterFonction(table, nouvelleFonction);
                        }
                    }
                    declFonctNode = declFonctNode->nextSibling;
                }
            }
                break;
            default:
                
                break;
        }
        noeudCourant = noeudCourant->nextSibling;
    }
    setOffset(table);
}









//setOffset : Calcule et affecte des décalages de mémoire pour les variables globales, locales, et les paramètres des fonctions.
void setOffset(TableDesSymboles *table) {
    int offset = 0;
    
    // Calcul des offsets pour les variables globales
    for (int i = 0; i < table->nombreVariablesGlobales; i++) {
        if (strcmp(table->variablesGlobales[i].type, "int") == 0) {
            offset += 4;
            table->variablesGlobales[i].offset = offset;
        }
        else if (strcmp(table->variablesGlobales[i].type, "char") == 0) {
            offset += 1;
            table->variablesGlobales[i].offset = offset;
        }
        else if (strcmp(table->variablesGlobales[i].type, "int Array") == 0) {
            offset += 4 * table->variablesGlobales[i].taille;
            table->variablesGlobales[i].offset = offset;
        }
        else if (strcmp(table->variablesGlobales[i].type, "char Array") == 0) {
            offset += 1 * table->variablesGlobales[i].taille;
            table->variablesGlobales[i].offset = offset;
        }
    }
    
    // Calcul des offsets pour les paramètres des fonctions
    for (int i = 0; i < table->nombreFonctions; i++) {
        Fonction *func = &table->fonctions[i];
        for (int j = 0; j < func->nombreParametres; j++) {
            Symbole *param = &func->parametres[j];
            if (strcmp(param->type, "int") == 0 || strcmp(param->type, "int Array") == 0) {
                offset += 4;
                param->offset = offset;
            }
            else if (strcmp(param->type, "char") == 0 || strcmp(param->type, "char Array") == 0) {
                offset += 1;
                param->offset = offset;
            }
        }
        
        // Calcul des offsets pour les variables locales des fonctions
        for (int k = 0; k < func->nombreVariablesLocales; k++) {
            Symbole *var = &func->variablesLocales[k];
            if (strcmp(var->type, "int") == 0) {
                offset += 4;
                var->offset = offset;
            }
            else if (strcmp(var->type, "char") == 0) {
                offset += 1;
                var->offset = offset;
            }
            else if (strcmp(var->type, "int Array") == 0) {
                offset += 4 * var->taille;
                var->offset = offset;
            }
            else if (strcmp(var->type, "char Array") == 0) {
                offset += 1 * var->taille;
                var->offset = offset;
            }
        }
    }
}






//Affiche le contenu de la table des symboles pour les variables et les fonctions, y compris les paramètres et les variables locales
void afficheTableDesSymboles(TableDesSymboles *table) {
    // Affichage des variables globales
    for (int i = 0; i < table->nombreVariablesGlobales; i++) {
        if(table->variablesGlobales[i].taille > 0){
            printf("Variable globale: %s, Type: %s taille: %d offset: %d\n", table->variablesGlobales[i].nom, table->variablesGlobales[i].type, table->variablesGlobales[i].taille,table->variablesGlobales[i].offset);

        }
        else{
            printf("Variable globale: %s, Type: %s offset : %d \n", table->variablesGlobales[i].nom, table->variablesGlobales[i].type,table->variablesGlobales[i].offset);

        }
        
    }

    // Affichage des fonctions
    for (int i = 0; i < table->nombreFonctions; i++) {
        Fonction func = table->fonctions[i];
        printf("Fonction: %s, Type de retour: %s\n", func.nom, func.typeRetour);
        printf("  Paramètres (%d):\n", func.nombreParametres);
        for (int j = 0; j < func.nombreParametres; j++) {
            Symbole param = func.parametres[j];
            printf("    %d. %s : %s  offset:%d\n", j + 1, param.nom, param.type,param.offset);
        }
        printf("  Variables locales (%d):\n", func.nombreVariablesLocales);
        for (int k = 0; k < func.nombreVariablesLocales; k++) {
            Symbole var = func.variablesLocales[k];
            if (var.taille > 0) {
                printf("    %d. %s : %s  taille: %d  offset:%d\n", k + 1, var.nom, var.type,var.taille,var.offset);
            }
            else{
                printf("    %d. %s : %s  offset:%d\n", k + 1, var.nom, var.type,var.offset);

            }
        }
    }
}

















/***************************************************Generation de code pour l'assembleur*********************************/















//open_functions_file : Ouvre un fichier pour écrire du code assembleur, 
//initialise les sections .data, .bss, et .text, et déclare des fonctions système de base pour les opérations d'entrée/sortie.
FILE *open_functions_file() {
    FILE *fasm = fopen("my_functions.asm", "w");
    if (!fasm) {
        perror("Failed to open file for writing assembly code");
        exit(EXIT_FAILURE);
    }
    // Initialisation des sections et des points d'entrée pour les fonctions
    fprintf(fasm, "section .data\n");
    fprintf(fasm, "    digits db \"0123456789\"\n");
    fprintf(fasm, "    error_message db \"Error: Input is not a digit.\", 10\n");
    fprintf(fasm, "    error_length equ $ - error_message\n\n");
    
    fprintf(fasm, "section .bss\n");
    fprintf(fasm, "    buffer resb 16         \n");
    fprintf(fasm, "    char_buffer resb 1     \n\n");
    
    fprintf(fasm, "section .text\n");
    fprintf(fasm, "global putchar\n");
    fprintf(fasm, "global putint\n");
    fprintf(fasm, "global getchar\n");
    fprintf(fasm, "global getint\n\n");

    // Fonction putchar
    fprintf(fasm, "putchar:\n");
    fprintf(fasm, "    mov     rax, 1          \n");
    fprintf(fasm, "    mov     rdi, 1          \n");
    fprintf(fasm, "    mov     rsi, rdi        \n");
    fprintf(fasm, "    mov     rdx, 1          \n");
    fprintf(fasm, "    syscall                 \n");
    fprintf(fasm, "    ret                     \n\n");

    // Fonction putint
    fprintf(fasm, "putint:\n");
    fprintf(fasm, "    mov     rax, rdi        \n");
    fprintf(fasm, "    mov     rsi, buffer + 19 \n");
    fprintf(fasm, "    mov     byte [rsi], 0    \n\n");
    fprintf(fasm, ".print_loop:\n");
    fprintf(fasm, "    mov     rbx, 10\n");
    fprintf(fasm, "    div     rbx             \n");
    fprintf(fasm, "    add     dl, '0'         \n");
    fprintf(fasm, "    dec     rsi             \n");
    fprintf(fasm, "    mov     [rsi], dl       \n");
    fprintf(fasm, "    test    rax, rax        \n");
    fprintf(fasm, "    jnz     .print_loop     \n\n");
    fprintf(fasm, "    mov     rdx, buffer + 19 \n");
    fprintf(fasm, "    sub     rdx, rsi        \n");
    fprintf(fasm, "    mov     rax, 1          \n");
    fprintf(fasm, "    mov     rdi, 1          \n");
    fprintf(fasm, "    mov     rsi, rsi        \n");
    fprintf(fasm, "    syscall                 \n");
    fprintf(fasm, "    ret                     \n\n");

    // Fonction getchar
    fprintf(fasm, "getchar:\n");
    fprintf(fasm, "    mov     rax, 0          \n");
    fprintf(fasm, "    mov     rdi, 0          \n");
    fprintf(fasm, "    mov     rsi, char_buffer \n");
    fprintf(fasm, "    mov     rdx, 1          \n");
    fprintf(fasm, "    syscall                 \n");
    fprintf(fasm, "    mov     al, [char_buffer] \n");
    fprintf(fasm, "    ret                     \n\n");

    // Fonction getint
    fprintf(fasm, "getint:\n");
    fprintf(fasm, "    mov     rax, 0          \n");
    fprintf(fasm, "    mov     rdi, 0          \n");
    fprintf(fasm, "    mov     rsi, buffer     \n");
    fprintf(fasm, "    mov     rdx, 16         \n");
    fprintf(fasm, "    syscall                 \n\n");
    fprintf(fasm, "    mov     al, byte [buffer]\n");
    fprintf(fasm, "    cmp     al, '0'\n");
    fprintf(fasm, "    jb      input_error      \n");
    fprintf(fasm, "    cmp     al, '9'\n");
    fprintf(fasm, "    ja      input_error      \n\n");
    fprintf(fasm, "    sub     al, '0'          \n");
    fprintf(fasm, "    movzx   rax, al          \n");
    fprintf(fasm, "    mov     rbx, rax         \n");
    fprintf(fasm, ".next_char:\n");
    fprintf(fasm, "    inc     rsi              \n");
    fprintf(fasm, "    mov     al, byte [rsi]   \n");
    fprintf(fasm, "    cmp     al, 0            \n");
    fprintf(fasm, "    je      .done            \n");
    fprintf(fasm, "    cmp     al, '0'\n");
    fprintf(fasm, "    jb      input_error      \n");
    fprintf(fasm, "    cmp     al, '9'\n");
    fprintf(fasm, "    ja      input_error      \n\n");
    fprintf(fasm, "    sub     al, '0'          \n");
    fprintf(fasm, "    imul    rbx, rbx, 10     \n");
    fprintf(fasm, "    add     rbx, rax         \n");
    fprintf(fasm, "    jmp     .next_char       \n\n");
    fprintf(fasm, ".done:\n");
    fprintf(fasm, "    mov     rax, rbx         \n");
    fprintf(fasm, "    ret                     \n\n");
    fprintf(fasm, "input_error:\n");
    fprintf(fasm, "    mov     rax, 1          \n");
    fprintf(fasm, "    mov     rdi, 1          \n");
    fprintf(fasm, "    mov     rsi, error_message \n");
    fprintf(fasm, "    mov     rdx, error_length \n");
    fprintf(fasm, "    syscall                 \n\n");
    fprintf(fasm, "    mov     rax, 60         \n");
    fprintf(fasm, "    mov     rdi, 5          \n");
    fprintf(fasm, "    syscall                 \n");

    return fasm;
}



/*genererBss : Génère la section .bss du fichier assembleur, définissant des espaces de stockage pour les variables globales basées sur leur type et taille,
et prépare la section .text avec des points d'entrée essentiels*/ 
void genererBss(TableDesSymboles* table, FILE* fichier) {
    if (fichier == NULL) {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }

    fprintf(fichier, "section .bss\n");
    for (int i = 0; i < table->nombreVariablesGlobales; i++) {
        Symbole var = table->variablesGlobales[i];
        if (strcmp(var.type, "int") == 0) {
            if (var.taille > 4) { // Considérer les variables de taille supérieure à 4 octets comme tableaux
                fprintf(fichier, "    %s resd %d  \n", var.nom, var.taille / 4);
            } else {
                fprintf(fichier, "    %s resd 1  \n", var.nom);
            }
        } else if (strcmp(var.type, "char") == 0) {
            if (var.taille > 1) { // Considérer les variables de taille supérieure à 1 octet comme tableaux
                fprintf(fichier, "    %s resb %d  ", var.nom, var.taille);
            } else {
                fprintf(fichier, "    %s resb 1  \n", var.nom);
            }
        } else {
            fprintf(fichier, "    %s resb %d  \n", var.nom, var.taille);
        }

    }
    fprintf(fichier, "section .text\n");
    fprintf(fichier,"    extern putchar\n");
    fprintf(fichier,"    extern getchar\n");
    fprintf(fichier,"    extern getintr\n");
    fprintf(fichier,"    extern putint\n");
    fprintf(fichier, "_start:\n");
}




/*open_asm_file : Ouvre un fichier assembleur nommé "_anonymous.asm" pour l'écriture, et initialise la section .text avec le point d'entrée global _star*/
FILE *open_asm_file() {
    FILE *fasm = fopen("_anonymous.asm", "w");
    if (!fasm) {
        perror("Failed to open file for writing assembly code");
        exit(EXIT_FAILURE);
    }
    // Initialisation de la section .text et du point d'entrée _start
    fprintf(fasm, "global _start\n");
    
    return fasm;
}


 


/*gen_if : Génère du code assembleur pour une instruction conditionnelle "if",
 comprenant des comparaisons et des sauts conditionnels en fonction du résultat de la comparaison.*/
void gen_if(FILE* fasm, Node* comp, Node* elem1, Node* elem2, Node* true_branch, TableDesSymboles* table) {
    switch ((enum yytokentype) elem1->label) {
        case NUM:
            fprintf(fasm, "    mov eax, %d\n", elem1->data.numval);
            break;
        case IDENT:
            fprintf(fasm, "    mov eax, [%s]\n", elem1->data.identifiant);
            break;
        default:
            break;    
    }

    switch ((enum yytokentype) elem2->label) {
        case NUM:
            fprintf(fasm, "    cmp eax, %d\n", elem2->data.numval);
            break;
        case IDENT:
            fprintf(fasm, "    mov ebx, [%s]\n", elem2->data.identifiant);
            fprintf(fasm, "    cmp eax, ebx\n");
            break;
        default:    
            break;    
    }

    char* jump_instr = NULL;

    if (strcmp(comp->data.identifiant, ">") == 0) {
        jump_instr = "jle";
    } else if (strcmp(comp->data.identifiant, "<") == 0) {
        jump_instr = "jge";
    } else if (strcmp(comp->data.identifiant, ">=") == 0) {
        jump_instr = "jl";
    } else if (strcmp(comp->data.identifiant, "<=") == 0) {
        jump_instr = "jg";
    } else if (strcmp(comp->data.identifiant, "==") == 0) {
        jump_instr = "jne";
    } else if (strcmp(comp->data.identifiant, "!=") == 0) {
        jump_instr = "je";
    }

    if (jump_instr != NULL) {
        fprintf(fasm, "    %s .no_op\n", jump_instr);  // Jump to .no_op if comparison is false
    }

    // Generate code for true branch recursively
    Node* instru = true_branch;
    while (instru != NULL) {
        if (instru->label == Instr) {
            Node* varNode = instru->firstChild;
            Node* exprNode = instru->firstChild->nextSibling->nextSibling;
            gen_expr(fasm, varNode, exprNode, table);  // Generate code for the expression
        } else if ((enum yytokentype)instru->label == IF) {
            Node* compNode = instru->firstChild;
            Node* elem1 = compNode->firstChild;
            Node* elem2 = elem1->nextSibling;
            gen_if(fasm, compNode, elem1, elem2, instru->nextSibling, table);  // Recursive call for nested if
        }
        instru = instru->nextSibling;
    }
}






/*gen_expr : Génère du code assembleur pour une expression, supportant des opérations arithmétiques simples 
(addition, soustraction, multiplication, division) et la gestion des variables et constantes.*/
void gen_expr(FILE* fasm, Node* iden, Node* expr, TableDesSymboles* table) {
    if (expr== NULL){
        return;
    } ;
    char* name = iden ? iden->data.identifiant : NULL;

    switch ((enum yytokentype) expr->label) {
        case NUM:  
            if (name != NULL) {
                fprintf(fasm, "    mov dword [%s], %d\n", name, expr->data.numval);
                fprintf(fasm,"    push rax\n");
                break;
                
            }
            else{
                fprintf(fasm, "    mov rax , %d\n", expr->data.numval);
                fprintf(fasm,"    push rax\n");
               

            }
            
            
            break;

        case IDENT:  
            if (strcmp(expr->data.identifiant, "getint") == 0) {
                fprintf(fasm, "    call getint\n");
                if (name != NULL) {
                    fprintf(fasm, "    mov eax, [%s]\n", name);
                }
            } else {
                fprintf(fasm, "    mov rax , qword[%s]\n", expr->data.identifiant);
                fprintf(fasm,"    push rax\n");
                
            }
            break;

        case ADDSUB:  
            gen_expr(fasm, NULL, expr->firstChild, table);  
            gen_expr(fasm, NULL, expr->firstChild->nextSibling, table);  
            switch (expr->data.charval) {  
                case '+':
                    fprintf(fasm, "    pop rcx\n");
                    fprintf(fasm, "    pop rax\n");
                    fprintf(fasm, "    add rax, rcx\n");
                    fprintf(fasm, "    push rax\n");
                    if (name!=NULL && strcmp(name,"+")!=0){
                        fprintf(fasm,"    mov [%s],eax\n",name);
                    }
                    
                    break;
                case '-':
                    fprintf(fasm, "    pop rcx\n");
                    fprintf(fasm, "    pop rax\n");
                    fprintf(fasm, "    sub rax, rcx\n");
                    fprintf(fasm, "    push rax\n");
                    if (name!=NULL && strcmp(name,"-")!=0){
                        fprintf(fasm,"    mov [%s],eax\n",name);
                    }
                    break;
            }
            break;

        case DIVSTAR:  
            gen_expr(fasm, NULL, expr->firstChild, table);  
            gen_expr(fasm, NULL, expr->firstChild->nextSibling, table);  

            switch (expr->data.charval) {  
                case '*':
                    fprintf(fasm, "    pop rcx\n");
                    fprintf(fasm, "    pop rax\n");
                    fprintf(fasm, "    imul rax, rcx\n");
                    fprintf(fasm, "    push rax\n");
                    if (name!=NULL && strcmp(name,"*")!=0){
                        fprintf(fasm,"    mov [%s],eax\n",name);
                    }
        
                    break;
                case '/':
                    fprintf(fasm, "    pop rcx\n");
                    fprintf(fasm, "    pop rax\n");
                    fprintf(fasm, "    idiv rax, rcx\n");
                    fprintf(fasm, "    push rax\n");
                    if (name!=NULL && strcmp(name,"/")!=0){
                        fprintf(fasm,"    mov [%s],eax\n",name);
                    }
                    break;
            }
            break;

        default:
            
            break;
    }
}

            
             
        
  

/*parcourassembleur : Parcourt un arbre abstrait représentant le code source et génère du code assembleur
pour les fonctions et instructions contenues dans cet arbre, y compris la gestion des structures conditionnelles et des retours de fonction.
*/
void parcourassembleur(FILE* fasm, Node* racine, TableDesSymboles* table) {
    if (racine == NULL) return;
    Node* noeudCourant = racine->firstChild;
    while (noeudCourant != NULL) {
        switch (noeudCourant->label) {
            case DeclFoncts: 
            {
                Node* declFonctNode = noeudCourant->firstChild;
                char identifiant[20];
                while (declFonctNode != NULL) {
                    if (declFonctNode->label == DeclFonct) {
                        strcpy(identifiant , declFonctNode->firstChild->firstChild->nextSibling->data.identifiant);
                        fprintf(fasm,"%s :\n",identifiant);
                        Node* instrus = declFonctNode->firstChild->nextSibling->firstChild;
                        Node* instru = instrus->firstChild;
                        while (instru != NULL) {
                            if (instru->label == Instr) {
                                
                                Node* varNode = instru->firstChild;
                                Node* exprNode = instru->firstChild->nextSibling->nextSibling;
                                gen_expr(fasm, varNode,exprNode, table);
                                
                                

                                
                            }
                            if((enum yytokentype)instru->label == IF){
                                Node* compNode = instru->firstChild;
                                Node* elem1 = compNode->firstChild;
                                Node* elem2 = elem1->nextSibling;
                                Node* instru=compNode->nextSibling->firstChild;
                                gen_if(fasm,compNode,elem1,elem2,instru,table);
                            }
                            if((enum yytokentype)instru->label == RETURN){
                                if(instru->firstChild==NULL){
                                    fprintf(fasm,"    xor rax, rax\n");
                                    fprintf(fasm,"    ret\n");
                                    break;
                                }
                                else if((enum yytokentype)instru->firstChild->label==NUM){
                                    
                                    fprintf(fasm,"    mov eax,%d\n",instru->firstChild->data.numval);
                                    fprintf(fasm,"    ret");
                                    break;
                                }
                                else if((enum yytokentype)instru->firstChild->label==IDENT){
                                    
                                    fprintf(fasm,"    mov eax,[%s]\n",instru->firstChild->data.identifiant);
                                    fprintf(fasm,"    ret");
                                    break;
                                    
                                }
                                else if((enum yytokentype)instru->firstChild->label==ADDSUB || (enum yytokentype)instru->firstChild->label==DIVSTAR ){
                                    Node* varNode = instru->firstChild;
                                    Node* exprNode = instru->firstChild->firstChild;
                                    gen_expr(fasm, exprNode,varNode, table);
                                    
                                    
                                    fprintf(fasm, "    pop rax\n");
                                    fprintf(fasm,"    ret");
                                    break;
                                }
                                

                            }

                            instru = instru->nextSibling;
                            fprintf(fasm, "    pop rax\n");
                        }
                        
                  
                    }
                    declFonctNode = declFonctNode->nextSibling;
                }
            } 
            break;
            default:
                break;
        }
        noeudCourant = noeudCourant->nextSibling;
    }
    
}








/*close_asm_file : Ferme le fichier assembleur en ajoutant des instructions pour appeler les fonctions générées, 
gérer la sortie du programme et effectuer l'appel système pour terminer l'exécution.*/
void close_asm_file(FILE *fasm,TableDesSymboles* table) {
    for (int i = 0; i < table->nombreFonctions; i++) {
            Fonction funcloc = table->fonctions[i];
            fprintf(fasm,"    call %s\n",funcloc.nom);     
    }
    fprintf(fasm, "    mov rdi, rax       ; return code 0\n");
    fprintf(fasm, "    mov rax, 60        ; syscall: exit\n");
    fprintf(fasm, "    syscall\n");
    
}






