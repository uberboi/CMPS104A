// Elizabeth Cepernich (eceperni@ucsc.edu)
// Leah Langford (llangfor@ucsc.edu)
// CMPS 104A Fall 2016
// Assignment 5: .oil file

#include <string>
#include <string.h>
#include <iostream>
#include <libgen.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "emit.h"

int stringcon_nr=1;
int intcon_nr=1;
int charcon_nr=1;

void emit_header(FILE* oilfile)  //DONE
{
    fprintf(oilfile,"#define __OCLIB_C__\n");
    fprintf(oilfile, "#include \"oclib.oh\"\n\n");
}

void emit_typedef(FILE* oilfile, astree* node, astree* parent)
{
    
    if (strcmp(node->lexinfo->c_str(),"int")==0)
    {
        fprintf(oilfile, "int");
        if (node->attr[ATTR_array])
        {
            fprintf(oilfile, "*");
        }
        fprintf(oilfile, " ");
    }
    else if (strcmp(node->lexinfo->c_str(),"string")==0)
    {
        fprintf(oilfile, "char*");
        if (node->attr[ATTR_array])
        {
            fprintf(oilfile, "*");
        }
        fprintf(oilfile, " ");
    }
    else
    {
        fprintf(oilfile, "struct %s*", parent->emit_code);
        if (node->attr[ATTR_array])
        {
            fprintf(oilfile, "*");
        }
        fprintf(oilfile, " ");
    }

}

// EMIT STRUCTDEF
void emit_structdef(FILE* oilfile, astree* root)
{
    emit_structdecl(oilfile, root);
    for (astree* child: root->children) 
    {
      emit_structdecl(oilfile, child);
    }
}
void emit_structdecl(FILE* oilfile, astree* node)
{
    if (node->symbol==TOK_STRUCT)
    {
        astree* left=nullptr;
        astree* right=nullptr;
        if (node->children.size()>=1)
        {
            left=node->children[0];
            fprintf(oilfile, "struct ");
            std::string structname="s_";
            structname.append(left->lexinfo->c_str());
            node->emit_code=structname.c_str();
            fprintf(oilfile, "%s {\n", node->emit_code);
            if (node->children.size()>=2)
            {
                right=node->children[1];
                astree* rleft=nullptr;
                if (right->children.size()>=1)
                {
                    while(right!=nullptr)
                    {
                        fprintf(oilfile, "\t");
                        rleft=right->children[0];
                        emit_typedef(oilfile, right, node);
                        std::string fieldname="f_";
                        fieldname.append(left->lexinfo->c_str());
                        fieldname.append("_");
                        fieldname.append(rleft->lexinfo->c_str());
                        right->emit_code=fieldname.c_str();
                        fprintf(oilfile, "%s \n", right->emit_code);

                        if (right->children.size()>=2)
                        {
                            right=right->children[1];
                        }
                        else
                        {
                            right=nullptr;
                        }
                    }

                }
            }   
            fprintf(oilfile, "\n}\n");    
        }
    }
}

// EMIT STRINGDEF
void emit_stringdef(FILE* oilfile, astree* node){
    emit_stringcon(oilfile, node);
    for (astree* child: node->children) 
    {
      emit_stringdef(oilfile, child);
    }
}
void emit_stringcon(FILE* oilfile, astree* node)
{
    if (node->symbol==TOK_STRINGCON)
    {
        std::string varname = "s";
        varname += std::to_string(stringcon_nr);
        node->emit_code=varname.c_str();

        fprintf(oilfile, "char* %s = %s \n", node->emit_code, 
                                             node->lexinfo->c_str());
        stringcon_nr++;
    }
}

// EMIT VARDEF
void emit_vardef(FILE* oilfile, astree* node)
{
    emit_vardecl(oilfile, node);
    for (astree* child: node->children) 
    {
      emit_vardecl(oilfile, child);
    }
}

void emit_vardecl(FILE* oilfile, astree* node)
{
    if (node->symbol==TOK_VARDECL)
    {
        astree* left=nullptr;
        astree* left2=nullptr;
        std::string varname="__";
        if (node->children.size()>=1)
        {
            left=node->children[0];
            if (left->children.size()>=1)
            {
                left2=left->children[0];
                varname.append(left2->lexinfo->c_str());
                left->emit_code=varname.c_str();
                left2->emit_code=varname.c_str();
                fprintf(oilfile, "%s %s;\n", left->lexinfo->c_str(),
                                             left2->emit_code);
            }
        }
    }
}

