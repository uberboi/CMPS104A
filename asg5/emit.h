// Elizabeth Cepernich (eceperni@ucsc.edu)
// Leah Langford (llangfor@ucsc.edu)
// CMPS 104A Fall 2016
// Assignment 5: .oil file

#include "astree.h"
#include "symstack.h"
#include "lyutils.h"
#include "auxlib.h"
#include <string>
#include <iostream>
#include <libgen.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

void emit_header(FILE* oilfile);

void emit_program(FILE* oilfile, astree* node);

void emit_typedef(FILE* oilfile, astree* node, astree* parent);
void emit_structdef(FILE* oilfile, astree* node);
void emit_structdecl(FILE* oilfile, astree* node);

void emit_stringdef(FILE* oilfile, astree* node);

void emit_vardef(FILE* oilfile, astree* node);
void emit_vardecl(FILE* oilfile, astree* node);

void emit_expression(FILE* oilfile, astree* node);
void emit_binop(FILE* oilfile, astree* node);
void emit_unop(FILE* oilfile, astree* node);

void emit_stringcon(FILE* oilfile, astree* node);
void emit_intcon(FILE* oilfile, astree* node);
void emit_charcon(FILE* oilfile, astree* node);

// FUNCTION METHODS
void emit_function(FILE* oilfile, astree* node);
void emit_function_name(FILE* oilfile, astree* node);
void emit_function_params(FILE* oilfile, astree* node);
void emit_function_body(FILE* oilfile, astree* node);

void emit_call_name(FILE* oilfile, astree* node);
void emit_call_params(FILE* oilfile, astree* node);

void emit(FILE* oilfile, astree* node);

void emit_main(FILE* oilfile, astree* root);
void emit_everything(FILE* oilfile, astree* root);

