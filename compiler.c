#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "compiler.h"

#define DEBUG 1     // 1 => Turn ON debugging, 0 => Turn OFF debugging

void debug(const char * format, ...)
{
    va_list args;
    if (DEBUG)
    {
        va_start (args, format);
        vfprintf (stdout, format, args);
        va_end (args);
    }
}

//---------------------------------------------------------
// Lexer

char token[MAX_TOKEN_LENGTH];      // token string
TokenType  t_type;                 // token type
bool  activeToken = false;
unsigned int  token_length;
unsigned int  line_no = 1;

const char *reserved[] =
{
    "",
    "VAR",
    "IF",
    "WHILE",
    "SWITCH",
    "CASE",
    "DEFAULT",
    "print",
    "GOTO",
    "FOR",
    "+",
    "-",
    "/",
    "*",
    "=",
    ":",
    ",",
    ";",
    "[",
    "]",
    "(",
    ")",
    "{",
    "}",
    "<>",
    ">",
    "<",
    "ID",
    "NUM",
    "ERROR"
};

void skipSpace()
{
    char c;

    c = getchar();
    line_no += (c == '\n');
    while (!feof(stdin) && isspace(c))
    {
        c = getchar();
        line_no += (c == '\n');
    }

    // return character to input buffer if eof is not reached
    if (!feof(stdin))
    {
        ungetc(c, stdin);
    }
}

int isKeyword(char *s)
{
    int i;

    for (i = 1; i <= KEYWORDS; i++)
    {
        if (strcmp(reserved[i], s) == 0)
        {
            return i;
        }
    }
    return false;
}

TokenType scan_number()
{
    char c;

    c = getchar();
    if (isdigit(c))
    {
        if (c == '0')
        {
            token[token_length] = c;
            token_length++;
        }
        else
        {
            while (isdigit(c))
            {
                token[token_length] = c;
                token_length++;;
                c = getchar();
            }
            ungetc(c, stdin);
        }
        token[token_length] = '\0';
        return NUM;
    }
    else
    {
        return ERROR;
    }
}


TokenType scan_id_or_keyword()
{
    TokenType t_type;
    char c;
    int k;

    c = getchar();
    if (isalpha(c))
    {
        while (isalnum(c) && token_length < MAX_TOKEN_LENGTH)
        {
            token[token_length] = c;
            token_length++;
            c = getchar();
        }
        if (!feof(stdin))
        {
            ungetc(c, stdin);
        }

        token[token_length] = '\0';
        k = isKeyword(token);
        if (k == 0)
        {
            t_type = ID;
        }
        else
        {
            t_type = (TokenType) k;
        }
        return t_type;
    }
    else
    {
        return ERROR;
    }
}

void ungetToken()
{
    activeToken = true;
}

TokenType getToken()
{
    char c;

    if (activeToken)
    {
        activeToken = false;
        return t_type;
    }

    skipSpace();
    token_length = 0;
    c = getchar();

    switch (c)
    {
        case '+':   t_type = PLUS;      break;
        case '-':   t_type = MINUS;     break;
        case '/':   t_type = DIV;       break;
        case '*':   t_type = MULT;      break;
        case '=':   t_type = EQUAL;     break;
        case ':':   t_type = COLON;     break;
        case ',':   t_type = COMMA;     break;
        case ';':   t_type = SEMICOLON; break;
        case '[':   t_type = LBRAC;     break;
        case ']':   t_type = RBRAC;     break;
        case '(':   t_type = LPAREN;    break;
        case ')':   t_type = RPAREN;    break;
        case '{':   t_type = LBRACE;    break;
        case '}':   t_type = RBRACE;    break;
        case '>':   t_type = GREATER;   break;
        case '<':
            c = getchar();
            if (c == '>')
            {
                t_type = NOTEQUAL;
            }
            else
            {
                ungetc(c, stdin);
                t_type = LESS;
            }
            break;
        default :
            if (isdigit(c))
            {
                ungetc(c, stdin);
                t_type = scan_number();
            }
            else if (isalpha(c))
            {
                // token is either a keyword or ID
                ungetc(c, stdin);
                t_type = scan_id_or_keyword();
            }
            else if (c == EOF)
            {
                t_type = END_OF_FILE;
            }
            else
            {
                t_type = ERROR;
            }
            break;
    } // End Switch
    return t_type;
}

