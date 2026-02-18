#include "differentiator.h"

char* OperatorToString(int op) {
    switch(op) {
        case ADD: return "+";
        case SUB: return "-";
        case MUL: return "*";
        case DIV: return "/";
        case POW: return "^";
        case SIN: return "sin";
        case COS: return "cos";
        case TAN: return "tan";
        case LN: return "ln";
        case EXP: return "exp";
        default: return "";
    }
}

int get_operator_code(char* op_str) {
    if (strcmp(op_str, "+") == 0) return ADD;
    if (strcmp(op_str, "-") == 0) return SUB;
    if (strcmp(op_str, "*") == 0) return MUL;
    if (strcmp(op_str, "/") == 0) return DIV;
    if (strcmp(op_str, "^") == 0) return POW;
    if (strcmp(op_str, "sin") == 0) return SIN;
    if (strcmp(op_str, "cos") == 0) return COS;
    if (strcmp(op_str, "tan") == 0) return TAN;
    if (strcmp(op_str, "ln") == 0) return LN;
    if (strcmp(op_str, "exp") == 0) return EXP;
    return 0;
}

Node_t* CreateOperatorNodeByEnum(ExpressionTree_t* tree, int op_code, Node_t* node_left, Node_t* node_right){
    char* op_str = OperatorToString(op_code);
    return CreateOperatorNode(tree, op_str, node_left, node_right);
}

Node_t* CopySubtree (ExpressionTree_t* tree, Node_t* node)
{
    assert (tree);

    if (!node) return nullptr;

    Node_t* new_node = NodeCtor (node->tree_data, tree, node->type);

    new_node->left_child =  CopySubtree (tree, node->left_child);
    new_node->right_child = CopySubtree (tree, node->right_child);

    return new_node;
}

Node_t* DifferentiateNode (ExpressionTree_t* tree, Node_t* node, char variable)
{
    assert(tree);

    if (!node) {
        printf("DifferentiateNode: node is NULL\n");
        return nullptr;
    }

    printf("DifferentiateNode: node=%p, type=%d\n", (void*)node, node->type);

    switch (node->type)
    {
        case NUMBERTYPE:
        {
            printf("  Number: %d -> derivative = 0\n", node->tree_data.numb);
            expr_type data = {};
            data.numb = 0;
            return NodeCtor(data, tree, NUMBERTYPE);
        }
        case VARIABLETYPE:
        {
            char node_var_name = node->tree_data.varible[0];
            printf("  Variable: %c", node_var_name);
            
            if (node_var_name == variable)
            {
                printf(" (same as %c) -> derivative = 1\n", variable);
                expr_type data = {};
                data.numb = 1;
                return NodeCtor(data, tree, NUMBERTYPE);
            }
            else
            {
                printf(" (different from %c) -> derivative = 0\n", variable);
                expr_type data = {};
                data.numb = 0;
                return NodeCtor(data, tree, NUMBERTYPE);
            }
        }
        case OPERATORTYPE:
        {
            printf("  Operator: '%s'\n", node->tree_data.operat);
            return DifferentiateOperator(tree, node, variable);
        }
        default:
            printf("  Unknown type: %d\n", node->type);
            return NULL;
    }
}

Node_t* DifferentiateOperator (ExpressionTree_t* tree, Node_t* node, char variable)
{
    assert (tree);

    Node_t* result = NULL;

    int op_code = get_operator_code(node->tree_data.operat);

    switch (op_code)
    {
        case ADD:
        {
            result = ADD_(dL, dR);
            break;
        }
        case SUB:
        {
            result = SUB_(dL, dR);
            break;
        }
        case MUL:
        {
            if (node->left_child->type == NUMBERTYPE)
                return MUL_(cL, dR);
            else if (node->right_child->type == NUMBERTYPE)
                return MUL_(dL, cR);
            else
                return ADD_(MUL_(dL, cR), MUL_(cL, dR));
            break;
        }
        case DIV:
        {
            result = DIV_(SUB_(MUL_(dL, cR), MUL_(cL, dR)), POW_(cR, NUM_(2)));
            break;
        }
        case POW:
        {
            if (node->right_child->type == NUMBERTYPE)
            {
               int power = node->right_child->tree_data.numb;
               result = MUL_(
                    MUL_(
                        NUM_(power),              // n
                        POW_(cL, NUM_(power - 1)) // f^(n-1)
                    ), 
                    dL                                // f'
                );
            }
            else if (node->left_child->type == NUMBERTYPE)
            {
                result = MUL_(MUL_(POW_(cL, cR), LN_(cL)), dR);
            }
            else
            {
                Node_t* y = POW_(cL, cR);
                Node_t* term1 = MUL_(dR, LN_(cL));
                Node_t* term2 = MUL_(cR, DIV_(dL, cL));
                result = MUL_(y, ADD_(term1, term2));
            }
            break;
        }
        case SIN:
        {
            result = MUL_(COS_(cL), dL);
            break;
        }
        case COS:
        {
            result = MUL_(MUL_(NUM_(-1), SIN_(cL)), dL);
            break;
        }
        case TAN:
        {
            result = MUL_(DIV_(NUM_(1), POW_(COS_(cL), NUM_(2))), dL);
            break;
        }
        case LN:
        {
            result = DIV_( dL, cL);
            break;
        }
        case EXP:
        {
            result = MUL_(EXP_(cL), dL);
            break;
        }
        default:
            result = NULL;
    }

    return result;
}


Node_t* NewNode (ExpressionTree_t* tree, int type, expr_type value, Node_t* left, Node_t* right)
{
    assert (tree);

    Node_t* new_node = NodeCtor (value, tree, type);
    new_node->left_child = left;
    new_node->right_child = right;
    return new_node;
}