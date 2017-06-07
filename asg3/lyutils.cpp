// $Id: lyutils.cpp,v 1.3 2016-10-06 16:42:35-07 - - $

#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "auxlib.h"
#include "lyutils.h"

extern FILE * outTokFile;

bool lexer::interactive = true;
location lexer::lloc = {0, 1, 0};
size_t lexer::last_yyleng = 0;
vector<string> lexer::filenames;

astree* parser::root = nullptr;
astree* yyparse_astree = nullptr;

astree* new_parseroot(void) {
   yyparse_astree = new astree (TOK_ROOT, {0, 0, 0}, "ROOT");
   return yyparse_astree;
}

const string* lexer::filename (int filenr) {
   return &lexer::filenames.at(filenr);
}

void lexer::newfilename (const string& filename) {
   lexer::lloc.filenr = lexer::filenames.size();
   lexer::filenames.push_back (filename);
}

void lexer::advance() {
   if (not interactive) {
      if (lexer::lloc.offset == 0) {
         printf (";%2zd.%3zd: ",
                 lexer::lloc.filenr, lexer::lloc.linenr);
      }
      printf ("%s", yytext);
   }
   lexer::lloc.offset += last_yyleng;
   last_yyleng = yyleng;
}

void lexer::newline() {
   ++lexer::lloc.linenr;
   lexer::lloc.offset = 0;
}

void lexer::badchar (unsigned char bad) {
   char buffer[16];
   snprintf (buffer, sizeof buffer,
             isgraph (bad) ? "%c" : "\\%03o", bad);
   errllocprintf (lexer::lloc, "invalid source character (%s)\n",
                  buffer);
   exec::exit_status = EXIT_FAILURE;
}

void lexer::badtoken (char* lexeme) {
   errllocprintf (lexer::lloc, "invalid token (%s)\n", lexeme);
}

int yylval_token (int symbol) {
   yylval = new astree (symbol, lexer::lloc, yytext);
   fprintf(outTokFile, "%4ld %4.3f %4d %-16s (%s)\n",
           lexer::lloc.filenr, lexer::lloc.linenr + 
           lexer::lloc.offset/1000.0, symbol, 
           parser::get_tname(symbol), yytext);
   return symbol;
}

void lexer::include() {
   size_t linenr;
   static char filename[0x1000];
   assert (sizeof filename > strlen (yytext));
   int scan_rc = sscanf (yytext, "# %zd \"%[^\"]\"", &linenr, filename);
   if (scan_rc != 2) {
      errprintf ("%s: invalid directive, ignored\n", yytext);
   }else {
      if (yy_flex_debug) {
         fprintf (stderr, "--included # %zd \"%s\"\n",
                  linenr, filename);
      }
      fprintf(outTokFile, "# %3ld  \"%s\"\n", linenr, filename);
      lexer::lloc.linenr = linenr - 1;
      lexer::newfilename (filename);
   }
}

void yyerror (const char* message) {
   assert (not lexer::filenames.empty());
   errllocprintf (lexer::lloc, "%s\n", message);
}

