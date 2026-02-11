#include "tree.h"

void TreeCtor(ExpressionTree_t* tree){
    assert(tree != nullptr);
    tree->root = (Node_t*)calloc(1, sizeof(Node_t));
    tree->root->left_child = tree->root->right_child = nullptr;
    tree->size = 0;
}

Node_t* NodeCtor(expr_type data, ExpressionTree_t* tree, int type){

   Node_t* new_node = (Node_t*)calloc(1, sizeof(Node_t));

   new_node->tree_data = data;

   new_node->left_child = new_node->right_child = new_node->parent =  nullptr;
    
   return new_node;
}

Node_t* NodeDtor(){ 
    
}

TreeError_t TreeInsert(ExpressionTree_t* tree, Node_t* node, expr_type data, Child child){ 
    printf("=== TreeInsert CALL ===\n");
    printf("  tree->size: %zu\n", tree->size);
    printf("  node: %p\n", (void*)node);
    printf("  data: %s\n", data);
    printf("  child: %s\n", (child == LEFT_CHILD) ? "LEFT" : "RIGHT");
    
    if (tree->size == 0) {
        printf("  Creating ROOT node\n");
        if (node != nullptr) {
            printf("  Using provided node as root: %p\n", (void*)node);
            tree->root = node;
            node->tree_data = data;
            node->right_child = node->left_child = nullptr;
        } else {
            tree->root = NodeCtor(data, tree, NOTYPE);
            printf("  Created new root node: %p\n", (void*)tree->root);
        }
        tree->size++;
        printf("  Root created successfully. New size: %zu\n", tree->size);
        return TREE_VERIFIDE;
    }

    printf("  Creating CHILD node\n");
    Node_t* new_node = NodeCtor(data, tree, NOTYPE);
    printf("  New node created: %p\n", (void*)new_node);
    
    if (node == nullptr) {
        printf("  ERROR: node is NULL\n");
        return ERROR_NODE_NULL;
    }

    printf("  Parent node: %p\n", (void*)node);
    printf("  Parent left: %p, right: %p\n", 
           (void*)node->left_child, (void*)node->right_child);

    if (child == LEFT_CHILD) {
        if (node->left_child != nullptr) {
            printf("  ERROR: Left child already exists: %p\n", (void*)node->left_child);
            return ERROR_LEFT_CHILD_IS_NOT_EMPTY;
        }
        printf("  Adding as LEFT child\n");
        new_node->parent = node;
        node->left_child = new_node;
    }

    if (child == RIGHT_CHILD) {
        if (node->right_child != nullptr) {
            printf("  ERROR: Right child already exists: %p\n", (void*)node->right_child);
            return ERROR_RIGHT_CHILD_IS_NOT_EMPTY;
        }
        printf("  Adding as RIGHT child\n");
        new_node->parent = node;
        node->right_child = new_node;
    }
    
    tree->size++;
    printf("  Child added successfully. New size: %zu\n", tree->size);
    printf("=== TreeInsert END ===\n\n");
    
    return TREE_VERIFIDE;
}

TreeError_t TreeDelete(ExpressionTree_t* tree, Node_t* node){

    if (tree == nullptr) return ERROR_TREE_NULL;
    if (node == nullptr) return ERROR_NODE_NULL;

    TreeDelete(tree, node->left_child);
    TreeDelete(tree, node->right_child);

    node->left_child  = nullptr;
    node->right_child = nullptr;
    node->parent      = nullptr;

    free(node);

    return TREE_VERIFIDE;
}

size_t MaxDepthOfBInaryTree(Node_t* tree_descent){
    if (tree_descent == nullptr) return 0;
    
    size_t left_max_depth = MaxDepthOfBInaryTree(tree_descent->left_child);
    size_t right_max_depth = MaxDepthOfBInaryTree(tree_descent->right_child);

    return max(left_max_depth, right_max_depth) + 1;
}

Node_t* DeleteSubTree(Node_t* node){

    if (node == nullptr) return nullptr;
    
    DeleteSubTree(node->left_child);
    DeleteSubTree(node->right_child);
    free(node);
}

Node_t* CreateOperatorNode(ExpressionTree_t* tree, char* op_code, Node_t* node_left, Node_t* node_right){
    union expr_type value;
    value.operat = op_code;
    Node_t* new_node = NodeCtor(value, tree, OPERATORTYPE);
    
    if (new_node) {
        new_node->left_child  =  node_left;
        new_node->right_child = node_right; 
    }
 
    return new_node;
}

Node_t* CreateVariableNode(ExpressionTree_t* tree, char* var_code){
    union expr_type value;
    value.varible = var_code;
    return NodeCtor(value, tree, VARIABLETYPE);
} 

Node_t* CreateNumberNode(ExpressionTree_t* tree, int numb_code){
    union expr_type value;
    value.numb = numb_code;
    return NodeCtor(value, tree, NUMBERTYPE);
}

#if 0
TreeError_t TreeVerify(Tree* tree, Node_t* node){
    if (tree == nullptr) return ERROR_TREE_NULL;
    if (node == nullptr) return ERROR_NODE_NULL;

    TreeError_t status = TREE_VERIFIDE;

    if (tree->  == NULL)                            status |= LIST_DATA_NULL;
    if (list->current_numb_of_nodes > list->capacity)  status |= LIST_ERROR_OVERFLOW;
    if (list->head > list->capacity)                   status |= LIST_INVALID_INDEX;
    if (list->tail > list->capacity)                   status |= LIST_INVALID_INDEX;
    if (list->free_cell > list->capacity)              status |= LIST_INVALID_INDEX;
    
    int node_count = 0;
    int current = list->head;
    
    while (current != 0 && current <= (int)list->capacity && node_count <= (int)list->capacity) {
        node_count++;
        
        if (list->prev[current] < -1 || list->prev[current] > (int)list->capacity) {
            status |= LIST_INVALID_INDEX;
        }
        
        if (list->next[current] < 0 || list->next[current] > (int)list->capacity) {
            status |= LIST_INVALID_INDEX;
        }
        
        current = list->next[current];
    }
    
    if (node_count != list->current_numb_of_nodes) {
        status |= LIST_ERROR_SIZE;
    }
    
    return status;
}
#endif

