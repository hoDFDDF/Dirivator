#ifndef _LATEX_H_
#define _LATEX_H_

#include <stdio.h>

#include "tree.h"

#define MAX_LATEX_EXPRESSION_LENGTH 4096
#define MAX_STR_SIZE 256

typedef struct {
    FILE* file_ptr;
    const char* filename;
    int step_counter;
    int current_funny_index;
    const char** funny_phrases;
    int funny_phrases_count;
} LatexDumpState;

void CtorLatexDump(LatexDumpState* state, const char* filename);
void DtorLatexDump(LatexDumpState* state);
void AddLatexStep(LatexDumpState* state, const char* description, ExpressionTree_t* tree);
void PrintTreeLatex(FILE* file, Node_t* node);
void NodeToLatexString(Node_t* node, char* buffer, int* pos, int buffer_size);

int StartLatexDump(FILE* file);
int EndLatexDump(FILE* file);

#endif // LATEX_H_
