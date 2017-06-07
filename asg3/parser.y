// Base code provided by Wesley Mackey

%{
#include <cassert>

#include "lyutils.h"
#include "astree.h"

%}

%debug
%defines
%error-verbose
%token-table
%verbose

%token TOK_VOID TOK_CHAR TOK_INT TOK_STRING
%token TOK_IF TOK_ELSE TOK_WHILE TOK_RETURN TOK_STRUCT
%token TOK_NULL TOK_NEW TOK_ARRAY
%token TOK_EQ TOK_NE TOK_LT TOK_LE TOK_GT TOK_GE
%token TOK_IDENT TOK_INTCON TOK_CHARCON TOK_STRINGCON

%token TOK_BLOCK TOK_CALL TOK_IFELSE TOK_INITDECL
%token TOK_POS TOK_NEG TOK_NEWARRAY TOK_TYPEID TOK_FIELD
%token TOK_ORD TOK_CHR TOK_ROOT
%token ROOT IDENT

%token TOK_FUNCTION TOK_PARAMLIST TOK_PROTOTYPE TOK_NEWSTRING
%token TOK_INDEX TOK_DECLID TOK_RETURNVOID TOK_VARDECLAR

%right TOK_IF TOK_ELSE
%right '='
%left  TOK_EQ TOK_NE TOK_LT TOK_LE TOK_GT TOK_GE
%left  '+' '-'
%left  '*' '/' '%'
%right TOK_POS TOK_NEG '!' TOK_NEW
%left  TOK_ARRAY TOK_FIELD TOK_FUNCTION
%left '[' '.'

%start start

%%

start       : program               {yyparse_astree = $1;}
            ;

program     : program structdef     {$$ = $1->adopt($2);}
            | program function      {$$ = $1->adopt($2);}
            | program statement     {$$ = $1->adopt($2);}
            | program error '}'     {$$ = $1;}
            | program error ';'     {$$ = $1;}
            |                       {$$ = new_parseroot();}
            ;

sdefstmt    : '{' fielddeclar ';'   {destroy($3); $$ = $1->adopt($2);}
            | sdefstmt fielddeclar ';'
                                    {destroy($3); $$ = $1->adopt($2);}
            ;

structdef   : TOK_STRUCT TOK_IDENT sdefstmt '}'
                    {destroy($4); $2 = $2->swap_sym($2, TOK_TYPEID);
                    $$ = $1->adopt($2, $3);}
            | TOK_STRUCT TOK_IDENT '{' '}'
                    {destroy($3, $4); $2 = $2->swap_sym($2, TOK_TYPEID);
                    $$ = $1->adopt($2);}
            ;

fielddeclar : basetype TOK_IDENT
                {$2 = $2->swap_sym($2, TOK_FIELD); $$ = $1->adopt($2);}
            | basetype TOK_ARRAY TOK_IDENT
                {$3 = $3->swap_sym($3, TOK_FIELD); 
                        $$ = $2->adopt($1, $3);}
            ;

basetype    : TOK_VOID              {$$ = $1;}
            | TOK_CHAR              {$$ = $1;}
            | TOK_INT               {$$ = $1;}
            | TOK_STRING            {$$ = $1;}
            | TOK_IDENT             {$$ = $1->swap_sym($1, TOK_TYPEID);}
            ;

plist       : '(' identdeclar
                {$1 = $1->swap_sym($1, TOK_PARAMLIST);
                    $$ = $1->adopt($2);}
            | plist ',' identdeclar {destroy($2); $$ = $1->adopt($3);}
            ;

function    : identdeclar plist ')' block
                {
                    destroy($3); 
                    $$ = new astree(TOK_FUNCTION, $1->lloc, "");
                    $$ = $$->adopt($1, $2);
                    $$ = $$->adopt($4);
                }
            | identdeclar plist ')' ';'
                {
                    destroy($3, $4);
                    $$ = new astree(TOK_PROTOTYPE, $1->lloc, "");
                    $$ = $$->adopt($1, $2);
                }
            | identdeclar '(' ')' block
                {
                    destroy($3);
                    $2 = $2->swap_sym($2, TOK_PARAMLIST);
                    $$ = new astree(TOK_FUNCTION, $1->lloc, "");
                    $$ = $$->adopt($1, $2);
                    $$ = $$->adopt($4);
                }
            | identdeclar '(' ')' ';'
                {
                    destroy($3, $4);
                    $2 = $2->swap_sym($2, TOK_PARAMLIST);
                    $$ = new astree(TOK_PROTOTYPE, $1->lloc, "");
                    $$ = $$->adopt($1, $2);
                }
            ;

identdeclar : basetype TOK_IDENT
                { 
                    $2 = $2->swap_sym($2, TOK_DECLID);
                    $$ = $1->adopt($2);
                }
            | basetype TOK_ARRAY TOK_IDENT
                {
                    $3 = $3->swap_sym($3, TOK_DECLID);
                    $$ = $2->adopt($1, $3);
                }
            ;

body        : '{' statement
                {
                    $1 = $1->swap_sym($1, TOK_BLOCK);
                    $$ = $1->adopt($2);
                }
            | body statement
                {
                    $$ = $1->adopt($2); 
                }
            ;

