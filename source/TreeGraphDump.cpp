#include "TreeGraphDump.h"

void DrawTree(ExpressionTree_t* tree, Node_t* node, FILE* dot_file){

    fprintf (dot_file,"digraph tree{\n");
    fprintf (dot_file, "    charset=\"UTF-8\";\n");

    fprintf(dot_file, "    bgcolor=\"#001f29\";\n");
    fprintf(dot_file, "     rankdir = TB;\n");  
    fprintf (dot_file, "    nodesep = 0.5;\n");
    fprintf (dot_file, "    ranksep = 0.7;\n");

    fprintf (dot_file, "    node [shape=plaintext, style=filled, fontname=\"Arial\"];\n");

    fprintf (dot_file, "    edge [fontname=\"Arial\"];\n\n");

    fprintf (dot_file, "    labelloc=\"t\";\n");
}

void CreateNode(Node_t* node, FILE* dot_file, ExpressionTree_t* tree){
    if (node == nullptr) {
        printf("CreateNode: node is null\n");
        return;
    }
    if (!tree || !dot_file) return;
    
    printf("CreateNode: node=%p, type=%d\n", (void*)node, node->type);
    
    const char* fillcolor = "";
    const char* color = "black";

    if (node == tree->root) {
        fillcolor = "#5f3035";
        color     = "#fdfdfd";
    } else if (node->left_child == nullptr || node->right_child == nullptr) {
        fillcolor = "#445c00";
        color     = "#fdfdfd";
    } else {
        fillcolor = "#2799a0";
        color     = "#fdfdfd";
    }

    char data_str[64] = "";
    
    if (node->type == NUMBERTYPE) {
        printf("  Type: NUMBER, value=%d\n", node->tree_data.numb);
        sprintf(data_str, "NUM: %d", node->tree_data.numb);
    } 
    else if (node->type == VARIABLETYPE) {
        printf("  Type: VARIABLE\n");
        if (node->tree_data.varible) {
            printf("  Var string: '%s'\n", node->tree_data.varible);
            sprintf(data_str, "VAR: %s", node->tree_data.varible);
        } else {
            printf("  Var is NULL\n");
            sprintf(data_str, "VAR: NULL");
        }
    }
    else if (node->type == OPERATORTYPE) {
        printf("  Type: OPERATOR\n");
        if (node->tree_data.operat) {
            printf("  Op string: '%s'\n", node->tree_data.operat);
            sprintf(data_str, "OP: %s", node->tree_data.operat);
        } else {
            printf("  Op is NULL\n");
            sprintf(data_str, "OP: NULL");
        }
    }
    else if (node->type == NOTYPE) {
        printf("  Type: NOTYPE\n");
        sprintf(data_str, "NO TYPE");
    }
    else {
        printf("  Type: UNKNOWN (%d)\n", node->type);
        sprintf(data_str, "UNKNOWN: %d", node->type);
    }

    fprintf(dot_file, 
        "    node%p [label=<<TABLE BORDER='1' CELLBORDER='1' CELLSPACING='0'>"
        "<TR><TD COLSPAN='2'>%p</TD></TR>"
        "<TR><TD COLSPAN='2'>%s</TD></TR>"
        "<TR><TD>%p</TD><TD>%p</TD></TR></TABLE>>, "
        "fillcolor=\"%s\", color=\"%s\", fontcolor=\"%s\"];\n",
        (void*)node, (void*)node, data_str, 
        (void*)node->left_child, (void*)node->right_child, 
        fillcolor, color, color);
    
    printf("  Written to dot\n");
}

void MakeArrow(Node_t* node, ExpressionTree_t* tree, FILE* dot_file){
    if (node == nullptr) return;
    
    if (node->left_child != nullptr) {
        fprintf(dot_file, 
            "    node%p -> node%p [color=\"pink\", penwidth=2, "
            "fontcolor=\"pink\", fontsize=12, arrowsize=0.8];\n",  
            (void*)node, (void*)node->left_child);
    }

    if (node->right_child != nullptr) {
        fprintf(dot_file, 
            "    node%p -> node%p [color=\"pink\", penwidth=2, "
            "fontcolor=\"pink\", fontsize=12, arrowsize=0.8];\n",  
            (void*)node, (void*)node->right_child);
    }
    
}

void SetRankToNodes(ExpressionTree_t* tree, Node_Info* progress, FILE* dot_file){
    printf("SetRankToNodes start\n");
    
    if (!progress) {
        printf("progress null\n");
        return;
    }
    if (!tree) {
        printf("tree null\n");
        return;
    }
    if (!dot_file) {
        printf("dot_file null\n");
        return;
    }
    
    printf("Progress size: %zu\n", progress->size);
    
    for (size_t i = 0; i < progress->size; i++) {
        printf("Node %zu\n", i);
        Node_t* node = progress->nodes_array[i].node;
        if (node) {
            printf("  Addr: %p\n", (void*)node);
            CreateNode(node, dot_file, tree);
        } else {
            printf("  Null\n");
        }
    }
    
    printf("Arrows\n");
    for (size_t i = 0; i < progress->size; i++) {
        Node_t* node = progress->nodes_array[i].node;
        if (node) {
            MakeArrow(node, tree, dot_file);
        }
    }
    
    printf("SetRankToNodes end\n");
}
void FinishDotFile(FILE* dot_file){
    fprintf(dot_file, "}\n");
}

void ProgressTreeInit(Node_Info* progress, ExpressionTree_t* tree){
    if (progress == nullptr || tree == nullptr) return;
    
    progress->capacity = 32;
    progress->nodes_array = (LoadNodeProgress*)calloc(progress->capacity, sizeof(LoadNodeProgress));
    progress->size = 0;
    RecursiveFillNodeInfoProgress(progress, tree->root, 0);

}

void RecursiveFillNodeInfoProgress(Node_Info* progress, Node_t* node, size_t rank){
    if (progress == nullptr ||  node == nullptr) return;

    progress->nodes_array[progress->size].node = node;
    progress->nodes_array[progress->size].rank = rank;
    progress->size++;

    RecursiveFillNodeInfoProgress(progress, node->left_child, rank + 1);
    RecursiveFillNodeInfoProgress(progress, node->right_child, rank + 1);
}