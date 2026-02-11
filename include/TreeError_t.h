#ifndef _TREE_ERRORS_H_
#define _TREE_ERRORS_H_

enum TreeError_t{

    TREE_VERIFIDE                         = 0,
    TREE_ROOT_IS_NULL               =  1 << 0,
    NODE_IS_NULL                    =  1 << 1,
    TREE_SIZE_ERROR                 =  1 << 2,
    THREE_LINKING_ERROR             =  1 << 3,
    ERROR_LEFT_CHILD_IS_NOT_EMPTY   =  1 << 4,
    ERROR_RIGHT_CHILD_IS_NOT_EMPTY  =  1 << 5,
    ERROR_NODE_NULL                 =  1 << 6,
    ERROR_TREE_NULL                 =  1 << 7,
    
}; 
#endif //_TREE_ERRORS_H_