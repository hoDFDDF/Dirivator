#include "ConvolutionOfConstants.h"

bool SimplificationTree(Node_t* node, ExpressionTree_t* tree, bool* changed, LatexDumpState* latex_dump){
    
    if (!node) return false;

    bool local_change = false;
    bool child_changed = false;

    if (node->left_child) {
        child_changed = SimplificationTree(node->left_child, tree, &local_change, latex_dump);
        local_change = local_change || child_changed;
    }

    if (node->right_child) {
        child_changed = SimplificationTree(node->right_child, tree, &local_change, latex_dump);
        local_change = local_change || child_changed;
    }

    Node_t* original_left = node->left_child;
    Node_t* original_right = node->right_child;
    int original_type = node->type;
    union expr_type original_value = node->tree_data;

    double num_value = CalculationNode(node);

    if (!isnan(num_value)) {
        ChangeNodeToNumber(node, num_value, tree);
        if (node->type != original_type || node->tree_data.numb != original_value.numb)
            local_change = true;
    }
    
    if (node->type == OPERATORTYPE) {
        SimplificatoinPrimeNumbers(node, tree, &local_change, latex_dump);
    }

    if (node->left_child != original_left || node->right_child != original_right ||
        node->type != original_type || !AreNodeValuesEqual(node->tree_data, original_value))
        local_change = true;

    if (changed && local_change)
        *changed = true;

    return local_change;
}

void ChangeNodeToNumber (Node_t* node, double num_value, ExpressionTree_t* tree){

    if (!node) return;

    if (node->left_child)
    {
        TreeDelete(tree, node->left_child);
        node->left_child = nullptr;
    }
    if (node->right_child)
    {
        TreeDelete(tree, node->right_child);
        node->right_child = nullptr;
    }

    node->type = NUMBERTYPE;
    node->tree_data.numb = (int)num_value;
}

double CalculationNode (Node_t* node){
    if (!node) return NAN;

    if (node->type == NUMBERTYPE)
        return (double)node->tree_data.numb;

    int op_func = get_operator_code(node->tree_data.operat);

    if (node->type == OPERATORTYPE)
    {
        
        if (op_func == SIN ||
            op_func == COS ||
            op_func == TAN ||
            op_func == EXP ||
            op_func == LN)
        {
            if (node->left_child) {
                double arg_val = CalculationNode(node->left_child);
                if (isnan(arg_val)) return NAN;
                switch (op_func) {
                    case SIN: return sin (arg_val);
                    case COS: return cos (arg_val);
                    case TAN: return tan (arg_val);
                    case LN:  return log (arg_val);
                    case EXP:
                    {
                        double arg_val = CalculationNode(node->left_child);
                        if (isnan(arg_val)) return NAN;

                        if (arg_val > 700.0) {
                            printf("WARNING: exp(%g) would overflow\n", arg_val);
                            return INFINITY;
                        }
                        return exp (arg_val);
                    }
                    default:  return NAN;
                }
            }
            return NAN;
        }
        else if (op_func == SUB && node->left_child == nullptr && node->right_child) {
            double right_val = CalculationNode(node->right_child);
            if (isnan(right_val)) return NAN;
            return -right_val;
        }
        else if (node->left_child && node->right_child) {
            double left_val  = CalculationNode(node->left_child);
            double right_val = CalculationNode(node->right_child);

            if (!isnan(left_val) && !isnan(right_val)) {
                switch (op_func) {
                    case ADD: return left_val + right_val;
                    case SUB: return left_val - right_val;
                    case MUL: return left_val * right_val;
                    case DIV:
                        if (fabs(right_val) < 1e-10) return NAN;
                        return left_val / right_val;
                    case POW: return pow(left_val, right_val);
                    default: return NAN;
                }
            }
        }
    }
    return NAN;
}