block       :  body '}'
                {
                    destroy($2);
                    $$ = $1->swap_sym($1, TOK_BLOCK);
                }
            | '{' '}'
                {
                    destroy($2);
                    $$ = $1->swap_sym($1, TOK_BLOCK);
                }
            ;

statement   : block                 {$$ = $1;}
            | vardeclar               {$$ = $1;}
            | while                 {$$ = $1;}
            | ifelse                {$$ = $1;}
            | return                {$$ = $1;}
            | exp ';'              {destroy($2); $$ = $1;}
            | ';'                   {$$ = $1;}
            ;

vardeclar   : identdeclar '=' exp ';'
                {
                    destroy($4);
                    $2 = $2->swap_sym($2, TOK_VARDECLAR);
                    $$ = $2->adopt($1, $3);
                }
            ;

while       : TOK_WHILE '(' exp ')' statement
                { 
                    destroy($2, $4);
                    $$ = $1->adopt($3, $5);
                }
            ;

ifelse      : TOK_IF '(' exp ')' statement TOK_ELSE statement
                {
                    destroy($2, $4);
                    $1->swap_sym($1, TOK_IFELSE);
                    $$ = $1->adopt($3, $5);
                    $$ = $$->adopt($7);
                }
            | TOK_IF '(' exp ')' statement %prec TOK_ELSE
                {
                    destroy($2, $4);
                    $$ = $1->adopt($3, $5);
                }
            ;

return      : TOK_RETURN ';'
                {
                    destroy($2);
                    $$ = $1->swap_sym($1, TOK_RETURNVOID);
                }
            | TOK_RETURN exp ';'
                {
                    destroy($3);
                    $$ = $1->adopt($2);
                }
            ;

exp         : binop                     {$$ = $1;}
            | unop                      {$$ = $1;}
            | allocator                 {$$ = $1;}
            | call                      {$$ = $1;}
            | '(' exp ')'               {destroy($1, $3); $$ = $2;}
            | var                       {$$ = $1;}
            | constant                  {$$ = $1;}
            ;

binop       : exp TOK_EQ exp            {$$ = $2->adopt($1, $3);}
            | exp TOK_NE exp            {$$ = $2->adopt($1, $3);}
            | exp TOK_LT exp            {$$ = $2->adopt($1, $3);}
            | exp TOK_LE exp            {$$ = $2->adopt($1, $3);}
            | exp TOK_GT exp            {$$ = $2->adopt($1, $3);}
            | exp TOK_GE exp            {$$ = $2->adopt($1, $3);}
            | exp '+' exp               {$$ = $2->adopt($1, $3);}
            | exp '-' exp               {$$ = $2->adopt($1, $3);}
            | exp '*' exp               {$$ = $2->adopt($1, $3);}
            | exp '/' exp               {$$ = $2->adopt($1, $3);}
            | exp '=' exp               {$$ = $2->adopt($1, $3);}
            ;

unop        : TOK_POS exp           {$$ = $$->adopt($2);}
            | TOK_NEG exp           {$$ = $$->adopt($2);}
            | '!' exp               {$$ = $$->adopt($2);}

            ;

allocator   : TOK_NEW TOK_IDENT '(' ')'
                {
                    destroy($3, $4);
                    $2 = $2->swap_sym($2, TOK_TYPEID);
                    $$ = $1->adopt($2);
                }
            | TOK_NEW TOK_STRING '(' exp ')'
                {
                    destroy($3, $5);
                    $1 = $1->swap_sym($1, TOK_NEWSTRING);
                    $$ = $1->adopt($4);
                }
            | TOK_NEW basetype '[' exp ']'
                {
                    destroy($3, $5);
                    $1 = $1->swap_sym($1, TOK_NEWARRAY);
                    $$ = $1->adopt($2, $4);
                }
            ;

cexp        : TOK_IDENT '(' exp
                {
                    $2 = $2->swap_sym($2, TOK_CALL);
                    $$ = $2->adopt($1, $3);
                }
            | cexp ',' exp
                {
                    destroy($2);
                    $$ = $1->adopt($3);
                }
            ;

call        : cexp ')'
                {
                    destroy($2);
                    $$ = $1;
                }
            | TOK_IDENT '(' ')'
                {
                    destroy($3);
                    $2 = $2->swap_sym($2, TOK_CALL);
                    $$ = $2->adopt($1);
                }
            ;

var         : TOK_IDENT         {$$ = $1;}
            | exp '[' exp ']'  
                { 
                    destroy($4);
                    $2 = $2->swap_sym($2, TOK_INDEX);
                    $$ = $2->adopt($1, $3);
                }
            | exp '.' TOK_IDENT 
                {
                    $3 = $3->swap_sym($3, TOK_FIELD);
                    $$ = $2->adopt($1, $3);
                }
            ;

constant    : TOK_INTCON            {$$ = $1;}
            | TOK_CHARCON           {$$ = $1;}
            | TOK_STRINGCON         {$$ = $1;}
            | TOK_NULL              {$$ = $1;}
            ;

%%

const char *parser::get_tname(int symbol) {
   return yytname [YYTRANSLATE(symbol)];
}

const char *get_yytname(int symbol) {
   return yytname [YYTRANSLATE(symbol)];
}


bool is_defined_token(int symbol) {
   return YYTRANSLATE(symbol) > YYUNDEFTOK;
}
