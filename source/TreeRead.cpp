#include "TreeRead.h"
#include "tree.h"


#if 0
void ReadDataFromFile(TreeFile* text_tree, FILE* tree_file_ptr, ExpressionTree_t* tree) {
    assert(text_tree != nullptr);
    assert(tree != nullptr);

    size_t buffer_size = 4096;
    char* buffer = (char*)malloc(buffer_size);
    if (!buffer) {
        printf("ERROR: Memory allocation failed\n");
        return;
    }
    
    size_t total_read = 0;
    char* line;

    while ((line = fgets(buffer + total_read, buffer_size - total_read, tree_file_ptr)) != nullptr) {
        total_read += strlen(line);
        
  
        if (total_read >= buffer_size - 100) {
            buffer_size *= 2;
            char* new_buffer = (char*)realloc(buffer, buffer_size);
            if (!new_buffer) {
                free(buffer);
                printf("ERROR: Memory reallocation failed\n");
                return;
            }
            buffer = new_buffer;
        }
    }
    
    if (total_read == 0) {
        printf("ERROR: Empty file or read error\n");
        free(buffer);
        return;
    }
    
    if (buffer[total_read - 1] == '\n') {
        buffer[total_read - 1] = '\0';
        total_read--;
    }
    
    printf("Read string (%zu chars): %s\n", total_read, buffer);
    
    if (strlen(buffer) == 0) {
        printf("ERROR: Empty string read from file\n");
        free(buffer);
        return;
    }
    
    char* str_ptr = buffer;
    printf("Starting parse at: '%s'\n", str_ptr);
    
    tree->root = NodeParse(&str_ptr);  
    
    printf("Parse completed. str_ptr now at: '%s'\n", str_ptr);
    
    if (tree->root == nullptr) {
        printf("ERROR: Failed to parse tree from file\n");
    } else {
        printf("Tree parsed successfully. Root: %s\n", tree->root->tree_data);
    }
}

void SkipSpaces(char** tree_str){
    assert(tree_str != nullptr);
    while (**tree_str && isspace(**tree_str)) {
        (*tree_str)++;
    }
}

Node_t* NodeParse(char** tree_str){
    assert(tree_str != nullptr);

    if (**tree_str == '\0') return nullptr;
    SkipSpaces(tree_str);

    if (strncmp(*tree_str, "nil", 3) == 0) {
        *(tree_str) += 3;
        return nullptr;
    }

    if (**tree_str != '(') return nullptr;

    (*tree_str)++;
    SkipSpaces(tree_str);

    size_t name_len = 0;
    char node_name[MAX_LEGHT_OF_DATA];
    SkipSpaces(tree_str);

    while (**tree_str != ')' && !isspace(**tree_str)) {
        if (name_len < MAX_LEGHT_OF_DATA - 1) {
            node_name[name_len] = **tree_str;
            name_len++;
        } else {
            return nullptr;
        }

        (*tree_str)++;
    }
    node_name[name_len] = '\0';
    
    Node_t* node = (Node_t*)(calloc(1, sizeof(Node_t)));

    node->tree_data = strdup(node_name);
    if (node->tree_data == nullptr) {
        free(node);
        return nullptr;
    }
    
    SkipSpaces(tree_str);

    if (**tree_str == ')') {
        node->left_child  = nullptr;
        node->right_child = nullptr;
        (*tree_str)++;

        return node;
    }

    node->left_child = NodeParse(tree_str);
    SkipSpaces(tree_str);

    if (**tree_str == ')') {
        (*tree_str)++;
        node->right_child = nullptr;
        return node;
    }
    
    node->right_child = NodeParse(tree_str);
    SkipSpaces(tree_str);

    if (**tree_str != ')') return nullptr;
    (*tree_str)++;

    return node;
}

#endif