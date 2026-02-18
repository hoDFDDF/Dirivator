#include "ConvolutionOfConstants.h"

bool SimplificationTree (Node_t* node, ExpressionTree_t* tree, bool* changed, LatexDumpState* latex_dump) {
    
    //!SECTION(!node) return false;

    bool local_change = false;

    if (node->left_child) {
        bool left_change = SimplificationTree(node, tree, &local_change, latex_dump);
        bool local_change = local_change || left_change;

    }

       if (node->right_child) {
        bool right_change = SimplificationTree(node, tree, &local_change, latex_dump);
        bool local_change = local_change || right_change;
    }

    Node_t* original_left = node->left_child;
    Node_t* original_right = node->right_child;
    int original_type = node->type;
    union expr_type original_value = node->tree_data;

      double num_value = CalculationNode(node);

    if (!isnan(num_value))
    {
        ChangeNodeToNumber (node, num_value, tree);

        if (node->type != original_type || node->tree_data.numb != original_value.numb)
            local_change = true;
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
            if (node->left_child)
            {
                double arg_val = CalculationNode(node->left_child);
                if (isnan(arg_val)) return NAN;
                switch (op_func)
                {
                    case SIN: return sin (arg_val);
                    case COS: return cos (arg_val);
                    case TAN: return tan (arg_val);
                    case LN:  return log (arg_val);
                    case EXP:
                    {
                        double arg_val = CalculationNode(node->left_child);
                        if (isnan(arg_val)) return NAN;

                        if (arg_val > 700.0)
                        {
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
        else if (op_func == SUB && node->left_child == nullptr && node->right_child)
        {
            double right_val = CalculationNode(node->right_child);
            if (isnan(right_val)) return NAN;
            return -right_val;
        }
        else if (node->left_child && node->right_child)
        {
            double left_val  = CalculationNode(node->left_child);
            double right_val = CalculationNode(node->right_child);

            if (!isnan(left_val) && !isnan(right_val))
            {
                switch (op_func)
                {
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
    switch (op_func)
    {
        case ADD:
        {
            if (IsZero (node->right_child)) // A+0
            {
                if (latex_dump && latex_dump->file_ptr)
                    AddLatexStep(latex_dump, "simplifying: A + 0 = A", tree);

                ReplaceWithLeftChild (node);
                if (changed) *changed = true;
            }
            else if (IsZero (node->left_child)) // 0+A
            {
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
                printf("������� �����!\n");

            if (IsZero (node->right_child)) // A-0
            {
                if (latex_dump && latex_dump->file_ptr)
                    AddLatexStep(latex_dump, "simplifying: A - 0 = A", tree);

                ReplaceWithLeftChild(node);
                if (changed) *changed = true;
            }
            else if (AreNodesEqual (node->left_child, node->right_child)) // A-A
            {
                if (latex_dump && latex_dump->file_ptr)
                    AddLatexStep (latex_dump, "simplifying: A - A = 0", tree);

                ChangeNodeToNumber (node, 0, tree);
                if (changed) *changed = true;
            }
            else if (IsZero (node->left_child)) // 0-A
            {
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
            if (IsZero (node->left_child) || IsZero (node->right_child))
            {
                if (latex_dump && latex_dump->file_ptr)
                    AddLatexStep(latex_dump, "simplifying: 0 * A = 0 || A * 0 = 0", tree);

                ChangeNodeToNumber (node, 0, tree); // 0*A || A*0
                if (changed) *changed = true;
            }
            else if (IsOne (node->right_child)) // A*1
            {
                if (latex_dump && latex_dump->file_ptr)
                    AddLatexStep(latex_dump, "simplifying: A * 1 = A", tree);

                ReplaceWithLeftChild (node);
                if (changed) *changed = true;
            }
            else if (IsOne (node->left_child)) // 1*A
            {
                if (latex_dump && latex_dump->file_ptr)
                    AddLatexStep(latex_dump, "simplifying: 1 * A = A", tree);

                ReplaceWithRightChild (node);
                if (changed) *changed = true;
            }
            break;
        }
        case DIV:
        {
            if (IsZero (node->left_child) && !IsZero (node->right_child)) // 0/A
            {
                if (latex_dump && latex_dump->file_ptr)
                    AddLatexStep(latex_dump, "simplifying: 0 / A = 0", tree);

                ChangeNodeToNumber (node, 0, tree);
                if (changed) *changed = true;
            }
            else if (IsOne (node->right_child)) // A/1
            {
                if (latex_dump && latex_dump->file_ptr)
                    AddLatexStep(latex_dump, "simplifying: A / 1 = A", tree);

                ReplaceWithLeftChild (node);
                if (changed) *changed = true;
            }
            else if (AreNodesEqual (node->left_child, node->right_child)) // A/A
            {
                if (latex_dump && latex_dump->file_ptr)
                    AddLatexStep(latex_dump, "simplifying: A / A = 1", tree);

                ChangeNodeToNumber (node, 1, tree);
                if (changed) *changed = true;
            }
            break;
        }
        case POW:
        {
            if (IsZero (node->right_child)) // A^0
            {
                if (latex_dump && latex_dump->file_ptr)
                    AddLatexStep(latex_dump, "simplifying: A^0 = 1", tree);

                ChangeNodeToNumber (node, 1, tree);
                if (changed) *changed = true;
            }
            else if (IsOne (node->left_child)) // 1^A
            {
                if (latex_dump && latex_dump->file_ptr)
                    AddLatexStep(latex_dump, "simplifying: 1^A = 1", tree);

                ChangeNodeToNumber (node, 1, tree);
                if (changed) *changed = true;
            }
            else if (IsOne (node->right_child)) // A^1
            {
                if (latex_dump && latex_dump->file_ptr)
                    AddLatexStep(latex_dump, "simplifying: A^1 = A", tree);

                ReplaceWithLeftChild (node);
                if (changed) *changed = true;
            }
            else if (IsZero(node->left_child) && !IsZero(node->right_child)) // 0^A
            {
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

    if (node->type == NUMBERTYPE && node->tree_data.numb == 0) {
        return 1; 
        
        double val = CalculationNode(node);
        return !isnan(val) && fabs(val) < 1e-10;
    }
}

int IsOne(Node_t* node) {

    if (!node) return 0;

    if (node->type == NUMBERTYPE && node->tree_data.numb == 1) {
        return 1; 
        
        double val = CalculationNode(node);
        return !isnan(val) && fabs(val - 1) < 1e-10;
    }
}

int AreNodesEqual (Node_t* a, Node_t* b)
{
    if (!a && !b) return 1;
    if (!a || !b) return 0;

    if (a->type != b->type) return 0;

    switch (a->type)
    {
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

bool SimplifyUntilStable(ExpressionTree_t* tree, int max_iterations, LatexDumpState* latex_dump)
{
    if (!tree || !tree->root) return false;

    printf ("\n=== STARTING SIMPLIFICATION LOOP ===\n");

    bool changed = true;
    int iteration = 0;

    while (changed && iteration < max_iterations)
    {
        changed = false;

        SimplificationTree (tree->root, tree, &changed, latex_dump);

        iteration++;

        printf ("Simplification iteration %d: %s\n",
               iteration, changed ? "changed" : "stable");

        if (changed && latex_dump && latex_dump->file_ptr)
        {
            char step_desc[MAX_STR_SIZE] = {};
            snprintf (step_desc, sizeof(step_desc), "After iteration of simplifying %d", iteration);
            AddLatexStep (latex_dump, step_desc, tree);
        }
    }

    printf ("=== SIMPLIFICATION COMPLETE ===\n");
    printf ("Total iterations: %d\n", iteration);

    return (iteration > 1);
}

int AreNodeValuesEqual(union expr_type a, union expr_type b)
{
    return a.numb == b.numb &&
           a.operat == b.operat &&
           a.varible == b.varible;
}