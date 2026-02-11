#include <stdio.h>
#include <stdlib.h>

#include "RecursiveDescentReadTree.h"
#include "TreeGraphDump.h"

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
    
    printf("Tree root: %p\n", (void*)root);
    
    FILE* dot_file = fopen("tree.dot", "w");
    if (!dot_file) {
        printf("ERROR: Cannot create tree.dot\n");
       
        return 1;
    }
    
    printf("File tree.dot opened successfully\n");
    
    DrawTree(&tree, tree.root, dot_file);
    printf("DrawTree called\n");
    
    Node_Info progress = {};
    ProgressTreeInit(&progress, &tree);
    printf("Progress size: %zu\n", progress.size);
    
    SetRankToNodes(&tree, &progress, dot_file);
    printf("SetRankToNodes called\n");
    
    FinishDotFile(dot_file);
    printf("FinishDotFile called\n");
    
    fclose(dot_file);
    printf("File closed\n");
    
    dot_file = fopen("tree.dot", "r");
    if (dot_file) {
        fseek(dot_file, 0, SEEK_END);
        long size = ftell(dot_file);
        fclose(dot_file);
        printf("tree.dot size: %ld bytes\n", size);
        
        if (size > 0) {
            system("dot -Tpng tree.dot -o tree.png 2>nul");
            printf("PNG generated: tree.png\n");
        }
    }
    
    return 0;
}