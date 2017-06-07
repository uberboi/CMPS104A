//Steven Huang  
//sthuang
//Kevin Cai     
//kcai2

#ifndef __SYMTABLE_H__
#define __SYMTABLE_H__

#include "auxlib.h"
#include "astree.h"

#include <stdio.h>
#include <string>
#include <vector>
#include <map>
#include <bitset>
#include <unordered_map>
using namespace std;

enum { ATTR_void, ATTR_int, ATTR_null, ATTR_string,
       ATTR_struct, ATTR_array, ATTR_function, ATTR_variable,
       ATTR_field, ATTR_typeid, ATTR_param, ATTR_lval, ATTR_const,
       ATTR_vreg, ATTR_vaddr, ATTR_bitset_size,
};
using attr_bitset = bitset<ATTR_bitset_size>;

struct symbol;
struct astree;
using symbol_table = unordered_map<const string*, symbol*>;
using symbol_entry = symbol_table::value_type;

struct symbol {
   attr_bitset attributes;
   symbol_table* fields;
   size_t filenr, linenr, offset;
   size_t block_nr;
   vector<symbol*>* parameters;
   const string* structtype;
   const string* fieldtype;
};

symbol* newSymbol(astree* node);
const char* checkAttributes(astree* node);
void insertSymbol(symbol_table table, const string *key,
                  symbol *sym, astree* node);
void setAttributes(astree* node);
void traverseAttributes(astree* root);
void insertStructSymbol(astree *root);
void insertFuncSymbol(astree *root);
void insertProtoSymbol(astree *root);
void treeTraversal(astree *root);
#endif
