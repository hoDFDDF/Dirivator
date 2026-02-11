#ifndef _GET_STRING_PARAM_
#define _GET_STRING_PARAM_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "tree.h"

const size_t MAX_LEGHT_OF_DATA         = 4096;
const size_t MAX_LENGHT_OF_TEMP_BUFFER = 4096;

struct TreeFile {
    char** buffer;
    size_t buff_size_in_bytes;
    size_t buff_lenght;
};

void ReadDataFromFile(TreeFile* text_tree, FILE* tree_file_ptr, ExpressionTree_t* tree);
Node_t* NodeParse(char** tree_str);
void SkipSpaces(char** tree_str);

#endif //_GET_STRING_PARAM_