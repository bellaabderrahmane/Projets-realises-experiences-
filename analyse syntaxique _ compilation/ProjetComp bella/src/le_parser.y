%{
#include "tree.h"
#include "symbole.h"
#include <unistd.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/* Inclusion des en-têtes standard et définition des variables externes */
extern int lineno;
extern int yylex();

/* Fonction yyerror pour la gestion des erreurs  */
void yyerror(const char *s) {
    fprintf(stderr, "Erreur à la ligne %d: %s\n", lineno, s);
}

/* Racine de l'arbre abstrait */
Node *root = NULL;


/* Fonction pour afficher l'aide de la ligne de commande */
void displayHelp() {
    printf("Usage: ./tpcas [OPTIONS] < filename.tpc\n");
    printf("Options:\n");
    printf("  -t\tAfficher l'arbre abstrait\n");
    printf(" -s\tAfficher la table des symboles\n");
    printf("  -h\tAfficher cette aide\n");
    printf("Lit l'entrée standard.\n");
}




%}

/* Définition de l'union pour les valeurs de retour de Bison */
%union {
    Node *node;
    int numval;
    char charval;
    char identifiant[20];
    char type[20];
    char comparaison[5];    
}




%token <comparaison> ORDER EQ
%token <numval> NUM
%token <type> TYPE
%token <charval> ADDSUB DIVSTAR CHARACTER  EXCLAM EQUALS
%token OR AND IF ELSE VOID WHILE RETURN 
%token '[' ']'
%token <identifiant> IDENT
%type<node> Prog DeclVars Declarateurs Declarateur DeclFoncts DeclFonct EnTeteFonct Parametres ListTypVar  Corps SuiteInstr Instr Exp TB FB M E T F LValue Arguments ListExp 


/* Configuration des options et des priorités d'analyse */

%define parse.error verbose
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%left IDENT
%left '['
%left ']'

%%


Prog:  DeclVars DeclFoncts{
    // Création du nœud 'Prog' et ajout des enfants 'DeclVars' et 'DeclFoncts'
    $$ = makeNode(Prog);
    root=$$; // Enregistrement de la racine de l'arbre
    addChild($$,$1);
    addChild($$,$2);
}
    ;




DeclVars:
       DeclVars TYPE Declarateurs ';'{
        // Ajout d'une nouvelle déclaration à la liste existante
           if ($1 != NULL) {
               Node *type = makeNode(TYPE);
               addChild($1, type);
               setNodeType(type, $<type>2); 
               addChild($1, $3); 
               $$ = $1; 
           } 
           else {
            // Création d'une nouvelle liste de déclarations
               $$ = makeNode(DeclVars);
               Node *type = makeNode(TYPE);
               addChild($$, type); 
               addChild($$, $3); 
               setNodeType(type, $<type>2);
               
           }
       }

    |  {
           $$ = NULL; 
}
    ;





Declarateurs:
    Declarateurs ',' Declarateur {
        // Extension de la liste existante de déclarateurs
        if ($1 != NULL) {
            $$ = $1; // Utilise le nœud Declarateurs existant
            addChild($1, $3); // Ajoute Declarateur au nœud existant
        } else {
            // Si Declarateurs n'existe pas encore, créer un nouveau nœud
            $$ = makeNode(Declarateurs);
            addChild($$, $3); // Ajoute le premier Declarateur
        }
    }
    | Declarateur {
        $$ = makeNode(Declarateurs);
        addChild($$, $1); // Ajoute le premier Declarateur
    }
    ;




Declarateur:
       IDENT {
           // Création d'un nœud pour un identifiant simple
           Node *ident= makeNode(IDENT);
           setNodeIdentifier(ident,$<identifiant>1);
           $$ = ident;
       }
    |  IDENT '[' NUM ']' {
           // Création d'un nœud pour un tableau avec une taille spécifiée
           $$ = makeNode(Array); 
           Node *ident= makeNode(IDENT);
           setNodeIdentifier(ident,$<identifiant>1);
           addChild($$, ident);
           Node *num=makeNode(NUM);
           setNumVal(num,$<numval>3);
           addChild($$, num);
       }
    ;




DeclFoncts:
       DeclFoncts DeclFonct {
            // Ajout d'une nouvelle fonction à la liste existante
           addChild($1, $2); 
           $$ = $1;         
       }
    |  DeclFonct {
            // Création d'une liste de déclarations de fonctions avec un seul élément
           $$ = makeNode(DeclFoncts); 
           addChild($$, $1);          
       }
    ;



DeclFonct:
       EnTeteFonct Corps {
        // Création d'un nœud pour une fonction avec un en-tête et un corps
           $$ = makeNode(DeclFonct);
           addChild($$, $1); 
           addChild($$, $2); 
       }
    ;




