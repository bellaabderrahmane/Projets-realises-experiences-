%{
#include <stdio.h>
#include "tree.h"
#include <string.h>

    
extern int yylex();
extern int lineno ;
void yyerror(const char *s);
Node *Tree ;
void readFile(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }
    printf("\ndescription de tpcas : \n");
    char buffer[1024]; 
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        printf("%s", buffer);
    }
    printf("\n");
    fclose(file);
}
%}
 
   
 
%token <number> NUM  
%token <ident> TYPE IDENT
%token VOID
%token IF ELSE RETURN WHILE
%token <byte> ADDSUB DIVSTAR CHARACTER
%token <comp> EQ ORDER AND OR

%union {
Node *node;
char ident[50];
int number;
char byte;
char comp[3];
}

%nonassoc Priority
%nonassoc ELSE

    
%type  <node>  Prog  DeclVars Declarateurs DeclFoncts DeclFonct Var EnTeteFonct Parametres ListTypVar Corps  SuiteInstr Instr  Exp TB FB M T E F LValue Arguments ListExp


%%

Prog:       
            DeclVars DeclFoncts                 { Tree = makeNode(Prog); 
                                                addChild(Tree ,$1);
                                                addChild(Tree ,$2); 
                                                 }
            ;
DeclVars:
            DeclVars TYPE Declarateurs ';'      {
                                                Node *typeNode = makeNode(Type);
                                                typeNode->valueType = IDENTIFIER;
                                                strcpy(typeNode->value.ident, $2);
                                                addChild(typeNode, $3); 
                                                    if ($1 == NULL) { 
                                                        $$ = typeNode; 
                                                    } else { 
                                                        addSibling($1, typeNode); 
                                                        $$ = $1; 
                                                    }
                                                }

            |                                   { $$ = NULL; }
            ;
Declarateurs:
            Declarateurs ',' Var                {  
                                                    if ($1 == NULL) {
                                                        $$ = $3; 
                                                    } else {
                                                        addSibling($1, $3); 
                                                        $$ = $1; 
                                                    }
                                                }
            | IDENT                             { $$ = makeNode(Ident);
                                                  $$->valueType = IDENTIFIER;
                                                  strcpy($$->value.ident, $1);}
            | IDENT '[' NUM ']'                 { $$ = makeNode(Ident); 
                                                  $$->valueType = IDENTIFIER;
                                                  strcpy($$->value.ident, $1);
                                                  Node *num = makeNode(Num);
                                                  num->valueType = NUMBER;
                                                  num->value.number = $3;
                                                  addChild($$, num); }
            ;
Var : 
            IDENT                               { $$ = makeNode(Ident); 
                                                  $$->valueType = IDENTIFIER;
                                                  strcpy($$->value.ident, $1);}
            |IDENT '[' NUM ']'                  { $$ = makeNode(Ident); 
                                                  $$->valueType = IDENTIFIER;
                                                  strcpy($$->value.ident, $1);
                                                  Node *num = makeNode(Num);
                                                  num->valueType = NUMBER;
                                                  num->value.number = $3;
                                                  addChild($$, num); }
            ;
DeclFoncts: 
            DeclFoncts DeclFonct                {
                                                    if ($1 == NULL) {
                                                        $$ = $2;
                                                        
                                                    } else {
                                                        $$ = $1;
                                                        addSibling($1, $2);
                                                    }
                                                }
            | DeclFonct                         { $$ = $1; }
            ;
DeclFonct:  
            EnTeteFonct Corps                   {
                                                    $$ = makeNode(DeclFonct);
                                                    addChild($$, $1); 
                                                    addChild($$, $2);   
                                                }
            ;
EnTeteFonct:
            TYPE IDENT '(' Parametres ')'       {
                                                    $$ = makeNode(Type);
                                                    $$->valueType = IDENTIFIER;
                                                    strcpy($$->value.ident, $1);
                                                    
                                                    Node *name = makeNode(Ident) ;
                                                    name->valueType = IDENTIFIER;
                                                    strcpy(name->value.ident, $2);
                                                    addSibling($$,name);


                                                    addSibling($$,$4);
                                                }
            | VOID IDENT '(' Parametres ')'     {
                                                    $$ = makeNode(Void);
                                                    Node *name = makeNode(Ident) ;
                                                    name->valueType = IDENTIFIER;
                                                    strcpy(name->value.ident, $2);
                                                    addSibling($$,name);
                                                    addSibling($$,$4);
                                                }
            ;
Parametres: 
            VOID                                {
                                                    $$ = makeNode(Parametres);
                                                    addChild($$, makeNode(Void));
                                                }
            | ListTypVar                        {           
                                                    $$ = makeNode(Parametres); 
                                                    addChild($$, $1);
                                                }
            ;




