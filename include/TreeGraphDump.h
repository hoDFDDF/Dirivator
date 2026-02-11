#ifndef _GRAPH_DUMP_
#define _GRAPH_DUMP_

#include <stdlib.h>
#include <stdio.h>

#include "tree.h"


/////CONST COLORS////
static const char* white      = "#fdfdfd";
static const char* turquoise  = "#2799a0";
static const char* swamp      = "#445c00";
static const char* pink       = "#ffabd1";
static const char* dark_blue  = "#001f29";
static const char* brown      = "#5f3035";

///CONST COLORS//////


void DrawTree(ExpressionTree_t* tree, Node_t* node, FILE* dot_file);
void CreateNode(Node_t* node, FILE* dot_file, ExpressionTree_t* tree);
void MakeArrow(Node_t* node, ExpressionTree_t* tree, FILE*  dot_file);
void SetRankToNodes(ExpressionTree_t* tree, Node_Info* progress, FILE* dot_file);
void FinishDotFile(FILE* dot_file);
void ProgressTreeInit(Node_Info* progress, ExpressionTree_t* tree);
void RecursiveFillNodeInfoProgress(Node_Info* progress, Node_t* node, size_t rank);

#endif //_GRAPH_DUMP_

