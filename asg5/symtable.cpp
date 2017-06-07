//Steven Huang  
//sthuang
//Kevin Cai     
//kcai2

#include "auxlib.h"
#include "symtable.h"
#include "lyutils.h"
#include "string_set.h"
#include "astree.h"

#include <bitset>
#include <string>
#include <unordered_map>
#include <vector>
#include <string>
#include <string.h>
#include <iostream>

using namespace std;

vector<symbol_table*> symbol_stack = {new symbol_table, nullptr};
int next_block = 1;
vector<int> block_count;
extern FILE * outSymFile;

symbol* newSymbol(astree* node){
   if(symbol_stack.empty()){
      symbol_stack.push_back(new symbol_table);
      symbol_stack.push_back(nullptr);
   }
   if(block_count.empty()){
      block_count.push_back(0);
   }
   symbol *sym = new symbol();

   sym->fields=nullptr;
   sym->parameters=nullptr;
   sym->structtype=nullptr;
   sym->fieldtype=nullptr;
   sym->attributes=node->attributes;
   sym->block_nr=block_count.back();
   sym->filenr=node->lloc.filenr;
   sym->linenr=node->lloc.linenr;
   sym->offset=node->lloc.offset;
   return sym;
}

const char* checkAttributes(astree* node){
   attr_bitset attr_arr= node->attributes;
   string s = "";
   if(attr_arr[ATTR_field]){
      s = s +"field ";
      if(node->fieldtype){
         s = s + "{" + *(node->fieldtype) + "} ";
      }
   }
   if(attr_arr[ATTR_void]){
      s = s +"void ";
   }
   if(attr_arr[ATTR_int]){
      s = s +"int ";
   }
   if(attr_arr[ATTR_null]){
      s = s +"null ";
   }
   if(attr_arr[ATTR_string]){
      s = s +"string ";
   }
   if(attr_arr[ATTR_struct]){
      s = s +"struct ";
      if(node->structtype){
         s = s + "\"" + *(node->structtype) + "\" ";
      }
   }
   if(attr_arr[ATTR_array]){
      s = s +"array ";
   }
   if(attr_arr[ATTR_function]){
      s = s +"function ";
   }
   if(attr_arr[ATTR_variable]){
      s = s +"variable ";
   }
   if(attr_arr[ATTR_typeid]){
      s = s +"typeid ";
   }
   if(attr_arr[ATTR_param]){
      s = s +"param ";
   }
   if(attr_arr[ATTR_lval]){
      s = s +"lval ";
   }
   if(attr_arr[ATTR_const]){
      s = s +"const ";
   }
   return s.c_str();
}

void insertSymbol(symbol_table table, const string* key, 
                symbol* sym, astree* node){
   table[key]=sym;
   for(size_t i=1; i<block_count.size(); i++){
      fprintf(outSymFile, "  ");
      }
   fprintf(outSymFile, "%s (%zd.%zd.%zd) {%zd} %s\n",
           key->c_str(),sym->filenr, sym->linenr, sym->offset,
           sym->block_nr,checkAttributes(node));
}