ListTypVar: 
            ListTypVar ',' TYPE IDENT           { 
                                                    if ($1 == NULL) {
                                                        $$ = makeNode(Type);               
                                                        $$->valueType = IDENTIFIER;
                                                        strcpy($$->value.ident, $3);

                                                        Node *name = makeNode(Ident) ;
                                                        name->valueType = IDENTIFIER;
                                                        strcpy(name->value.ident, $4);

                                                        addChild($$, name);         
                                                    } else {
                                                        $$ = $1;
                                                        Node *type = makeNode(Type);
                                                        type->valueType = IDENTIFIER;
                                                        strcpy(type->value.ident, $3);
                                                        Node *name = makeNode(Ident);
                                                        name->valueType = IDENTIFIER;
                                                        strcpy(name->value.ident, $4);
                                                        addChild(type, name); 
                                                        addSibling($$, type); 
                                                    }
                                                }
            | ListTypVar ',' TYPE IDENT '[' ']' { 
                                                    if ($1 == NULL) {
                                                        $$ = makeNode(Type);               
                                                        $$->valueType = IDENTIFIER;
                                                        strcpy($$->value.ident, $3);
                                                        Node *name = makeNode(Ident) ;
                                                        name->valueType = IDENTIFIER;
                                                        strcpy(name->value.ident, $4);
                                                        Node *num = makeNode(Num);
                                                        num->valueType = NUMBER;
                                                        num->value.number = 0;
                                                        addChild(name, num); 
                                                        addChild($$, name); 
                                                    } else {
                                                        $$ = $1;
                                                        Node *type = makeNode(Type);
                                                        type->valueType = IDENTIFIER;
                                                        strcpy(type->value.ident, $3);
                                                        Node *name = makeNode(Ident);
                                                        name->valueType = IDENTIFIER;
                                                        strcpy(name->value.ident, $4);
                                                        Node *num = makeNode(Num);
                                                        num->valueType = NUMBER;
                                                        num->value.number = 0;
                                                        addChild(name, num);
                                                        addChild(type, name); 
                                                        addSibling($$, type); 
                                                    }
                                                }
            | TYPE IDENT                        { 
                                                    $$ = makeNode(Type); 
                                                    $$->valueType = IDENTIFIER;
                                                    strcpy($$->value.ident, $1);
                                                    Node *name = makeNode(Ident);
                                                    name->valueType = IDENTIFIER;
                                                    strcpy(name->value.ident, $2);   //fix redanduncy and no speacial for tabs
                                                    addChild($$, name); 
                                                }
            | TYPE IDENT '[' ']'                { 
                                                    $$ = makeNode(Type); 
                                                    $$->valueType = IDENTIFIER;
                                                    strcpy($$->value.ident, $1);
                                                    Node *name = makeNode(Ident);
                                                    name->valueType = IDENTIFIER;
                                                    strcpy(name->value.ident, $2); 
                                                    Node *num = makeNode(Num);
                                                    num->valueType = NUMBER;
                                                    num->value.number = 0;
                                                    addChild(name, num); 
                                                    addChild($$, name);  
                                                }
            ; 
Corps:     
            '{' DeclVars SuiteInstr '}'         { $$ = makeNode(Corps); addChild($$, $2);addChild($$, $3); }
            ;          
SuiteInstr:
            SuiteInstr Instr                    { 
                                                    if ($1 == NULL) { 
                                                        $$ = $2; 
                                                    } else { 
                                                        addSibling($$, $2); 
                                                    }
                                                     
                                                }
            |                                   { $$ = NULL ; }
            ;
Instr:                
        LValue '=' Exp ';'                          { $$ = makeNode(Affectation); $$->valueType = BYTE;$$->value.byte = '=';addChild($$, $1); addChild($$, $3); }
            |  IF '(' Exp ')' Instr   %prec Priority { $$ = makeNode(If); addChild($$, $3); addChild($$, $5) ; }
            |  IF '(' Exp ')' Instr ELSE Instr      {   $$ = makeNode(If); 
                                                        addChild($$, $3); 
                                                        Node *instr = makeNode(Instr);
                                                        addChild(instr, $5); 
                                                        addChild($$, instr);
                                                        Node *instr1 = makeNode(Instr);
                                                        addChild(instr1, $7); 
                                                        addChild($$, instr1);}
            |  WHILE '(' Exp ')' Instr              { $$ = makeNode(While); addChild($$, $3); addChild($$, $5); }
            |  IDENT '(' Arguments ')' ';'          { $$ = makeNode(Ident);$$->valueType = IDENTIFIER;strcpy($$->value.ident, $1);addChild($$, $3);  }
            |  RETURN Exp ';'                       { $$ = makeNode(Return) ; addChild($$,$2) ; }
            |  RETURN ';'                           { $$ = makeNode(Return);}
            |  '{' SuiteInstr '}'                   { $$ = $2; }
            |  ';'                                  {}          
            ;

 


