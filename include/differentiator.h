#ifndef _DIFFER_H_
#define _DIFFER_H_

#include "DSL.h"
#include "RecursiveDescentReadTree.h"

enum TypeValue {
    ADD     =  1,
    SUB     =  2,
    MUL     =  3,
    DIV     =  4,
    POW     =  5, 
    SIN     =  6,
    COS     =  7,
    TAN     =  8,
    LN      =  9,
    EXP     = 10,
    NOTTYPE =  11
};

Node_t* DifferentiateNode (ExpressionTree_t* tree, Node_t* node, char variable);
Node_t* DifferentiateOperator(ExpressionTree_t* tree, Node_t* node, char variable);
Node_t* CopySubtree (ExpressionTree_t* tree, Node_t* node);
Node_t* CreateOperatorNodeByEnum(ExpressionTree_t* tree, int op_code, Node_t* node_left, Node_t* node_right);

int get_operator_code(char* op_str);

char* OperatorToString(int op);

#endif //_DIFFER_H_