void SimplificatoinPrimeNumbers (Node_t* node, ExpressionTree_t* tree, bool* changed, LatexDumpState* latex_dump)
{
    if (!node || node->type != OPERATORTYPE) return;

    int op_func = get_operator_code(node->tree_data.operat);
    switch (op_func) {
        case ADD:
        {
            if (IsZero (node->right_child)) {
                if (latex_dump && latex_dump->file_ptr)
                    AddLatexStep(latex_dump, "simplifying: A + 0 = A", tree);

                ReplaceWithLeftChild (node);
                if (changed) *changed = true;
            }
            else if (IsZero (node->left_child)) {
                if (latex_dump && latex_dump->file_ptr)
                    AddLatexStep(latex_dump, "simplifying: 0 + A = A", tree);

                ReplaceWithRightChild (node);
                if (changed) *changed = true;
            }
            break;
        }
        case SUB:
        {
            if (node->left_child == nullptr)
                printf("!\n");

            if (IsZero (node->right_child)) {
                if (latex_dump && latex_dump->file_ptr)
                    AddLatexStep(latex_dump, "simplifying: A - 0 = A", tree);

                ReplaceWithLeftChild(node);
                if (changed) *changed = true;
            }
            else if (AreNodesEqual (node->left_child, node->right_child)) {
                if (latex_dump && latex_dump->file_ptr)
                    AddLatexStep (latex_dump, "simplifying: A - A = 0", tree);

                ChangeNodeToNumber (node, 0, tree);
                if (changed) *changed = true;
            }
            else if (IsZero (node->left_child)) {
                if (latex_dump && latex_dump->file_ptr)
                    AddLatexStep(latex_dump, "simplifying: 0 - A = -A", tree);

                Node_t* temp = node->right_child;
                node->type = OPERATORTYPE;
                int op_sub = get_operator_code(node->tree_data.operat);
                op_sub = SUB;
                node->left_child = nullptr;
                node->right_child = temp;
                if (changed) *changed = true;
            }
            break;
        }
        case MUL:
        {
            if (IsZero (node->left_child) || IsZero (node->right_child)) {
                if (latex_dump && latex_dump->file_ptr)
                    AddLatexStep(latex_dump, "simplifying: 0 * A = 0 || A * 0 = 0", tree);

                ChangeNodeToNumber (node, 0, tree);
                if (changed) *changed = true;
            }
            else if (IsOne (node->right_child)) {
                if (latex_dump && latex_dump->file_ptr)
                    AddLatexStep(latex_dump, "simplifying: A * 1 = A", tree);

                ReplaceWithLeftChild (node);
                if (changed) *changed = true;
            }
            else if (IsOne (node->left_child)) {
                if (latex_dump && latex_dump->file_ptr)
                    AddLatexStep(latex_dump, "simplifying: 1 * A = A", tree);

                ReplaceWithRightChild (node);
                if (changed) *changed = true;
            }
            break;
        }
        case DIV:
        {
            if (IsZero (node->left_child) && !IsZero (node->right_child)) {
                if (latex_dump && latex_dump->file_ptr)
                    AddLatexStep(latex_dump, "simplifying: 0 / A = 0", tree);

                ChangeNodeToNumber (node, 0, tree);
                if (changed) *changed = true;
            }
            else if (IsOne (node->right_child)) {
                if (latex_dump && latex_dump->file_ptr)
                    AddLatexStep(latex_dump, "simplifying: A / 1 = A", tree);

                ReplaceWithLeftChild (node);
                if (changed) *changed = true;
            }
            else if (AreNodesEqual (node->left_child, node->right_child)) {
                if (latex_dump && latex_dump->file_ptr)
                    AddLatexStep(latex_dump, "simplifying: A / A = 1", tree);

                ChangeNodeToNumber (node, 1, tree);
                if (changed) *changed = true;
            }
            break;
        }
        case POW:
        {
            if (IsZero (node->right_child)) {
                if (latex_dump && latex_dump->file_ptr)
                    AddLatexStep(latex_dump, "simplifying: A^0 = 1", tree);

                ChangeNodeToNumber (node, 1, tree);
                if (changed) *changed = true;
            }
            else if (IsOne (node->left_child)) {
                if (latex_dump && latex_dump->file_ptr)
                    AddLatexStep(latex_dump, "simplifying: 1^A = 1", tree);

                ChangeNodeToNumber (node, 1, tree);
                if (changed) *changed = true;
            }
            else if (IsOne (node->right_child)) {
                if (latex_dump && latex_dump->file_ptr)
                    AddLatexStep(latex_dump, "simplifying: A^1 = A", tree);

                ReplaceWithLeftChild (node);
                if (changed) *changed = true;
            }
            else if (IsZero(node->left_child) && !IsZero(node->right_child)) {
                if (latex_dump && latex_dump->file_ptr)
                    AddLatexStep(latex_dump, "simplifying: 0^A = 0", tree);

                ChangeNodeToNumber (node, 0, tree);
                if (changed) *changed = true;
            }
            break;
        }
        default:
            break;
    }
}

