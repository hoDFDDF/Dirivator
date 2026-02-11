#include "RecursiveDescentReadTree.h"

void ReadDataFromFile(TreeFile *text_tree, FILE *tree_file_ptr, ExpressionTree_t *tree, MathExpression *math_exp)
{
    assert(text_tree != nullptr);
    assert(tree != nullptr);

    size_t buffer_size = 4096;
    char *buffer = (char *)calloc(1, buffer_size);
    if (!buffer)
    {
        printf("ERROR: Memory allocation failed\n");
        return;
    }

    size_t total_read = 0;
    char *line;

    while ((line = fgets(buffer + total_read, buffer_size - total_read, tree_file_ptr)) != nullptr) {
        total_read += strlen(line);
        if (total_read >= buffer_size - 100) {
            buffer_size *= 2;
            char *new_buffer = (char *)realloc(buffer, buffer_size);
            if (!new_buffer) {
                free(buffer);
                printf("ERROR: Memory reallocation failed\n");
                return;
            }
            buffer = new_buffer;
        }
    }

    if (total_read == 0)
    {
        printf("ERROR: Empty file or read error\n");
        free(buffer);
        return;
    }

    if (buffer[total_read - 1] == '\n')
    {
        buffer[total_read - 1] = '\0';
        total_read--;
    }

    printf("Read string (%zu chars): %s\n", total_read, buffer);

    if (strlen(buffer) == 0)
    {
        printf("ERROR: Empty string read from file\n");
        free(buffer);
        return;
    }

    char *str_ptr = buffer;
    printf("Starting parse at: '%s'\n", str_ptr);

    tree->root = GetG(math_exp, &str_ptr);

    printf("Parse completed. str_ptr now at: '%s'\n", str_ptr);

    if (tree->root == nullptr)
    {
        printf("ERROR: Failed to parse tree from file\n");
    }
    else
    {
        printf("Tree parsed successfully. Root: %s\n", tree->root->tree_data);
    }
}

void SkipSpaces(char **exp)
{
    assert(exp != nullptr);
    while (**exp && isspace(**exp))
    {
        (*exp)++;
    }
}

Node_t *GetN(MathExpression *math_exp, char **exp_from_file) {
    assert(math_exp != nullptr);

    if (!('0' <= **exp_from_file && **exp_from_file <= '9'))
        return nullptr;

    double value = 0;
    while ('0' <= **exp_from_file && **exp_from_file <= '9')
    {
        value = value * 10 + (**exp_from_file - '0');
        (*exp_from_file)++;
    }

    return CreateNumberNode(math_exp->tree, value);
}

Node_t *GetT(MathExpression *math_exp, char **exp_from_file)
{
    Node_t *val = GetP(math_exp, exp_from_file);

    while (**exp_from_file == '*' || **exp_from_file == '/')
    {
        char op = **exp_from_file;
        (*exp_from_file)++;
        Node_t *right = GetP(math_exp, exp_from_file);
        val = CreateOperatorNode(math_exp->tree, &op, val, right);
    }
    return val;
}

Node_t *GetE(MathExpression *math_exp, char **exp_from_file)
{
    Node_t *val_left = GetT(math_exp, exp_from_file);
   
    while (**exp_from_file == '+' || **exp_from_file == '-')
    {
        char op = **exp_from_file;
        (*exp_from_file)++;
        Node_t *val_right = GetT(math_exp, exp_from_file);

        if (op == '+' || op == '-')
        {
            val_left = CreateOperatorNode(math_exp->tree, &op, val_left, val_right);
        }
    }
    return val_left;
}

Node_t *GetP(MathExpression *math_exp, char **exp_from_file)
{
    assert(math_exp != nullptr);
    assert(exp_from_file != nullptr);

    if (**exp_from_file == '(')
    {
        (*exp_from_file)++;
        Node_t *val = GetE(math_exp, exp_from_file);

        if (**exp_from_file == ')')
        {
            (*exp_from_file)++;
        }

        return val;
    }
    else if (isdigit(**exp_from_file)) {
        return GetN(math_exp, exp_from_file);
    } else if (isalpha(**exp_from_file)) {
        return GetV(math_exp, exp_from_file);
    }
    
}

