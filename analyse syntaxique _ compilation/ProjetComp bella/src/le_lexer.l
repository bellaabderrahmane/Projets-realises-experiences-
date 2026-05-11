%{
#include "tree.h"
#include <stdio.h>
#include "le_parser.h"
int lineno=0;
void yyerror(const char *s);

%}

%x STRING
%option nounput
%option noinput
%option noyywrap

%%


\/\* BEGIN STRING ;
<STRING>\n {}
<STRING>(.|\t|\*|\r|\/) ;
<STRING>\*\/ BEGIN INITIAL ;
"//".* 


[0-9]+                       { yylval.numval = atoi(yytext); return NUM; }



"=="                           { strcpy(yylval.comparaison, "=="); return EQ; }
"!="                           { strcpy(yylval.comparaison, "!="); return EQ; }


"<"                            { strcpy(yylval.comparaison, "<"); return ORDER; }
"<="                           { strcpy(yylval.comparaison, "<="); return ORDER; }
">"                            { strcpy(yylval.comparaison, ">"); return ORDER; }
">="                           { strcpy(yylval.comparaison, ">="); return ORDER; }


"+"                            { yylval.charval = '+'; return ADDSUB; }
"-"                            { yylval.charval = '-'; return ADDSUB; }


"*"                            { yylval.charval = '*'; return DIVSTAR; }
"/"                            { yylval.charval = '/'; return DIVSTAR; }
"%"                            { yylval.charval = '%'; return DIVSTAR; }




"="                            { yylval.charval = '='; return EQUALS;}
"!"                            { yylval.charval = '!'; return EXCLAM;}



"&&"                           { return AND; }
"||"                           { return OR; }


"int"   {strcpy(yylval.type, "int");return TYPE;}
"char"  {strcpy(yylval.type, "char");return TYPE; }


"if"                           { return IF; }
"else"                         { return ELSE; }
"return"                       { return RETURN; }
"while"                        { return WHILE;}
"void"                         { return VOID;}


[a-zA-Z_]([a-zA-Z_]|[0-9])*    {strcpy(yylval.identifiant, yytext);return IDENT;}

'\\[nrt]'|'[^\\]' { yylval.charval = yytext[0]; return CHARACTER; }
'.'    { yylval.charval = yytext[0]; return CHARACTER; }

";"                            { return ';'; }
","                            { return ','; }
"("                            { return '('; }
")"                            { return ')'; }
"{"                            { return '{'; }
"}"                            { return '}'; }
"["                            { return '['; }
"]"                            { return ']'; }


\n               { lineno++; }


.                             {}