int IsZero(Node_t* node) {
    if (!node) return 0;

    printf("IsZero: node=%p, type=%d\n", (void*)node, node->type);
    
    if (node->type == NUMBERTYPE) {
        printf("  number value=%d\n", node->tree_data.numb);
        if (node->tree_data.numb == 0) {
            printf("  -> is ZERO (number)\n");
            return 1;
        }
    }
    
    double val = CalculationNode(node);
    printf("  calculated value=%g\n", val);
    if (!isnan(val) && fabs(val) < 1e-10) {
        printf("  -> is ZERO (calculated)\n");
        return 1;
    }
    
    printf("  -> NOT zero\n");
    return 0;
}

int IsOne(Node_t* node) {
    if (!node) return 0;

    printf("IsOne: node=%p, type=%d\n", (void*)node, node->type);
    
    if (node->type == NUMBERTYPE) {
        printf("  number value=%d\n", node->tree_data.numb);
        if (node->tree_data.numb == 1) {
            printf("  -> is ONE (number)\n");
            return 1;
        }
    }
    
    if (node->type == VARIABLETYPE) {
        printf("  variable name=%s\n", node->tree_data.varible);
        return 0;
    }
    
    double val = CalculationNode(node);
    printf("  calculated value=%g\n", val);
    if (!isnan(val) && fabs(val - 1.0) < 1e-10) {
        printf("  -> is ONE (calculated)\n");
        return 1;
    }
    
    printf("  -> NOT one\n");
    return 0;
}

int AreNodesEqual (Node_t* a, Node_t* b){
    if (!a && !b) return 1;
    if (!a || !b) return 0;

    if (a->type != b->type) return 0;

    switch (a->type) {
        case NUMBERTYPE:
            return a->tree_data.numb == b->tree_data.numb;
        case VARIABLETYPE:
            return a->tree_data.varible == b->tree_data.varible;
        case OPERATORTYPE:
            return a->tree_data.operat == b->tree_data.operat &&
                   AreNodesEqual (a->left_child, b->left_child) &&
                   AreNodesEqual (a->right_child, b->right_child);
        default:
            return 0;
    }
}

bool SimplifyUntilStable(ExpressionTree_t* tree, int max_iterations, LatexDumpState* latex_dump){
    if (!tree || !tree->root) return false;

    printf("\n=== STARTING SIMPLIFICATION LOOP ===\n");
    printf("Initial tree root type: %d\n", tree->root->type);

    bool changed = true;
    int iteration = 0;

    while (changed && iteration < max_iterations) {
        changed = false;
        
        printf("\n--- Iteration %d ---\n", iteration + 1);
        printf("Before simplification:\n");
        char before[MAX_LATEX_EXPRESSION_LENGTH] = {0};
        int pos = 0;
        NodeToLatexString(tree->root, before, &pos, sizeof(before));
        printf("  %s\n", before);

        SimplificationTree(tree->root, tree, &changed, latex_dump);

        printf("After simplification:\n");
        char after[MAX_LATEX_EXPRESSION_LENGTH] = {0};
        pos = 0;
        NodeToLatexString(tree->root, after, &pos, sizeof(after));
        printf("  %s\n", after);
        printf("Changed: %s\n", changed ? "YES" : "NO");

        iteration++;

        if (changed && latex_dump && latex_dump->file_ptr) {
            char step_desc[MAX_STR_SIZE] = {};
            snprintf(step_desc, sizeof(step_desc), "After iteration %d of simplifying", iteration);
            AddLatexStep(latex_dump, step_desc, tree);
        }
    }

    printf("=== SIMPLIFICATION COMPLETE ===\n");
    printf("Total iterations: %d\n", iteration);

    return (iteration > 0);
}

int AreNodeValuesEqual(union expr_type a, union expr_type b){
    return a.numb == b.numb &&
           a.operat == b.operat &&
           a.varible == b.varible;
}

void ReplaceWithLeftChild(Node_t* node){
    if (!node || !node->left_child) return;
    
    Node_t* left = node->left_child;
    
    node->type = left->type;
    node->tree_data = left->tree_data;
    
    node->left_child = left->left_child;
    node->right_child = left->right_child;
    
    if (node->left_child) node->left_child->parent = node;
    if (node->right_child) node->right_child->parent = node;
    
    free(left);
}

void ReplaceWithRightChild(Node_t* node){
    if (!node || !node->right_child) return;
    
    Node_t* right = node->right_child;
    
    node->type = right->type;
    node->tree_data = right->tree_data;
    
    node->left_child = right->left_child;
    node->right_child = right->right_child;
    
    if (node->left_child) node->left_child->parent = node;
    if (node->right_child) node->right_child->parent = node;
    
    free(right);
}