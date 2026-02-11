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
    if (node == nullptr) return;
    assert(tree != nullptr);
    
    const char* fillcolor =  "";
    const char* color = "black";

    if (node == tree->root) {

        fillcolor = brown;
        color     = white;

    } else if (node->left_child == nullptr || node->right_child == nullptr) {

        fillcolor = swamp;
        color     = white;

    } else {

        fillcolor = turquoise;
        color     =  white;
    }

    fprintf (dot_file, "    node%p [label=<<TABLE BORDER='1' CELLBORDER='1' CELLSPACING='0'>"
            "<TR><TD COLSPAN='2'>add: %p</TD></TR>"
            "<TR><TD COLSPAN='2'>data: %s</TD></TR>"
            "<TR><TD>LEFT</TD><TD>RIGHT</TD></TR>"
            "<TR><TD>%p</TD><TD>%p</TD></TR></TABLE>>, "
            "fillcolor=\"%s\", color=\"%s\", fontcolor=\"%s\"];\n",
            (void*)node, (void*)node, node->tree_data, (void*)node->left_child, (void*)node->right_child, fillcolor, color, color);
    
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
    
    assert(progress);
    assert(tree);
    assert(dot_file);

    //TreeCreation
    for (size_t curr_node = 0; curr_node < progress->size; curr_node++) {
        Node_t* new_node = progress->nodes_array[curr_node].node;
        size_t rank = progress->nodes_array[curr_node].rank;
        
        if (new_node == nullptr) continue;

        CreateNode(new_node, dot_file, tree);
    }

    size_t max_depth = 0;
    //fprintf(stdout, "progress size = %zu\n", progress->size);
    for (size_t i = 0; i < progress->size; i++) {
        fprintf(stdout, "progress size = %zu\n", progress->size);
        if (progress->nodes_array[i].node != nullptr) {
            size_t curr_rank = progress->nodes_array[i].rank;
                if (curr_rank > max_depth) {
                    max_depth = curr_rank;
                }
        }
    }
    
    for (size_t node_rank = 0; node_rank <= max_depth; node_rank++) {
        fprintf(dot_file, " { rank = same;");
        fprintf(stdout, "max_depth = %zu", max_depth);
        for (size_t rank_width = 0; rank_width < progress->size; rank_width++) {
            fprintf(stdout, "I'm here");
            if (progress->nodes_array[rank_width].rank == node_rank) {
                fprintf(dot_file, "node%p", progress->nodes_array[rank_width].node);
            }
        }
        fprintf(dot_file, "}");
    }

    for (size_t curr_node = 0; curr_node < progress->size; curr_node++) {
        Node_t* node = progress->nodes_array[curr_node].node;
        
        if (node == nullptr) continue;

        MakeArrow(node, tree, dot_file);
    }    
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