Exp :       
            Exp OR TB                           { $$ = makeNode(Or);$$->valueType = COMP;
                                                strcpy($$->value.comp, $2); 
                                                addChild($$, $1); 
                                                addChild($$, $3); }
            |  TB                               { $$ = $1;}
            ;
TB  :       
            TB AND FB                           { $$ = makeNode(And);
                                                $$->valueType = COMP;
                                                strcpy($$->value.comp, $2);
                                                addChild($$, $1);
                                                addChild($$, $3); }
            |  FB                               { $$ = $1; }
            ;
FB  :       
            FB EQ M                             { $$ = makeNode(Eq);
                                                $$->valueType = COMP;
                                                strcpy($$->value.comp, $2);
                                                addChild($$, $1);
                                                addChild($$, $3); }
            |  M                                { $$ = $1; }
            ;
M   :       
            M ORDER E                           { $$ = makeNode(Order);
                                                $$->valueType = COMP;
                                                strcpy($$->value.comp, $2);
                                                addChild($$, $1);
                                                addChild($$, $3); }
            |  E                                {  $$ = $1; }
            ;
E   :       
            E ADDSUB T                          { $$ = makeNode(Addsub) ;
                                                $$->valueType = BYTE;
                                                $$->value.byte = $2; 
                                                addChild($$, $1);  
                                                addChild($$, $3); }
            | T                                 { $$ = $1; }
            ;
T   :       
            T DIVSTAR F                         { $$ = makeNode(Divstar);
                                                $$->valueType = BYTE;
                                                $$->value.byte = $2;
                                                addChild($$, $1); 
                                                addChild($$, $3); }
            | F                                 {  $$ = $1; }
            ;
F   :       
            ADDSUB F                           { $$ = makeNode(Addsub);$$->valueType = BYTE;$$->value.byte = $1;  addChild( $$, $2);}
            | '!' F                            { $$ = makeNode(Non);$$->valueType = BYTE;$$->value.byte = '!'; addChild($$, $2);}
            | '(' Exp ')'                      { $$ = $2;}
            | NUM                              { $$ = makeNode(Num);$$->valueType = NUMBER; $$->value.number = $1;}
            | CHARACTER                        { $$ = makeNode(Character);$$->valueType = BYTE;$$->value.byte = $1 ;}
            | LValue                           { $$ = $1; }
            | IDENT '(' Arguments ')'          { $$ = makeNode(Ident);$$->valueType = IDENTIFIER;strcpy($$->value.ident, $1); addChild($$, $3);}
            ;   
LValue:     
            IDENT                              { $$ = makeNode(Ident);  $$->valueType = IDENTIFIER;strcpy($$->value.ident, $1);}
            |  IDENT '[' Exp ']'               { $$ = makeNode(Ident); $$->valueType = IDENTIFIER;strcpy($$->value.ident, $1); addChild($$, $3); }
            ;
Arguments:
            ListExp                            { $$ = makeNode(Arguments);addChild($$, $1);   }
            |                                  { $$ = NULL;                                   }
            ;
ListExp:
            ListExp ',' Exp                    {
                                                    if ($1 == NULL) {
                                                        $$ = $3;
                                                    } else {
                                                        $$ = $1; 
                                                    }
                                                    addSibling($$, $3);
                                                }
            | Exp                               { $$ = $1; }

%%



void yyerror(const char *s) {
    fprintf(stderr, "Erreur : %s dans la ligne : %d\n", s,lineno);
}

int main(int args , char *argv[]) {
    int Parse_result = yyparse();  
    if (Parse_result == 0) {
        if (args > 1) {
            for (int i = 1 ; i < args ; i++){
                if ( strcmp(argv[1], "-t") == 0){
                    printTree(Tree,1);
                }
                if ( strcmp(argv[1], "-h") == 0){
                    readFile("src/description.txt");
                    return 0;
                }
                if ( strcmp(argv[1], "-tl") == 0){
                    printTree(Tree,0);
                }
            }
        }
        printf("L'expression est engendrée par la grammaire !\n");
        return 0;
    } else if (Parse_result == 1) {
        printf("N'est pas engendrée par la grammaire.\n");
        return 1;
    }else {
        return 2 ;
    }

}