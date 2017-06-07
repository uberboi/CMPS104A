// $Id: astree.h,v 1.7 2016-10-06 16:13:39-07 - - $
//Steven Huang  
//sthuang
//Kevin Cai     
//kcai2

#ifndef __ASTREE_H__
#define __ASTREE_H__

#include <bitset>
#include <string>
#include <vector>
using namespace std;

#include "auxlib.h"
#include "symtable.h"
using attr_bitset = bitset<15>;

struct location {
   size_t filenr;
   size_t linenr;
   size_t offset;
};

struct astree {

   // Fields.
   int symbol;               // token code
   location lloc;            // source location
   const string* lexinfo;    // pointer to lexical information
   vector<astree*> children; // children of this n-way node

   attr_bitset attributes;
   int block_nr;
   const string* structtype;
   const string* functype;
   const string* fieldtype;   

   // Functions.
   astree (int symbol, const location&, const char* lexinfo);
   ~astree();
   astree* adopt (astree* child1, astree* child2 = nullptr);
   astree* adopt_sym (astree* child, int symbol);
   astree* swap_sym (astree* tree, int symbol);
   void dump_node (FILE*);
   void dump_tree (FILE*, int depth = 0);
   static void dump (FILE* outfile, astree* tree);
   static void print (FILE* outfile, astree* tree, int depth = 0);
};

void destroy (astree* tree1, astree* tree2 = nullptr);

void errllocprintf (const location&, const char* format, const char*);

#endif

