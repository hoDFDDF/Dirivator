#include "latexDump.h"

#include "latexDump.h"
#include <string.h>
#include <math.h>
#include <stdlib.h>

const char* funny_phrases[] = {
    "Очевидно, что:",
    "Каждому ежику понятно:",
    "Даже Бязит знает что:",
    "После недолгих размышлений:",
    "Как говорил Архимед:",
    "Не трудно заметить:",
    "Пытливый читатель уже мог догадаться что:",
    "Каждому бельчонку ясно:",
    "Стыдно было бы не знать:",
    "После третьего нервного срыва:",
    "После чашки кофе:",
    "ПОХУЙ УЖЕ:",
    "После пятой стопки егеря наконец-то получаем:",
    "Вроде бы пока с ответами сходится:",
    "После употребления тяжелых наркотиков вам станет понятно что:",
    "Иди нахуй:",
    "Получаем полный пиздец:",
    "Ебанутая хуйня:",
    "Залупа, дальше нет смысла решать:",
    "Я ща все тут расхуячу, потому что:",
    "Ебало завалите кому матан нравится:",
    "Похоже ты мой дорогой читатель девственник до конца дней:",
    "Похоже ты мой дорогой читатель инцен ебаный если тебе это интересно:",
    "Ебать мне похуй уже:",
    "Нихуя уже не сходится:",
    "Ну и вот мы получаем полный пиздец:",
    "Только блять попробуй сука не сократиться:",
    "А ну вроде норм, ща подгоним:",
    "Еба тебе делать нехуй:",
    "Сука остановись уже даже мне похуй:",
    "Пиздец у нас залупа получается:",
    "Опа нихуя у нас не получилось опять:",
    "НИ ХУЯ не сходится с ответами:",
    "Ну оставлю так может быть ошибка в условии:",
    "Пиздец куда 2 часа просрал:",
    "А ну вроде норм, ща подгоним:"
};

const int PHRASES_COUNT = sizeof(funny_phrases) / sizeof(funny_phrases[0]);

const char* GetPhrase(LatexDumpState* state){
    if (!state) return funny_phrases[0];
    
    int index = state->current_funny_index % PHRASES_COUNT;
    state->current_funny_index++;
    return funny_phrases[index];
}

void CtorLatexDump(LatexDumpState* state, const char* filename){
    if (!state || !filename) {
        printf("ERROR: invalid parameters in CtorLatexDump\n");
        return;
    }

    state->filename = filename;
    state->file_ptr = fopen(filename, "w");
    if (!state->file_ptr) {
        printf("ERROR: Cannot create LaTeX file: %s\n", filename);
        return;
    }

    state->step_counter = 1;
    state->current_funny_index = 0;
    state->funny_phrases = funny_phrases;
    state->funny_phrases_count = PHRASES_COUNT;

    StartLatexDump(state->file_ptr);

    fprintf(state->file_ptr, "\\section*{Пошаговый процесс расчленения и анального насилия}\n");
    fprintf(state->file_ptr, "\\textbf{Дата:} %s\\\\\n", __DATE__);
    fprintf(state->file_ptr, "\\vspace{0.5cm}\n\n");

    printf("LaTeX dump initialized: %s\n", filename);
}

void AddLatexStep(LatexDumpState* state, const char* description, ExpressionTree_t* tree){
    if (!state || !state->file_ptr || !description || !tree || !tree->root) {
        printf("WARNING: Cannot add LaTeX step - invalid parameters\n");
        return;
    }

    const char* funny_comment = GetPhrase(state);

    fprintf(state->file_ptr, "\\subsection*{%s}\n", description);
    fprintf(state->file_ptr, "\\textit{%s}\\\\\n", funny_comment);
    fprintf(state->file_ptr, "\\begin{dmath}\n");

    PrintTreeLatex(state->file_ptr, tree->root);

    fprintf(state->file_ptr, "\n\\end{dmath}\n\n");

    state->step_counter++;
}

void PrintTreeLatex(FILE* file, Node_t* node){
    if (!file || !node) return;

    char buffer[MAX_LATEX_EXPRESSION_LENGTH] = {0};
    int pos = 0;

    NodeToLatexString(node, buffer, &pos, sizeof(buffer));

    fprintf(file, "%s", buffer);
}

