#include <stdio.h>
#include <stdlib.h>

#include "RecursiveDescentReadTree.h"
#include "TreeGraphDump.h"

int main() {
    FILE* fp = fopen("Exception.txt", "r");
    if (fp == NULL) {
        printf("Error: Cannot open file\n");
        return 1;
    }
    
    char buffer[256];
    if (fgets(buffer, sizeof(buffer), fp) == NULL) {
        printf("Error: Cannot read from file\n");
        fclose(fp);
        return 1;
    }
    
    buffer[strcspn(buffer, "\n")] = '\0';
    
    printf("\n========================================\n");
    printf("PARSING EXPRESSION: '%s'\n", buffer);
    printf("========================================\n\n");
    
    ExpressionTree_t tree = {};
    TreeCtor(&tree);
    
    MathExpression math_exp = {&tree, nullptr, nullptr};
    
    char* str_ptr = buffer;
    Node_t* root = GetG(&math_exp, &str_ptr);
    
    printf("\n========================================\n");
    if (root == nullptr) {
        printf("PARSER FAILED!\n");
        printf("Remaining string: '%s'\n", str_ptr);
        printf("Tree root: %p\n", (void*)tree.root);
    } else {
        printf("PARSER SUCCESS!\n");
        printf("Root node: %p\n", (void*)root);
        tree.root = root;
        
        Node_Info progress = {};
        ProgressTreeInit(&progress, &tree);
        
        FILE* dot_file = fopen("tree.dot", "w");
        if (dot_file) {
            DrawTree(&tree, tree.root, dot_file);  
            SetRankToNodes(&tree, &progress, dot_file); 
            FinishDotFile(dot_file);
            fclose(dot_file);
            printf("Tree saved to tree.dot\n");
            
            system("dot -Tpng tree.dot -o tree.png 2>nul");
            if (system("dot -V >nul 2>&1") == 0) {
                printf("PNG saved to tree.png\n");
            }
        }
    }
    printf("========================================\n");
    
    fclose(fp); 
    return 0;
}