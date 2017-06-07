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
#include "symtable.h"

FILE * outTokFile;
FILE * outSymFile;
const string CPP = "/usr/bin/cpp";
const size_t LINESIZE = 1024;
int exit_status = 0;

int main (int argc, char** argv){
  FILE * outStrFile;
  FILE * outAstFile;
  FILE * outOilFile;
  int flag;
  int dFlag = 0;
  yy_flex_debug = 0;
  yydebug = 0;
  string d_opt = "";
  string astFile = "";
  string strFile = "";
  string tokFile = "";
  string symFile = "";
  string oilFile = "";
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
  
  astFile = baseFile + ".ast";  
  strFile = baseFile + ".str";
  tokFile = baseFile + ".tok";
  symFile = baseFile + ".sym";
  oilFile = baseFile + ".oil";
  outAstFile = fopen(astFile.c_str(), "w");
  outStrFile = fopen(strFile.c_str(), "w");
  outTokFile = fopen(tokFile.c_str(), "w");
  outSymFile = fopen(symFile.c_str(), "w");
  outOilFile = fopen(oilFile.c_str(), "w");

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
     yyparse();
  }
  fclose(outTokFile); 
  int pclose_rc = pclose(yyin);
  eprint_status (command.c_str(), pclose_rc);    

  traverseAttributes(yyparse_astree);
  treeTraversal(yyparse_astree);
  pclose(outSymFile);
 
  astree::print(outAstFile, yyparse_astree, 0);
  pclose(outAstFile); 
  
  pclose(outOilFile);
  
  string_set::dump(outStrFile);
  pclose(outStrFile);
  return exec::exit_status;
}



















