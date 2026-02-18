#ifndef _TREE_H_
#define _TREE_H_

#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#include "TreeError_t.h"

#define max(a, b) ((a) > (b) ? (a) : (b))

union expr_type{
    char* operat;
    char* varible;
    int numb;
};

enum InputEquationType{
    OPERATORTYPE,
    VARIABLETYPE,
    NUMBERTYPE, 
    NOTYPE
};

struct Node_t{
    Node_t* left_child;
    Node_t* right_child;
    Node_t* parent;
    expr_type tree_data;
    InputEquationType type;
};

struct ExpressionTree_t{
    Node_t* root;
    size_t size;
};

struct LoadNodeProgress{
    Node_t* node;
    size_t rank;
};

struct Node_Info{ 
    LoadNodeProgress* nodes_array;
    size_t capacity; 
    size_t size;
};


enum Child {
    LEFT_CHILD,
    RIGHT_CHILD
};

Node_t* TreeDtor(ExpressionTree_t* tree, Node_t* node);
Node_t* NodeCtor(expr_type data, ExpressionTree_t* tree, int type);
Node_t* DeleteSubTree(Node_t* node);
Node_t* CreateOperatorNode(ExpressionTree_t* tree, char* op_code, Node_t* node_left, Node_t* node_right);
Node_t* CreateVariableNode(ExpressionTree_t* tree, char* var_code);
Node_t* CreateNumberNode(ExpressionTree_t* tree, int numb_code);

TreeError_t TreeInsert(ExpressionTree_t* tree, Node_t* node, expr_type data, Child child);
TreeError_t TreeDelete(ExpressionTree_t* tree, Node_t* node);

void TreeCtor(ExpressionTree_t* tree);
void PrintTreeToTXT(Node_t* node, FILE* file_ptr);

size_t MaxDepthOfBInaryTree(Node_t* node);

#endif //_TREE_H_