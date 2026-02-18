#ifndef _READ_TREE_BY_RECURSIVE_DESCENT_H_
#define _READ_TREE_BY_RECURSIVE_DESCENT_H_

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>

#include "tree.h"
#include "differentiator.h"

const size_t VAR_MAX_SIZE                = 16;
const size_t MAX_LEGHT_OF_DATA         = 4096;
const size_t MAX_LENGHT_OF_TEMP_BUFFER = 4096;

enum DescentSyntaxEror_t{
    INCORRECT_EQUATION,
    INCORRECT_VALUE_NAME,
};

enum AvaliableVariables{
    x,
    y,
    z,
    t
};

struct MathExpression{
    ExpressionTree_t* tree;
    LoadNodeProgress* load_progress;
    Node_Info* node_info;
};

struct TreeFile {
    char** buffer;
    size_t buff_size_in_bytes;
    size_t buff_lenght;
};

Node_t* GetG(MathExpression* math_exp , char** exp_from_file);
Node_t* GetP(MathExpression* math_exp , char** exp_from_file);
Node_t* GetN(MathExpression* math_exp , char** exp_from_file);
Node_t* GetE(MathExpression* math_exp , char** exp_from_file);
Node_t* GetT(MathExpression* math_exp , char** exp_from_file);
Node_t* GetA(MathExpression* math_exp , char** exp_from_file);
Node_t *GetV(MathExpression *math_exp , char **exp_from_file);
Node_t* GetF(MathExpression* math_exp , char** exp_from_file);

void ReadDataFromFile(TreeFile *text_tree, FILE *tree_file_ptr, ExpressionTree_t *tree, MathExpression *math_exp);
void SkipSpaces(char** exp);

#endif //_READ_TREE_BY_RECURSIVE_DESCENT_H_
