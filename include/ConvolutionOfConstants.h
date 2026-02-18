#ifndef _EQUATION_SIMPLIFY_
#define _EQUATION_SIMPLIFY_

#define MAX_STR_SIZE 256 

#include <assert.h>

#include "tree.h"
#include "latexDump.h"
#include "differentiator.h"

void ChangeNodeToNumber (Node_t* node, double num_value, ExpressionTree_t* tree);
double CalculationNode (Node_t* node);
void ReplaceWithLeftChild (Node_t* node);
void ReplaceWithRightChild (Node_t* node);
int IsZero (Node_t* node);
int IsOne (Node_t* node);
int AreNodesEqual (Node_t* a, Node_t* b);
int AreNodeValuesEqual (union expr_type a, union expr_type b);
void LatexDumpSimplifyStep (ExpressionTree_t* tree, const char* rule);
bool SimplifyUntilStable (ExpressionTree_t* tree, int max_iterations, LatexDumpState* latex_dump);
bool SimplificationTree (Node_t* node, ExpressionTree_t* tree, bool* changed, LatexDumpState* latex_dump);
void SimplificatoinPrimeNumbers (Node_t* node, ExpressionTree_t* tree, bool* changed, LatexDumpState* latex_dump);

void SimplifyingSub (Node_t* node, ExpressionTree_t* tree, bool* changed, LatexDumpState* latex_dump);
void SimplifyingMul (Node_t* node, ExpressionTree_t* tree, bool* changed, LatexDumpState* latex_dump);
void SimplifyingDiv (Node_t* node, ExpressionTree_t* tree, bool* changed, LatexDumpState* latex_dump);
void SimplifyingPow (Node_t* node, ExpressionTree_t* tree, bool* changed, LatexDumpState* latex_dump);
void SimplifyingAdd (Node_t* node, ExpressionTree_t* tree, bool* changed, LatexDumpState* latex_dump);
#endif //_EQUATION_SIMPLIFY_