Node_t* GetA(MathExpression *math_exp, char **exp_from_file) { 
    printf("=== GetA START ===\n");
    printf("String: '%s'\n", *exp_from_file);
    
    SkipSpaces(exp_from_file);
    printf("After SkipSpaces: '%s'\n", *exp_from_file);
    
    Node_t* left_var = GetV(math_exp, exp_from_file);
    printf("GetV returned: %p\n", (void*)left_var);
    
    if (!left_var) {
        printf("GetA: Failed to read left variable\n");
        return nullptr;
    }
    
    SkipSpaces(exp_from_file);
    printf("Before '=' check: '%s'\n", *exp_from_file);
    
    if (**exp_from_file != '=') {
        printf("GetA: Expected '=', got '%c' (ASCII %d)\n", 
               **exp_from_file, **exp_from_file);
        return nullptr;
    }
    
    printf("Found '='!\n");
    
    (*exp_from_file)++;
    SkipSpaces(exp_from_file);
    printf("After '=': '%s'\n", *exp_from_file);
    
    Node_t* right_expr = GetE(math_exp, exp_from_file);
    printf("GetE returned: %p\n", (void*)right_expr);
    
    if (!right_expr) {
        printf("GetA: Failed to read right expression\n");
        return nullptr;
    }
    
    char op = '=';
    printf("Creating operator node for '='\n");
    Node_t* assign_node = CreateOperatorNode(math_exp->tree, &op, left_var, right_expr);
    
    printf("=== GetA SUCCESS ===\n");
    return assign_node;
}

Node_t *GetV(MathExpression *math_exp, char **exp_from_file)
{
    printf("=== GetV START ===\n");
    printf("String: '%s'\n", *exp_from_file);
    
    if (!isalpha(**exp_from_file)) {
        printf("GetV: Not a letter, got '%c' (ASCII %d)\n", 
               **exp_from_file, **exp_from_file);
        return nullptr;
    }

    char var_buffer[VAR_MAX_SIZE];
    size_t var_len = 0;
    
    printf("Reading variable...\n");
    while (isalpha(**exp_from_file) && var_len < VAR_MAX_SIZE - 1) {
        printf("  Char '%c' at pos %zu\n", **exp_from_file, var_len);
        var_buffer[var_len] = **exp_from_file;
        var_len++;
        (*exp_from_file)++;
    }
    var_buffer[var_len] = '\0';
    
    printf("Variable name: '%s'\n", var_buffer);
    printf("After reading: '%s'\n", *exp_from_file);
    
    Node_t* var = CreateVariableNode(math_exp->tree, var_buffer);
    printf("CreateVariableNode returned: %p\n", (void*)var);
    
    printf("=== GetV END ===\n");
    return var;
}


#if 0
Node_t* GetF(MathExpression* math_exp , char** exp_from_file){ 
    char* equation = *exp_from_file;
    aval_funcs operator = UNNOWNFUNC;
    
    switch(equation) {
        case sin:
            operator = SIN;
        case cos:
            operator = COS;
        case exp:
            operator = EXP
    }
    Node_t* operat = CreateOperatorNode();//TODO write this
}
#endif

Node_t *GetG(MathExpression *math_exp, char **exp_from_file){
    printf("=== GetG START ===\n");
    printf("Input string: '%s'\n", *exp_from_file);
    
    SkipSpaces(exp_from_file);
    
    printf("Trying GetA...\n");
    Node_t *tree_nodes = GetA(math_exp, exp_from_file);
    
    if (tree_nodes) {
        printf("GetA succeeded!\n");
    } else {
        printf("GetA failed, trying GetE...\n");
        tree_nodes = GetE(math_exp, exp_from_file);
    }
    
    SkipSpaces(exp_from_file);
    printf("After parsing, remaining: '%s'\n", *exp_from_file);
    
    if (**exp_from_file != '$' && **exp_from_file != '\0') {
        printf("ERROR: Expected end ($ or \\0), got '%c' (ASCII %d)\n", 
               **exp_from_file, **exp_from_file);
        DeleteSubTree(tree_nodes);
        return nullptr;
    }
    
    printf("=== GetG SUCCESS ===\n");
    return tree_nodes;
}
