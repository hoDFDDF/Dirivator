#ifndef _LATEX_H_
#define _LATEX_H_

#include <stdio.h>

struct LatexDumpState {
      
    FILE* file_ptr;
    int latex_dump_steps;

};

int StartLatexDump (FILE* file_ptr);
int EndLatexDump (FILE* file_ptr);
int DumpOriginalFunctionToFile (FILE* file_ptr, ExpressionTree_t* tree, double result_value);
int DumpOptimizationStepToFile (FILE* file_ptr, const char* description, ExpressionTree_t* tree, double result_value);
int DumpDerivativeToFile (FILE* file_ptr, ExpressionTree_t* derivative_tree, double derivative_result, int derivative_order);

void NodeToLatexString (Node_t* node, char* buffer, int* pos, int buffer_size);
void CtorLatexDump (LatexDumpState* state, const char* filename);
void AddLatexStep (LatexDumpState* state, const char* description, ExpressionTree_t* tree);
void DtorLatexDump (LatexDumpState* state);
void PrintTreeLatex (FILE* file_ptr, Node_t* node);
void LatexDumpDifferentiationStep (LatexDumpState* state, const char* rule_used, ExpressionTree_t* original, ExpressionTree_t* derivative);
void LatexDumpSimplifyStep (LatexDumpState* state, const char* rule_used, ExpressionTree_t* before, ExpressionTree_t* after);
void LatexDumpDetailedProcess (ExpressionTree_t* original, ExpressionTree_t* derivative);


#endif //LATEX_H_