void NodeToLatexString(Node_t* node, char* buffer, int* pos, int buffer_size){
    if (node == NULL || *pos >= buffer_size - 10) return;

    switch (node->type) {
        case NUMBERTYPE:
            *pos += snprintf(buffer + *pos, buffer_size - *pos, "%d", node->tree_data.numb);
            break;

        case VARIABLETYPE:
            if (node->tree_data.varible) {
                *pos += snprintf(buffer + *pos, buffer_size - *pos, "%s", node->tree_data.varible);
            } else {
                *pos += snprintf(buffer + *pos, buffer_size - *pos, "?");
            }
            break;

        case OPERATORTYPE: {
            if (!node->tree_data.operat) break;
            
            char op = node->tree_data.operat[0];
            
            switch (op) {
                case '+':
                case '-':
                case '*':
                case '/':
                case '^':

                    if (op == '-' && node->left_child == NULL) {
                       
                        *pos += snprintf(buffer + *pos, buffer_size - *pos, "-");
                        NodeToLatexString(node->right_child, buffer, pos, buffer_size);
                    } else {
                        
                        NodeToLatexString(node->left_child, buffer, pos, buffer_size);
                        
                        if (op == '*')
                            *pos += snprintf(buffer + *pos, buffer_size - *pos, " \\cdot ");
                        else if (op == '/')
                            *pos += snprintf(buffer + *pos, buffer_size - *pos, " / ");
                        else
                            *pos += snprintf(buffer + *pos, buffer_size - *pos, " %c ", op);
                        
                        NodeToLatexString(node->right_child, buffer, pos, buffer_size);
                    }
                    break;
                    
                default:

                    if (strcmp(node->tree_data.operat, "sin") == 0) {
                        *pos += snprintf(buffer + *pos, buffer_size - *pos, "\\sin(");
                        NodeToLatexString(node->left_child, buffer, pos, buffer_size);
                        *pos += snprintf(buffer + *pos, buffer_size - *pos, ")");
                    }
                    else if (strcmp(node->tree_data.operat, "cos") == 0) {
                        *pos += snprintf(buffer + *pos, buffer_size - *pos, "\\cos(");
                        NodeToLatexString(node->left_child, buffer, pos, buffer_size);
                        *pos += snprintf(buffer + *pos, buffer_size - *pos, ")");
                    }
                    else if (strcmp(node->tree_data.operat, "tan") == 0) {
                        *pos += snprintf(buffer + *pos, buffer_size - *pos, "\\tan(");
                        NodeToLatexString(node->left_child, buffer, pos, buffer_size);
                        *pos += snprintf(buffer + *pos, buffer_size - *pos, ")");
                    }
                    else if (strcmp(node->tree_data.operat, "ln") == 0) {
                        *pos += snprintf(buffer + *pos, buffer_size - *pos, "\\ln(");
                        NodeToLatexString(node->left_child, buffer, pos, buffer_size);
                        *pos += snprintf(buffer + *pos, buffer_size - *pos, ")");
                    }
                    else if (strcmp(node->tree_data.operat, "exp") == 0) {
                        *pos += snprintf(buffer + *pos, buffer_size - *pos, "\\exp(");
                        NodeToLatexString(node->left_child, buffer, pos, buffer_size);
                        *pos += snprintf(buffer + *pos, buffer_size - *pos, ")");
                    }
                    else {
                        *pos += snprintf(buffer + *pos, buffer_size - *pos, "%s", node->tree_data.operat);
                    }
                    break;
            }
            break;
        }

        case NOTYPE:
        default:
            *pos += snprintf(buffer + *pos, buffer_size - *pos, "?");
            break;
    }
}

int StartLatexDump(FILE* file){
    if (file == NULL) return -1;

    fprintf(file, "\\documentclass[12pt]{article}\n");
    fprintf(file, "\\usepackage[utf8]{inputenc}\n");
    fprintf(file, "\\usepackage[T2A]{fontenc}\n");
    fprintf(file, "\\usepackage[russian]{babel}\n");
    fprintf(file, "\\usepackage{amsmath}\n");
    fprintf(file, "\\usepackage{geometry}\n");

    fprintf(file, "\\sloppy\n");
    fprintf(file, "\\tolerance=9999\n");
    fprintf(file, "\\emergencystretch=10pt\n");
    fprintf(file, "\\hbadness=10000\n");

    fprintf(file, "\\usepackage{breqn}\n");
    fprintf(file, "\\geometry{a4paper, left=10mm, right=10mm, top=30mm, bottom=30mm}\n");
    fprintf(file, "\\begin{document}\n\n");

    fprintf(file, "\\section*{MATHANAL}\n\n");
    return 0;
}

int EndLatexDump(FILE* file) {
    if (file == NULL) return -1;

    fprintf(file, "\\end{document}\n");
    return 0;
}

void DtorLatexDump(LatexDumpState* state) {
    if (!state || !state->file_ptr) {
        printf("WARNING: Cannot close LaTeX dump - invalid state\n");
        return;
    }

    fprintf(state->file_ptr, "\\section*{Заключение}\n");
    fprintf(state->file_ptr, "Процесс дифференцирования и упрощения нихуя не интересен если ты это читаешь то ты инцел сто проц.\\\\\n");
    fprintf(state->file_ptr, "Всего выполнено шагов: %d(ДОХУИЩА)\\\\\n", state->step_counter - 1);
    fprintf(state->file_ptr, "\\vspace{0.5cm}\n\n");

    EndLatexDump(state->file_ptr);
    fclose(state->file_ptr);
    state->file_ptr = NULL;

    printf("LaTeX document completed. Total steps: %d\n", state->step_counter - 1);

    printf("Compiling LaTeX to PDF...\n");
    char command[MAX_STR_SIZE] = {};
    snprintf(command, sizeof(command), "pdflatex -interaction=nonstopmode \"%s\"", state->filename);

    int result = system(command);

    if (result == 0)
        printf("SUCCESS: PDF generated: %s.pdf\n", state->filename);
    else
        printf("ERROR: Failed to compile LaTeX document\n");
}
