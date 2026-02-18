#include <stdio.h>
#include <stdlib.h>

#include "RecursiveDescentReadTree.h"
#include "TreeGraphDump.h"
#include "tree.h"

int main() {
    FILE* fp = fopen("Exception.txt", "r");
    if (!fp) {
        printf("Cannot open Exception.txt\n");
        return 1;
    }
    
    char buffer[256];
    if (!fgets(buffer, sizeof(buffer), fp)) {
        printf("Cannot read from file\n");
        fclose(fp);
        return 1;
    }
    fclose(fp);
    
    buffer[strcspn(buffer, "\n")] = '\0';
    
    printf("Expression: %s\n", buffer);
    
    ExpressionTree_t tree = {};
    TreeCtor(&tree);
    
    MathExpression math_exp = {&tree, 0, 0};
    
    char* str_ptr = buffer;
    Node_t* root = GetG(&math_exp, &str_ptr);
    
    if (!root) {
        printf("Parser failed\n");
        return 1;
    }
    
    tree.root = root;
    printf("Original tree built successfully\n");

    char variable = 'x';
    printf("Differentiating with respect to: %c\n", variable);
    
    Node_t* differ_exp = DifferentiateNode(&tree, tree.root, variable);
    
    if (!differ_exp) {
        printf("ERROR: Differentiation failed!\n");
        TreeDtor(&tree, tree.root);
        return 1;
    }
    
    printf("Derivative tree root: %p\n", (void*)differ_exp);
    
    ExpressionTree_t derivated_tree = {};
    TreeCtor(&derivated_tree);
    derivated_tree.root = differ_exp;
    
    
    if (!derivated_tree.root) {
        printf("ERROR: Derivative tree has no root!\n");
        TreeDtor(&tree, tree.root);
        TreeDtor(&derivated_tree, derivated_tree.root);
        return 1;
    }
    
    printf("Derivative tree has root: %p\n", (void*)derivated_tree.root);
    printf("Derivative tree root type: %d\n", derivated_tree.root->type);
    

    FILE* dot_original = fopen("original_tree.dot", "w");
    if (!dot_original) {
        printf("ERROR: Cannot create original_tree.dot\n");
        TreeDtor(&tree, tree.root);
        TreeDtor(&derivated_tree, derivated_tree.root);
        return 1;
    }
    
    printf("\n=== Processing original tree ===\n");
    DrawTree(&tree, tree.root, dot_original);
    
    Node_Info progress_orig = {};
    ProgressTreeInit(&progress_orig, &tree);
    printf("Original tree nodes count: %zu\n", progress_orig.size);
    
    if (progress_orig.size == 0) {
        printf("ERROR: No nodes in original tree!\n");
    } else {
        SetRankToNodes(&tree, &progress_orig, dot_original);
    }
    
    FinishDotFile(dot_original);
    fclose(dot_original);
    printf("Original tree saved to original_tree.dot\n");

    FILE* dot_derivative = fopen("derivative_tree.dot", "w");
    if (!dot_derivative) {
        printf("ERROR: Cannot create derivative_tree.dot\n");
        TreeDtor(&tree, tree.root);
        TreeDtor(&derivated_tree, derivated_tree.root);
        return 1;
    }
    
    printf("\n=== Processing derivative tree ===\n");
    DrawTree(&derivated_tree, derivated_tree.root, dot_derivative);
    
    Node_Info progress_deriv = {};
    ProgressTreeInit(&progress_deriv, &derivated_tree);
    printf("Derivative tree nodes count: %zu\n", progress_deriv.size);
    
    if (progress_deriv.size == 0) {
        printf("ERROR: No nodes in derivative tree!\n");
        printf("This means differentiation returned an empty tree\n");
    } else {
        SetRankToNodes(&derivated_tree, &progress_deriv, dot_derivative);
    }
    
    FinishDotFile(dot_derivative);
    fclose(dot_derivative);
    printf("Derivative tree saved to derivative_tree.dot\n");
    
    printf("\n=== Checking DOT files ===\n");
    
    FILE* check_orig = fopen("original_tree.dot", "r");
    if (check_orig) {
        fseek(check_orig, 0, SEEK_END);
        long size = ftell(check_orig);
        fclose(check_orig);
        printf("original_tree.dot size: %ld bytes\n", size);
    }
    
    FILE* check_deriv = fopen("derivative_tree.dot", "r");
    if (check_deriv) {
        fseek(check_deriv, 0, SEEK_END);
        long size = ftell(check_deriv);
        fclose(check_deriv);
        printf("derivative_tree.dot size: %ld bytes\n", size);
    }
    
    printf("\n=== Generating PNG files ===\n");
    
    int result1 = system("dot -Tpng original_tree.dot -o original_tree.png");
    if (result1 == 0) {
        printf("Generated: original_tree.png\n");
    } else {
        printf("Failed to generate original_tree.png\n");
    }
    
    int result2 = system("dot -Tpng derivative_tree.dot -o derivative_tree.png");
    if (result2 == 0) {
        printf("Generated: derivative_tree.png\n");
    } else {
        printf("Failed to generate derivative_tree.png\n");
    }
    
    FILE* derivated_exeption_file = fopen("result.txt", "w");

    PrintTreeToTXT(derivated_tree.root, derivated_exeption_file);

    TreeDtor(&tree, tree.root);
    TreeDtor(&derivated_tree, derivated_tree.root);
    fclose(derivated_exeption_file);
       
    printf("\n=== DONE ===\n");
    
    return 0;
}