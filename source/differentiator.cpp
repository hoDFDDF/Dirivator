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
    assert (tree);

    if (!node) return nullptr;

    switch (node->type)
    {
        case NUMBERTYPE:
        {
            expr_type data = {};
            data.numb = 0;
            return NodeCtor (data, tree, NUMBERTYPE);
        }
        case VARIABLETYPE:
        {
            char node_var_name = node->tree_data.varible[0];
            
            if (node_var_name == variable)
            {
                expr_type data = {};
                data.numb = 1;
                return NodeCtor (data, tree, NUMBERTYPE);
            }
            else
            {
                expr_type data = {};
                data.numb = 0;
                return NodeCtor (data, tree, NUMBERTYPE);
            }
        }
        case OPERATORTYPE:
        {
            return DifferentiateOperator (tree, node, variable);
        }
        default:
            return NULL;
    }
}

Node_t* DifferentiateOperator (Tree_t* tree, Node_t* node, char variable)
{
    assert (tree);

    Node_t* result = NULL;

    switch (node->value.operator_type)
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
            if (node->left->type == NUMBERTYPE)
                return MUL_(cL, dR);
            else if (node->right->type == NUMBERTYPE)
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
            if (node->right->type == NUMBERTYPE)
            {
                result = MUL_(MUL_(NUM_(node->right->value.number), POW_(cL, NUM_(node->right->value.number - 1))), dL);
            }
            else if (node->left->type == NUMBERTYPE)
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