EnTeteFonct:
       TYPE IDENT '(' Parametres ')' {
            // Création d'un nœud pour l'en-tête de fonction avec un type spécifié
           $$ = makeNode(EnTeteFonct);
           Node *type= makeNode(TYPE);
           addChild($$, type); 
           setNodeType(type, $<type>1);
           Node *ident= makeNode(IDENT);
           setNodeIdentifier(ident,$<identifiant>2);
           addChild($$, ident); 
           addChild($$, $4); 
       }
    |  VOID IDENT '(' Parametres ')' {
           // Création d'un nœud pour l'en-tête de fonction de type void
           $$ = makeNode(EnTeteFonct);
           addChild($$, makeNode(VOID)); 
           Node *ident= makeNode(IDENT);
           setNodeIdentifier(ident,$<identifiant>2);
           addChild($$, ident); 
           addChild($$, $4); 
       }
    ;



Parametres:
    VOID {
        // Création d'un nœud pour indiquer l'absence de paramètres (void)
        $$ = makeNode(VOID); 
    }
    | ListTypVar {
        $$ = $1; // Utilise directement le nœud ListTypVar existant
    }
    ;




ListTypVar:
    ListTypVar ',' TYPE IDENT {
        Node *typeNode = makeNode(TYPE);
        setNodeType(typeNode, $<type>3);
        addChild($1, typeNode);

        Node *identNode = makeNode(IDENT);
        setNodeIdentifier(identNode, $<identifiant>4);
        addChild($1, identNode);

        $$ = $1;
    }
    | ListTypVar ',' TYPE IDENT '[' ']' {
        Node *typeNode = makeNode(TYPE);
        setNodeType(typeNode, $<type>3);
        addChild($1, typeNode);

        Node *identNode = makeNode(IDENT);
        setNodeIdentifier(identNode, $<identifiant>4);
        addChild($1, identNode);

        addChild($1, makeNode(Array));

        $$ = $1;
    }
    | TYPE IDENT {
        Node *typeNode = makeNode(TYPE);
        setNodeType(typeNode, $<type>1);
        
        Node *identNode = makeNode(IDENT);
        setNodeIdentifier(identNode, $<identifiant>2);

        $$ = makeNode(ListTypVar);
        addChild($$, typeNode);
        addChild($$, identNode);
    }
    | TYPE IDENT '[' ']' {
        Node *typeNode = makeNode(TYPE);
        setNodeType(typeNode, $<type>1);
        
        Node *identNode = makeNode(IDENT);
        setNodeIdentifier(identNode, $<identifiant>2);

        $$ = makeNode(ListTypVar);
        addChild($$, typeNode);
        addChild($$, identNode);
        addChild($$, makeNode(Array));
    }
    ;




Corps: '{' DeclVars SuiteInstr '}' {
    $$ = makeNode(Corps);
    if ($2 != NULL) {
        addChild($$, $2); 
    }
    if ($3 != NULL) {
        addChild($$, $3); 
    }
};





SuiteInstr:
       SuiteInstr Instr {
           if ($1 != NULL) {
               $$ = $1;
               addChild($1, $2); 
           } else {
               $$ = makeNode(SuiteInstr);
               addChild($$, $2);
           }
       }
       |  {
           $$ = NULL; 
       }
    ;



Instr:
       LValue EQUALS Exp ';' {
           $$ = makeNode(Instr);
           addChild($$, $1); 
           Node *charac= makeNode(EQUALS);
           setCharVal(charac,$<charval>2);
           addChild($$, charac); 
           addChild($$, $3); 
       }
    |  IF '(' Exp ')' Instr %prec LOWER_THAN_ELSE{
           $$ = makeNode(IF);
           addChild($$, $3); 
           addChild($$, $5); 
       }
    |  IF '(' Exp ')' Instr ELSE Instr {
           $$ = makeNode(Instr);
           addChild($$, makeNode(IF)); 
           addChild($$, $3); 
           addChild($$, $5); 
           addChild($$, makeNode(ELSE)); 
           addChild($$, $7); 
       }
    |  WHILE '(' Exp ')' Instr {
           $$ = makeNode(Instr);
           addChild($$, makeNode(WHILE)); 
           addChild($$, $3); 
           addChild($$, $5); 
       }
    |  IDENT '(' Arguments  ')' ';' {
           $$ = makeNode(Instr);
           Node *ident= makeNode(IDENT);
           setNodeIdentifier(ident,$<identifiant>2);
           addChild($$, ident); 
           addChild($$, $3); 
       }
    |  RETURN Exp ';' {
           $$ = makeNode(RETURN);
           addChild($$, $2); 
       }
    |  RETURN ';' {
           $$ = makeNode(RETURN);
       }
    |  '{' SuiteInstr '}' {
    $$ = $2; 
       }

    |  ';' {
       }
    ;






Exp :  Exp OR TB {
    $$ = makeNode(AND);
    addChild($$, $1); 
    addChild($$, $3); 
}
|  TB {
    $$ = $1; 
};

