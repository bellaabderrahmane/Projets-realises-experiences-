%{
#include <stdio.h>
#include "tree.h"
#include "le_parser.h" 
int lineno = 0 ;
%} 
%option nounput       
%option noinput
%%  
[ \t]                            {}
[\n]                             {lineno++;}
"/*"([^*]|"\n"|\*+[^*/])*"*/"    {} 
"//".*                           {}
"int"                            { strcpy( yylval.ident, yytext);    return TYPE;      }
"char"                           { strcpy( yylval.ident, yytext);    return TYPE;      }
"["                              {                                   return '[';       }
[0-9]+                           { yylval.number=atoi(yytext);       return NUM;       }
"]"                              {                                   return ']';       }
";"                              {                                   return ';';       }
"void"                           {                                   return VOID;      }
"("                              {                                   return '(';       }
")"                              {                                   return ')';       }
"{"                              {                                   return '{';       }
"="                              {                                   return '=';       }
"!"                              {                                   return '!';       }
","                              {                                   return ',';       }
"&&"                             { strcpy( yylval.comp, yytext) ;    return AND;       }
"||"                             { strcpy( yylval.comp, yytext) ;    return OR;        }
"=="                             { strcpy( yylval.comp, yytext) ;    return EQ;        }
"!="                             { strcpy( yylval.comp, yytext) ;    return EQ;        }
"<"                              { strcpy( yylval.comp, yytext) ;    return ORDER;     }
">"                              { strcpy( yylval.comp, yytext) ;    return ORDER;     }
">="                             { strcpy( yylval.comp, yytext) ;    return ORDER;     }
"<="                             { strcpy( yylval.comp, yytext) ;    return ORDER;     } 
"+"                              { yylval.byte=yytext[0];            return ADDSUB;    }
"-"                              { yylval.byte=yytext[0];            return ADDSUB;    }
[\*\%\/]                         { yylval.byte=yytext[0];            return DIVSTAR;   }
"if"                             {                                   return IF;        }
"else"                           {                                   return ELSE;      }
"while"                          {                                   return WHILE;     }
"return"                         {                                   return RETURN;    } 
"}"                              {                                   return '}';       }
'\\''|'\\n'|'\\t'                { yylval.byte=yytext[3];            return CHARACTER; }
'.'                              { yylval.byte=yytext[1];            return CHARACTER; }
[a-zA-Z_][a-zA-Z0-9_]*           { strcpy( yylval.ident, yytext);    return IDENT;     }
.                                {                                   return yytext[0] ;}               
%%
int yywrap() { return 1; }  