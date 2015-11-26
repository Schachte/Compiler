#ifndef _COMPILER_H_
#define _COMPILER_H_

/*
 * compiler.h
 */

typedef enum  { NOOP_STMT,
                PRINT_STMT,
                ASSIGN_STMT,
                IF_STMT,
                GOTO_STMT } StatementType;

#define KEYWORDS  9
#define RESERVED  29

typedef enum { END_OF_FILE = -1,
               VAR = 1,
               IF,
               WHILE,
               SWITCH,
               CASE,
               DEFAULT,
               PRINT,
               GOTO,
               FOR,
               PLUS,
               MINUS,
               DIV,
               MULT,
               EQUAL,
               COLON,
               COMMA,
               SEMICOLON,
               LBRAC,
               RBRAC,
               LPAREN,
               RPAREN,
               LBRACE,
               RBRACE,
               NOTEQUAL,
               GREATER,
               LESS,
               ID,
               NUM,
               ERROR } TokenType;

typedef enum { OP_NOOP,
               OP_PLUS,
               OP_MINUS,
               OP_MULT,
               OP_DIV } AssignmentOpType;

typedef enum { CONDITION_GREATER,
               CONDITION_LESS,
               CONDITION_NOTEQUAL } ConditionOpType;



// This implementation does not allow tokens
// that are more than 200 characters long
#define MAX_TOKEN_LENGTH 200

// The following global variables are defined in compiler.c:
extern char token[MAX_TOKEN_LENGTH];
extern TokenType  t_type;

//---------------------------------------------------------
// Data structures:

struct ValueNode
{
    char* name;
    int   value;
};

struct GotoStatement
{
    struct StatementNode* target;
};

struct AssignmentStatement
{
    struct ValueNode* left_hand_side;

    struct ValueNode* operand1;
    struct ValueNode* operand2;

    /*
     * If op is OP_NOOP then only operand1 is meaningful.
     * Otherwise op has to be one of the following values:
     * OP_PLUS, OP_MINUS, OP_MULT, OP_DIV
     * and both operands are meaningful
     */
    AssignmentOpType op;
};

struct PrintStatement
{
    struct ValueNode* id;
};

struct IfStatement
{
    struct ValueNode* condition_operand1;
    struct ValueNode* condition_operand2;

    /*
     * condition_op should be one of the following values:
     * CONDITION_GREATER, CONDITION_LESS, or CONDITION_NOTEQUAL
     */
    ConditionOpType condition_op;

    struct StatementNode* true_branch;
    struct StatementNode* false_branch;
};

struct StatementNode
{
    StatementType type;

    union
    {
        struct AssignmentStatement* assign_stmt;
        struct PrintStatement* print_stmt;
        struct IfStatement* if_stmt;
        struct GotoStatement* goto_stmt;
    };

    struct StatementNode* next; // next statement in the list or NULL
};

//---------------------------------------------------------
// Functions that are provided:

void debug(const char* format, ...);

TokenType getToken();
void ungetToken();

//---------------------------------------------------------
// Functions that you should write:

struct StatementNode* parse_generate_intermediate_representation();

/*
  NOTE:

  You need to write a function with the above signature. This function
  is supposed to parse the input program and generate an intermediate
  representation for it. The output of this function is passed to the
  execute_program function in main().

  Write your code in a separate file and include this header file (compiler.h)
  in your code as described below.

  A) If you are coding in C,

     Include compiler.h in your code like this:

     #include "compiler.h"

     Compile using the following command:

     gcc compiler.c yourcode.c

  B) If you are coding in C++,

     Include compiler.h in your code like this:

     extern "C" {
         #include "compiler.h"
     }

     Use the following commands to compile your code:

     gcc -c compiler.c
     g++ yourcode.cpp compiler.o

*/

#endif /* _COMPILER_H_ */