// EMIT FUNCTION
void emit_function(FILE* oilfile, astree* node)
{
    //emit_function(oilfile, node);
    if (node->symbol==TOK_FUNCTION)
    {
        if (node->children.size()>=1)
        {
            emit_function_name(oilfile, node);
        }
        if (node->children.size()>=2)
        {
             emit_function_params(oilfile, node);
        }
        if (node->children.size()>=3)
        {
            emit_function_body(oilfile, node);
        }
    }
    for (astree* child: node->children) 
    {
      emit_function(oilfile, child);
    }
}
void emit_function_name(FILE* oilfile, astree* node)
{
    astree* left=node->children[0];
    astree* left2=left->children[0];
    fprintf(oilfile, "%s __%s (",left->lexinfo->c_str(), 
                                 left2->lexinfo->c_str());
}
void emit_function_params(FILE* oilfile, astree* node)
{
    astree* paramhead = nullptr;
    if(node->children.size()>=2)
    {
        fprintf(oilfile, "\n");
        paramhead = node->children[1]->children[0];
        fprintf(oilfile, "%s",paramhead->emit_code);
        astree* plist=nullptr;
        astree* nexthead=nullptr;
        int i=1;

        if (paramhead->children.size()>=1)
        {
            plist=paramhead->children[0];
                fprintf(oilfile, "\t%s _%zu_%s",
                    paramhead->lexinfo->c_str(),
                    paramhead->block_nr, plist->lexinfo->c_str());
        }

        if (paramhead->children.size()>=2)
        {
            nexthead=paramhead->children[i];
            while(nexthead!=nullptr)
            {
                plist=nexthead->children[0];
                fprintf(oilfile, ";\n\t%s _%zu_%s",
                    nexthead->lexinfo->c_str(),
                    nexthead->block_nr,plist->lexinfo->c_str());

                if (paramhead->children.size()>=i+1)
                {
                    i++;
                    nexthead=paramhead->children[i];
                }
                else
                {
                    nexthead=nullptr;
                }
            }
        }
    }
    fprintf(oilfile, ") \n");

}
void emit_function_body(FILE* oilfile, astree* node)
{
    fprintf(oilfile, "{\n");
    astree* head=node->children[2];

    fprintf(oilfile, "}\n");
}

void emit_binop(FILE* oilfile, astree* node) //WIP
{
    astree* left=node->children[0];
    astree* right=node->children[1];
}

void emit_unop(FILE* oilfile, astree* node) //WIP
{
    astree* left=node->children[0];
}

void emit_expression(FILE* oilfile, astree* node)
{
    switch(node->symbol)
    {
        case '+': {
            emit_binop(oilfile, node);
            break;
        }
        case '-': { 
            emit_binop(oilfile, node);
            break;
        }
        case '*': {
            emit_binop(oilfile, node);
            break;
        }
        case '/': {
            emit_binop(oilfile, node);
            break;
        }
        case '%': {
            emit_binop(oilfile, node);
            break;
        }
        case TOK_GT: {
            emit_binop(oilfile, node);
            break;
        }
        case TOK_LT: {
            emit_binop(oilfile, node);
            break;
        }
        case TOK_GE: {
            emit_binop(oilfile, node);
            break;
        }
        case TOK_LE: {
            emit_binop(oilfile, node);
            break;
        }        
        case TOK_EQ: {
            emit_binop(oilfile, node);
            break;
        }
        case TOK_NE: {
            emit_binop(oilfile, node);
            break;
        }
        case '=': {
            emit_binop(oilfile, node);
            break;
        }
        case TOK_POS: {
            emit_unop(oilfile, node);
            break;
        }
        case TOK_NEG: {
            emit_unop(oilfile, node);
            break;
        }
        default: break;
    }
}

// More types
void emit_intcon(FILE* oilfile, astree* node)
{
    string varname = "a";
    varname += std::to_string(intcon_nr);
    node->emit_code=varname.c_str();

    fprintf(oilfile, "int* %s = %s\n ", node->emit_code, 
                                        node->lexinfo->c_str());
    intcon_nr++;
}
void emit_charcon(FILE* oilfile, astree* node)
{
    string varname = "c";
    varname += std::to_string(charcon_nr);
    node->emit_code=varname.c_str();

    fprintf(oilfile, "char** %s = %s", node->emit_code, 
                                      node->lexinfo->c_str());
    charcon_nr++;
}