void setAttributes(astree* node){
   int sym = node->symbol;
   if(sym==TOK_VOID){
      node->attributes[ATTR_void]=true;
      if(node->children.size()>0){
         node->children[0]->attributes[ATTR_void]=true;
      }
   }else if(sym==TOK_INT){
      node->attributes[ATTR_int]=true;
      node->attributes[ATTR_const]=true;
      if(node->children.size()>0){
         node->children[0]->attributes[ATTR_int]=true;
         node->children[0]->structtype=node->lexinfo;
      }
   }else if(sym==TOK_NULL){
      node->attributes[ATTR_null]=true;
      node->attributes[ATTR_const]=true;
   }else if(sym==TOK_STRING){
      node->attributes[ATTR_string]=true;
      node->attributes[ATTR_lval]=true;
      if(node->children.size()>0){
         node->children[0]->attributes[ATTR_string]=true;
      }
   }else if(sym==TOK_STRUCT){
      node->attributes[ATTR_struct]=true;
      node->structtype=node->children[0]->lexinfo;
      node->attributes[ATTR_variable]=false;
      node->attributes[ATTR_lval]=false;

      node->children[0]->attributes[ATTR_struct]=true;
      node->children[0]->attributes[ATTR_variable]=false;
      node->children[0]->attributes[ATTR_lval]=false;
   }else if(sym==TOK_ARRAY){
      node->attributes[ATTR_array]=true;
   }else if(sym==TOK_FUNCTION){
      node->attributes[ATTR_function]=true;
      node->attributes[ATTR_variable]=false;
      node->attributes[ATTR_lval]=false;

      node->children[0]->attributes[ATTR_function]=true;
      node->children[0]->attributes[ATTR_variable]=false;
      node->children[0]->attributes[ATTR_lval]=false;
   }else if(sym==TOK_VARDECLAR){
      node->attributes[ATTR_variable]=true;
   }else if(sym==TOK_FIELD){
      node->attributes[ATTR_field]=true;
   }else if(sym==TOK_TYPEID){
      node->attributes[ATTR_typeid]=true;
      for(size_t i=0;i<node->children.size();i++){
         node->children[i]->attributes[ATTR_struct]=true;
         node->children[i]->structtype=node->lexinfo;
      }
   }else if(sym==TOK_PARAMLIST){
      for(size_t i=0;i<node->children.size();i++){
         node->children[i]->attributes[ATTR_param]=true;
      }
   }else if(sym==TOK_INTCON || sym==TOK_CHARCON){
      node->attributes[ATTR_int]=true;
      node->attributes[ATTR_const]=true;
      node->attributes[ATTR_lval]=true;
   }else if(sym==TOK_STRINGCON){
      node->attributes[ATTR_string]=true;
      node->attributes[ATTR_const]=true;
      node->attributes[ATTR_lval]=true;
   }else if(sym==TOK_DECLID){
      node->attributes[ATTR_variable]=true;
      node->attributes[ATTR_lval]=true;
   }else if(sym==TOK_IDENT){
      node->attributes[ATTR_variable]=true;
      node->attributes[ATTR_lval]=true;
   }else if(sym==TOK_PROTOTYPE){
      node->children[0]->children[0]->attributes[ATTR_variable]=false;
      node->children[0]->children[0]->attributes[ATTR_lval]=false;
   }else if(sym==TOK_CHAR){
      //There is no char attribute
   }else if(sym==('.')){
      node->children[0]->attributes[ATTR_struct]=true;
      node->children[1]->attributes[ATTR_field]=true;
   }

}

void traverseAttributes(astree* root){
   for(size_t i=0; i<root->children.size(); i++){
      traverseAttributes(root->children[i]);
   }
   setAttributes(root);
}

void insertStructSymbol(astree* root){
   if(root->children.size()>0){
      root->children[0]->structtype=root->children[0]->lexinfo;
      root->children[0]->attributes[ATTR_typeid]=false;
   }
   symbol* sym = newSymbol(root->children[0]);
   symbol_table fieldloc;
   sym->fields=&fieldloc;
   sym->structtype=root->children[0]->lexinfo;
   insertSymbol(*symbol_stack[0], root->children[0]->lexinfo, 
                sym,root->children[0]);
   for(size_t i=0; i<root->children[1]->children.size(); i++){
      fprintf(outSymFile,"  ");
      astree* field = root->children[1]->children[i]->children[0];
      field->fieldtype=root->children[0]->lexinfo;
      field->structtype=root->children[1]->children[i]->lexinfo;
      sym=newSymbol(field);
      insertSymbol(fieldloc,field->lexinfo,sym,field);
   }
}
void insertFuncSymbol(astree *root){
   root->children[0]->children[0]->functype=root->children[0]->lexinfo;
   root->children[0]->children[0]->attributes[ATTR_function]=true;
   root->children[0]->children[0]->attributes[ATTR_lval]=false;
   root->children[0]->children[0]->attributes[ATTR_variable]=false;
   symbol *sym = newSymbol(root->children[0]->children[0]);
   vector<symbol*> paramloc;
   sym->parameters=&paramloc;
   insertSymbol(*symbol_stack[0],
      root->children[0]->children[0]->lexinfo,sym,
      root->children[0]->children[0]);
   astree* params = root->children[1];
   for(size_t i=0; i<params->children.size(); i++){
      params->children[i]->children[0]->
         structtype=params->children[i]->lexinfo;
      params->children[i]->children[0]->attributes[ATTR_param]=true;
      sym=newSymbol(params->children[i]->children[0]);
      sym->structtype=params->children[i]->children[0]->structtype;
      sym->block_nr++;
      paramloc.push_back(sym);
      fprintf(outSymFile,"  ");
      insertSymbol(*symbol_stack[0],
                   params->children[i]->children[0]->lexinfo,sym,
                   params->children[i]->children[0]);
   }
   block_count.push_back(next_block);
   next_block++;
   symbol_stack[block_count.back()]=new symbol_table;
   symbol_stack.push_back(nullptr);
   treeTraversal(root->children[2]);
   block_count.pop_back();
}