TB  :  TB AND FB {
    $$ = makeNode(AND);
    addChild($$, $1); 
    addChild($$, $3); 
}
|  FB {
    $$ = $1; 
};

FB  :  FB EQ M {
    Node* compar= makeNode(EQ);
    setNodeCompar(compar,$<comparaison>2);
    $$=compar;
    addChild($$, $1); 
    addChild($$, $3); 
}
|  M {
    $$ = $1; 
};

M   :  M ORDER E {
    Node* compar= makeNode(ORDER);
    setNodeCompar(compar,$<comparaison>2);
    $$=compar;
    addChild($$, $1); 
    addChild($$, $3); 
}
|  E {
    $$ = $1; 
};

E   :  E ADDSUB T {
    Node *charac= makeNode(ADDSUB);
    setCharVal(charac,$<charval>2);
    $$ = charac;
    addChild($$, $1); 
    addChild($$, $3); 
}
|  T {
    $$ = $1; 
};    
    
T   :  T DIVSTAR F {
    Node *charac= makeNode(DIVSTAR);
    setCharVal(charac,$<charval>2);
    $$ = charac;
    addChild($$, $1); 
    addChild($$, $3); 
}
|  F {
    $$ = $1; 
};
F   :  ADDSUB F
        { 
            Node *charac= makeNode(ADDSUB);
            setCharVal(charac,$<charval>1);
            $$ = charac;
            addChild($$, $2);               
        }
    |  EXCLAM F
        {   
            Node *charac= makeNode(EXCLAM);
            setCharVal(charac,$<charval>1);
            $$ = charac;   
            addChild($$, $2);               
        }
    |  '(' Exp ')'
        {
            $$ = $2;             
        }
    |  NUM
        {   
           Node *num=makeNode(NUM);
           setNumVal(num,$<numval>1);
           $$ = num;             
        }
    |  CHARACTER
        {
            Node *charac= makeNode(CHARACTER);
            setCharVal(charac,$<charval>1);
            $$ = charac;       
        }
    |  LValue
        {
            $$ = $1;                        
        }
    |  IDENT '(' Arguments  ')'
        {
            Node *ident= makeNode(IDENT);
            setNodeIdentifier(ident,$<identifiant>2);
            $$ = ident;
            addChild($$, $3);               
        }
    |  IDENT '[' Exp ']'
        {
            Node *ident= makeNode(IDENT);
            setNodeIdentifier(ident,$<identifiant>2);
            $$ = ident;
            addChild($$, $3);               
        }
    ;




LValue:
       IDENT {

        Node *ident= makeNode(IDENT);
        setNodeIdentifier(ident,$<identifiant>1);
        $$ = ident;
        
       }
       | LValue '[' Exp ']'{ Node *ident= makeNode(IDENT);setNodeIdentifier(ident,$<identifiant>1);$$ = ident; addChild($$, $1); addChild($$, $3); }
    ;




Arguments:
       ListExp { $$ = $1; } 
    |  { $$ = NULL; }       
    ;




ListExp:
       ListExp ',' Exp {
           $$ = makeNode(ListExp);
           addChild($$, $1); 
           addChild($$, $3); 
       }
    |  Exp {
           $$ = makeNode(ListExp);
           addChild($$, $1); 
       }
    ;



%%




int main(int argc, char *argv[]) {
    int opt;
    int tree_flag = 0;
    int symbole_flag = 0;
    TableDesSymboles* table = creerTableDesSymboles(); 


    
    while ((opt = getopt(argc, argv, "ths")) != -1) {
        switch (opt) {
            case 't':  
                tree_flag = 1;
                break;
            case 'h':  
                displayHelp();
                return EXIT_SUCCESS;
            case 's':
                symbole_flag =1;
                break; 
            default:  
                displayHelp();
                return EXIT_FAILURE;
        }
    }
    int parseResult = yyparse();
    


    

    

    
    if (parseResult == 0) {
        printf("Analyse Syntaxique réussie.\n");
        if (tree_flag) {
            printTree(root);
        }
        if(symbole_flag){
            parcourirArbre(root,table);
            afficheTableDesSymboles(table);
        }
        TABLE_VN* globa_t = NULL ;
        TABLE_FN* fonca_t = NULL;

        Tree_run(root->firstChild,&globa_t ,&fonca_t ) ;
        checkmain(fonca_t) ;                                    
        checkDuplicateNames(globa_t);
        checkD(fonca_t);
        checkNameConflict(fonca_t, globa_t) ;
        FunctionNames(fonca_t);                                 
        open_functions_file();
        FILE* fasm;
        fasm = open_asm_file();
        genererBss(table, fasm);
        close_asm_file(fasm,table);
        parcourassembleur(fasm,root, table);
        return 0 ;
    } else {
        printf("Erreur lors de l'analyse syntaxique.\n");
        return 1 ;
    }
}