// CALL METHODS
void emit_call_name(FILE* oilfile, astree* node) //DONE
{
    astree* left=node->children[0];
    fprintf(oilfile, "\t__%s",left->lexinfo->c_str());
}
void emit_call_params(FILE* oilfile, astree* node)
{
    fprintf(oilfile, "(");
    astree* paramhead = nullptr;
    if(node->children.size()>=2)
    {
        paramhead = node->children[1];
        fprintf(oilfile, "%s",paramhead->emit_code);
        int i=0;
        astree* plist=nullptr;
        if (paramhead->children.size()>=1)
        {
            plist=paramhead->children[i];
            while(plist!=nullptr)
            {
                fprintf(oilfile, ";\n %s",plist->emit_code);
                //maybe switch 
                i++;
                if (paramhead->children.size()>=(i+1))
                {
                    plist=paramhead->children[i];
                }
                else
                {
                    plist=nullptr;
                }
            }
        }
    }
    fprintf(oilfile, ") \n");
}

// IF-ELSE-WHILE methods
void whileloop(FILE* oilfile, astree* node){
    fprintf(oilfile, "while_%zd_%zd_%zd:;\n", 
        node->lloc.filenr, node->lloc.linenr, node->lloc.offset);
    emit(oilfile, node->children[0]);
    fprintf(oilfile, "\t__(!if%s) go to break_%zd_%zd_%zd:;\n", 
        node->children[0]->lexinfo->c_str(),
        node->lloc.filenr, node->lloc.linenr, node->lloc.offset);
    emit(oilfile,node->children[1]);
    fprintf(oilfile, "\t__goto while_%zd_%zd_%zd:;",
        node->lloc.filenr, node->lloc.linenr, node->lloc.offset);
    fprintf(oilfile, "break_%zd_%zd_%zd:;\n", 
        node->lloc.filenr, node->lloc.linenr, node->lloc.offset);
}

void ifelse(FILE* oilfile, astree* node){
    emit(oilfile, node->children[0]);
    fprintf(oilfile, "\t__(!if%s) go to else_%zd_%zd_%zd:;\n",
        node->children[0]->lexinfo->c_str(),
        node->lloc.filenr, node->lloc.linenr, node-> lloc.offset);
    emit(oilfile, node->children[1]);
    fprintf(oilfile, "\t__goto fi_%zd_%zd_%zd;\n",
    node->lloc.filenr, node->lloc.linenr, node->lloc.offset);
    fprintf(oilfile, "else_%zd_%zd_%zd:;\n", 
        node->lloc.filenr, node->lloc.linenr, node-> lloc.offset);
    emit(oilfile, node->children[2]);
    fprintf(oilfile, "fi_%zd_%zd_%zd\n",
    node->lloc.filenr, node->lloc.linenr, node -> lloc.offset);
}

void if_(FILE* oilfile, astree* node){
    emit(oilfile, node-> children[0]);
    fprintf(oilfile, "\t__(!if%s) go to fi_%zd_%zd_%zd;\n",
        node->children[0]->lexinfo->c_str(),
        node->lloc.filenr,node->lloc.linenr, node->lloc.offset);
    emit(oilfile, node->children[1]);
        fprintf(oilfile, "fi_%zd_%zd_%zd:;\n",
        node->lloc.filenr, node->lloc.linenr, node->lloc.offset);

}

void emit(FILE* oilfile, astree* node)
{
    switch(node->symbol)
    {
        case TOK_PROTOTYPE:
        case TOK_STRINGCON: 
        case TOK_INTCON: 
        case TOK_CHARCON: 
        {
            break;
        }
        case TOK_FUNCTION: {
            break;
        }
        case TOK_CALL: 
        {
            emit_call_name(oilfile, node);
            emit_call_params(oilfile, node);
            break;
            //expected result:__puts(s1)
        }
        case TOK_WHILE:
        {
            whileloop(oilfile, node);
            break;
        }
        case TOK_IF:
        {
            if_(oilfile, node);
            break;
        }
        case TOK_IFELSE:
        {
            ifelse(oilfile, node);
            break;
        }
        default:
        {
            break;
        }
    }
}

// EMIT MAIN for emit_everything
void emit_main(FILE* oilfile, astree* root)
{
    fprintf(oilfile, "\nvoid __ocmain(void)\n{ \n");
    emit(oilfile, root);
    for (astree* child: root->children) 
    {
      emit(oilfile, child);
    }
    fprintf(oilfile, "}\n");
}

// EMIT PROGRAM for emit_everything
void emit_program(FILE* oilfile, astree* node)
{
    emit_structdef(oilfile, node);
    emit_stringdef(oilfile, node);
    emit_vardef(oilfile, node);
    emit_function(oilfile, node);
}

//helps tp check and see if operand is a child
int emit_operands(astree* node){
    return 
      node->symbol == TOK_IDENT
    ||node->symbol == TOK_INTCON
    ||node->symbol == TOK_CHARCON;
}

// main function call 
void emit_everything(FILE* oilfile, astree* root)
{
    emit_header(oilfile);
    emit_program(oilfile, root);
    emit_main(oilfile, root);
}