void insertProtoSymbol(astree *root){
   root->children[0]->children[0]->functype=root->children[0]->lexinfo;
   root->children[0]->children[0]->attributes[ATTR_function]=true;
   root->children[0]->children[0]->attributes[ATTR_lval]=false;
   root->children[0]->children[0]->attributes[ATTR_variable]=false;
   symbol *sym = newSymbol(root->children[0]->children[0]);
   vector<symbol*> paramloc;
   sym->parameters=&paramloc;
   insertSymbol(*symbol_stack[0], 
                root->children[0]->children[0]->lexinfo, sym,
                root->children[0]->children[0]);
   astree* params = root->children[1];
   for(size_t i=0; i<params->children.size(); i++){
      params->children[i]->children[0]->
         structtype=params->children[i]->lexinfo;
      params->children[i]->children[0]->attributes[ATTR_param]=true;
      sym = newSymbol(params->children[i]->children[0]);
      sym->block_nr++;
      paramloc.push_back(sym);
   }
}

void treeTraversal(astree *root){
   for(size_t i=0; i<root->children.size(); i++){
      int sym = root->children[i]->symbol;
      switch(sym){
         case '.':
            {
            symbol_table* base_tbl=symbol_stack[0];
            auto c1=base_tbl->find(root->children[i]->
                                   children[0]->lexinfo);
            if(c1 != base_tbl->end()){
               root->children[i]->children[0]->
                  attributes[ATTR_struct]=true;
               root->children[i]->children[0]->
                  structtype=c1->second->structtype;
               root->children[i]->children[1]->
                  attributes[ATTR_field]=true;
               root->children[i]->children[1]->
                  fieldtype=c1->second->fieldtype;
            }
            fprintf(outSymFile, "\n");
            }
         case TOK_STRUCT:
            insertStructSymbol(root->children[i]);
            fprintf(outSymFile, "\n");
            break;
         case TOK_FUNCTION:
            insertFuncSymbol(root->children[i]);
            fprintf(outSymFile, "\n");
            break;
         case TOK_VARDECLAR:
            {
            astree *varDeclar = root->children[i]->
                                children[0]->children[0];
            varDeclar->structtype=root->children[i]->
                                  children[0]->lexinfo;
            symbol* sym = newSymbol(varDeclar);
            sym->structtype=varDeclar->structtype;
            insertSymbol(*symbol_stack[block_count.back()], 
                         varDeclar->lexinfo,sym, varDeclar);
            break;
            }
         case TOK_PROTOTYPE:
            insertProtoSymbol(root->children[i]);
            fprintf(outSymFile, "\n");
            break;
         case TOK_IF:
            block_count.push_back(next_block);
            next_block++;
            symbol_stack[block_count.back()]=new symbol_table;
            symbol_stack.push_back(nullptr);
            treeTraversal(root->children[i]->children[1]);
            block_count.pop_back();
            break;
         case TOK_IFELSE:
            block_count.push_back(next_block);
            next_block++;
            symbol_stack[block_count.back()]=new symbol_table;
            symbol_stack.push_back(nullptr);
            treeTraversal(root->children[i]->children[1]);
            block_count.pop_back();
            block_count.push_back(next_block);
            next_block++;
            symbol_stack[block_count.back()]=new symbol_table;
            symbol_stack.push_back(nullptr);
            treeTraversal(root->children[i]->children[2]);
            block_count.pop_back();
            break;
         default:
            break;
      }
   }
}
