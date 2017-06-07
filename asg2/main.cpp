//Steven Huang  
//sthuang
//Kevin Cai     
//kcai2

#include <fstream>
#include <unistd.h>
#include <iostream>
#include <errno.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>
#include <string>
using namespace std;

#include "auxlib.h"
#include "string_set.h"
#include "lyutils.h"
#include "astree.h"

FILE * outTokFile;
const string CPP = "/usr/bin/cpp";
const size_t LINESIZE = 1024;
int exit_status = 0;

// Chomp the last character from a buffer if it is delim.
void chomp (char* string, char delim) {
   size_t len = strlen (string);
   if (len == 0) return;
       char* nlpos = string + len - 1;
   if (*nlpos == delim) *nlpos = '\0';
}

// Run cpp against the lines of the file.
void cpplines (FILE* pipe, const char* filename) {
   int linenr = 1;
   char inputname[LINESIZE];
   strcpy (inputname, filename);
   for (;;) {
      char buffer[LINESIZE];
      char* fgets_rc = fgets (buffer, LINESIZE, pipe);
      if (fgets_rc == NULL) break;
      chomp (buffer, '\n');
      DEBUGF ('T', "%s:line %d: [%s]\n", filename, linenr, buffer);
      // http://gcc.gnu.org/onlinedocs/cpp/Preprocessor-Output.html
      int sscanf_rc = sscanf (buffer, "# %d \"%[^\"]\"",
                              &linenr, inputname);
      if (sscanf_rc == 2) {
         DEBUGF ('T', "DIRECTIVE: line %d file \"%s\"\n",
                 linenr, inputname);
         continue;
      }
      char* savepos = NULL;
      char* bufptr = buffer;
      for (int tokenct = 1;; ++tokenct) {
         char* token = strtok_r (bufptr, " \t\n", &savepos);
         bufptr = NULL;
         if (token == NULL) break;
         DEBUGF ('T',"token %d.%d: [%s]\n",
                 linenr, tokenct, token);
                 string_set::intern(token);
      }
      ++linenr;
   }
}

int main (int argc, char** argv){
  FILE * outStrFile;
  //FILE * outTokFile;
  int flag;
  int dFlag = 0;
  yy_flex_debug = 0;
  string d_opt = "";
  string strFile = "";
  string tokFile = "";
  string baseFile = "";

  while ((flag = getopt (argc, argv, "ly::@::D::")) != -1){
     switch (flag){
        case 'l':
           yy_flex_debug = 1;
           break;
        case 'y':
           yydebug = 1;
           break;
        case '@':
           set_debugflags(optarg);
           break;
        case 'D':
           dFlag = 1;
           d_opt = string(optarg);
           //cout << d_opt;
           break;
        default:
           fprintf(stderr, "Option: [-ly] [-@flag...] [-D string]\n");
           exit_status = 1;
           exit(exit_status);
      }
    }

  if (optind>=argc){
     fprintf(stderr,"Missing Input File\n");
     exit_status=1;
     exit(exit_status);
  }

  exec::execname = basename (argv[0]);
  exec::exit_status = EXIT_SUCCESS;
  string filename = argv[optind];
  size_t lastindex = filename.find_last_of(".");
  size_t firstindex = filename.find_first_of(".");
  //cout << filename.substr(lastindex);
  //cout << "lastindex: " << lastindex;
  if (lastindex == std::string::npos || lastindex != firstindex 
      || filename.substr(lastindex).compare(".oc")){
     fprintf(stderr, "oc: must be a .oc file.\n");
     //Must be .oc file extension
     return 1;
  }else{
     baseFile = filename.substr(0, lastindex);
  }
  //cout << "Filename: " << baseFile << "\n";
  //cout <<"-D = \n" << d_opt;
      
  strFile = baseFile + ".str";
  tokFile = baseFile + ".tok";
  outStrFile = fopen(strFile.c_str(), "w");
  outTokFile = fopen(tokFile.c_str(), "w");

  string command;
  if (dFlag) {
     command = CPP + " -D" + d_opt + " " + filename;
  } else {
     command = CPP + " " + filename;
  }
  yyin = popen (command.c_str(), "r");
  if (yyin == NULL){
     exec::exit_status = EXIT_FAILURE;
     syserrprintf(command.c_str());
  }else{
     int token = yylex();
     while(token != YYEOF){
        //cout << "yylex() = " << token;
        //cout << "token: " << parser::get_tname(token);
        token = yylex();
     }
     fclose(outTokFile); 
     int pclose_rc = pclose(yyin);
     eprint_status (command.c_str(), pclose_rc);  
  }     
  string_set::dump(outStrFile);
  pclose(outStrFile);
  return exec::exit_status;
}



