//---------------------------------------------------------
// Execute
void execute_program(struct StatementNode* program)
{
    struct StatementNode* pc = program;
    int op1, op2, result;

    while (pc != NULL)
    {
        switch (pc->type)
        {
            case NOOP_STMT:
                pc = pc->next;
                break;

            case PRINT_STMT:
                if (pc->print_stmt == NULL)
                {
                    debug("Error: pc points to a print statement but pc->print_stmt is null.\n");
                    exit(1);
                }
                if (pc->print_stmt->id == NULL)
                {
                    debug("Error: print_stmt->id is null.\n");
                    exit(1);
                }
                printf("%d\n", pc->print_stmt->id->value);
                pc = pc->next;
                break;

            case ASSIGN_STMT:
                if (pc->assign_stmt == NULL)
                {
                    debug("Error: pc points to an assignment statement but pc->assign_stmt is null.\n");
                    exit(1);
                }
                if (pc->assign_stmt->operand1 == NULL)
                {
                    debug("Error: assign_stmt->operand1 is null.\n");
                    exit(1);
                }
                if (pc->assign_stmt->op != OP_NOOP)
                {
                    if (pc->assign_stmt->operand2 == NULL)
                    {
                        debug("Error: assign_stmt->op requires two operands but assign_stmt->operand2 is null.\n");
                        exit(1);
                    }
                }
                if (pc->assign_stmt->left_hand_side == NULL)
                {
                    debug("Error: assign_stmt->left_hand_side is null.\n");
                    exit(1);
                }
                switch (pc->assign_stmt->op)
                {
                    case OP_PLUS:
                        op1 = pc->assign_stmt->operand1->value;
                        op2 = pc->assign_stmt->operand2->value;
                        result = op1 + op2;
                        break;
                    case OP_MINUS:
                        op1 = pc->assign_stmt->operand1->value;
                        op2 = pc->assign_stmt->operand2->value;
                        result = op1 - op2;
                        break;
                    case OP_MULT:
                        op1 = pc->assign_stmt->operand1->value;
                        op2 = pc->assign_stmt->operand2->value;
                        result = op1 * op2;
                        break;
                    case OP_DIV:
                        op1 = pc->assign_stmt->operand1->value;
                        op2 = pc->assign_stmt->operand2->value;
                        result = op1 / op2;
                        break;
                    case OP_NOOP:
                        op1 = pc->assign_stmt->operand1->value;
                        result = op1;
                        break;
                    default:
                        debug("Error: invalid value for assign_stmt->op (%d).\n", pc->assign_stmt->op);
                        exit(1);
                        break;
                }
                pc->assign_stmt->left_hand_side->value = result;
                pc = pc->next;
                break;

            case IF_STMT:
                if (pc->if_stmt == NULL)
                {
                    debug("Error: pc points to an if statement but pc->if_stmt is null.\n");
                    exit(1);
                }
                if (pc->if_stmt->true_branch == NULL)
                {
                    debug("Error: if_stmt->true_branch is null.\n");
                    exit(1);
                }
                if (pc->if_stmt->false_branch == NULL)
                {
                    debug("Error: if_stmt->false_branch is null.\n");
                    exit(1);
                }
                if (pc->if_stmt->condition_operand1 == NULL)
                {
                    debug("Error: if_stmt->condition_operand1 is null.\n");
                    exit(1);
                }
                if (pc->if_stmt->condition_operand2 == NULL)
                {
                    debug("Error: if_stmt->condition_operand2 is null.\n");
                    exit(1);
                }
                op1 = pc->if_stmt->condition_operand1->value;
                op2 = pc->if_stmt->condition_operand2->value;
                switch (pc->if_stmt->condition_op)
                {
                    case CONDITION_GREATER:
                        if (op1 > op2)
                        {
                            pc = pc->if_stmt->true_branch;
                        }
                        else
                        {
                            pc = pc->if_stmt->false_branch;
                        }
                        break;
                    case CONDITION_LESS:
                        if (op1 < op2)
                        {
                            pc = pc->if_stmt->true_branch;
                        }
                        else
                        {
                            pc = pc->if_stmt->false_branch;
                        }
                        break;
                    case CONDITION_NOTEQUAL:
                        if (op1 != op2)
                        {
                            pc = pc->if_stmt->true_branch;
                        }
                        else
                        {
                            pc = pc->if_stmt->false_branch;
                        }
                        break;
                    default:
                        debug("Error: invalid value for if_stmt->condition_op (%d).\n", pc->if_stmt->condition_op);
                        exit(1);
                        break;
                }
                break;

            case GOTO_STMT:
                if (pc->goto_stmt == NULL)
                {
                    debug("Error: pc points to a goto statement but pc->goto_stmt is null.\n");
                    exit(1);
                }
                if (pc->goto_stmt->target == NULL)
                {
                    debug("Error: goto_stmt->target is null.\n");
                    exit(1);
                }
                pc = pc->goto_stmt->target;
                break;

            default:
                debug("Error: invalid value for pc->type (%d).\n", pc->type);
                exit(1);
                break;
        }
    }
}

int main()
{
    struct StatementNode * program;
    program = parse_generate_intermediate_representation();
    execute_program(program);
    return 0;
}
