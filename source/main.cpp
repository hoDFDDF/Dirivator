#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include "RecursiveDescentReadTree.h"
#include "TreeGraphDump.h"
#include "tree.h"
#include "ConvolutionOfConstants.h"
#include "latexDump.h"

int main() {
    FILE* fp = fopen("Exception.txt", "r");
    if (!fp) {
        printf("Cannot open Exception.txt\n");
        return 1;
    }
    
    char buffer[256] = {0};
    if (!fgets(buffer, sizeof(buffer), fp)) {
        printf("Cannot read from file\n");
        fclose(fp);
        return 1;
    }
    fclose(fp);
    
    buffer[strcspn(buffer, "\n")] = '\0';
    printf("Expression: %s\n", buffer);
    
    ExpressionTree_t original_tree = {};
    TreeCtor(&original_tree);
    
    MathExpression math_exp = {&original_tree, 0, 0};
    char* str_ptr = buffer;
    Node_t* root = GetG(&math_exp, &str_ptr);
    
    if (!root) {
        printf("Parser failed\n");
        TreeDtor(&original_tree, original_tree.root);
        return 1;
    }
    
    original_tree.root = root;
    printf("Original tree built successfully\n");
    
    char variable = 'x';
    printf("Differentiating with respect to: %c\n", variable);
    
    Node_t* derivative_root = DifferentiateNode(&original_tree, original_tree.root, variable);
    
    if (!derivative_root) {
        printf("Differentiation failed!\n");
        TreeDtor(&original_tree, original_tree.root);
        return 1;
    }
    
    ExpressionTree_t derivative_tree = {};
    TreeCtor(&derivative_tree);
    derivative_tree.root = derivative_root;
    
    printf("Derivative tree built successfully\n");
    
    FILE* dot_original = fopen("original_tree.dot", "w");
    if (!dot_original) {
        printf("Cannot create original_tree.dot\n");
        TreeDtor(&original_tree, original_tree.root);
        TreeDtor(&derivative_tree, derivative_tree.root);
        return 1;
    }
    
    DrawTree(&original_tree, original_tree.root, dot_original);
    
    Node_Info progress_orig = {};
    ProgressTreeInit(&progress_orig, &original_tree);
    printf("Original tree nodes count: %zu\n", progress_orig.size);
    
    if (progress_orig.size == 0) {
        printf("No nodes in original tree!\n");
    } else {
        SetRankToNodes(&original_tree, &progress_orig, dot_original);
    }
    printf("PENIS");
    
    FinishDotFile(dot_original);
    fclose(dot_original);
    printf("Original tree saved to original_tree.dot\n");
    
    FILE* dot_derivative = fopen("derivative_tree.dot", "w");
    if (!dot_derivative) {
        printf("Cannot create derivative_tree.dot\n");
        TreeDtor(&original_tree, original_tree.root);
        TreeDtor(&derivative_tree, derivative_tree.root);
        return 1;
    }
    
    DrawTree(&derivative_tree, derivative_tree.root, dot_derivative);
    
    Node_Info progress_deriv = {};
    ProgressTreeInit(&progress_deriv, &derivative_tree);
    printf("Derivative tree nodes count: %zu\n", progress_deriv.size);
    
    if (progress_deriv.size == 0) {
        printf("No nodes in derivative tree!\n");
    } else {
        SetRankToNodes(&derivative_tree, &progress_deriv, dot_derivative);
    }
    
    FinishDotFile(dot_derivative);
    fclose(dot_derivative);
    printf("Derivative tree saved to derivative_tree.dot\n");
    
    system("dot -Tpng original_tree.dot -o original_tree.png");
    system("dot -Tpng derivative_tree.dot -o derivative_tree.png");
    
    const char* filename = "latexDUMP.tex";
    
    LatexDumpState latex_state = {};
    CtorLatexDump(&latex_state, filename);
    
    printf("Creating LaTeX file: %s\n", filename);
    
    AddLatexStep(&latex_state, "Original expression", &original_tree);
    AddLatexStep(&latex_state, "After differentiation", &derivative_tree);
    
    SimplifyUntilStable(&derivative_tree, 10, &latex_state);
    AddLatexStep(&latex_state, "After simplification", &derivative_tree);
    
    DtorLatexDump(&latex_state);
    
    TreeDtor(&original_tree, original_tree.root);
    TreeDtor(&derivative_tree, derivative_tree.root);
    
    printf("\nDONE\n");
    
    return 0;
}