#ifndef XYZ_COMPILER_GLOBALS_H
  #define XYZ_COMPILER_GLOBALS_H

  #include <stdio.h>
  #include <stdbool.h>
  #include <stdlib.h>
  #include <string.h>
  #include <stdarg.h>

  #define MAX_RESERVED_WORDS 10

  #define K_NRM  "\x1B[0m" // normal
  #define K_RED  "\x1B[31m" // red
  #define K_GRN  "\x1B[32m" // green
  #define K_YEL  "\x1B[33m" // yellow
  #define K_BLU  "\x1B[34m" // blue
  #define K_MAG  "\x1B[35m" // magenta
  #define K_CYN  "\x1B[36m" // cyan

  typedef enum {
    // bookkeeping tokens
    T_EOF = 256, T_ERROR, T_ID, T_UNKNOWN,
    // language construct
    T_MAIN, T_GETS, T_PUTS,
    // keywords
    T_CHAR, T_INT, T_DECIMAL,
    // if statement
    T_IF, T_THEN, T_ELSE,
    // loop statement
    T_FOR,
    // multi character tokens
    T_VARIABLE, T_STRING, T_LNUMBER, T_DNUMBER,
    // assignment operators
    T_ASSIGN_EQUAL,
    // relational symbols
    T_IS_EQUAL, T_IS_NOT_EQUAL, T_IS_GREATER, T_IS_SMALLER,
    T_IS_GREATER_OR_EQUAL, T_IS_SMALLER_OR_EQUAL,
    // logical operators
    T_BOOLEAN_OR, T_BOOLEAN_AND,
    // arithmetic operators
    T_ARITH_PLUS, T_ARITH_MINUS, T_ARITH_MUL, T_ARITH_DIV,
    // increment/decrement operators
    T_INC, T_DEC,
    // special symbols
    T_PAREN_OPEN, T_PAREN_CLOSE, T_CURLY_OPEN, T_CURLY_CLOSE,
    T_SEMICOLON, T_COMMA, T_DOT,
    // ' ', \t, \r, \n, \v and \f
    // T_WHITESPACE,
    // anything below ASCII 32 except whitespaces
    // T_BAD_CHARACTER
  } TokenType;

  typedef struct token {
    TokenType type;
    size_t line;
    size_t column;
    FILE *file;
    struct {
      char *sval;
      size_t slen;
    };
  } Token;

  typedef struct file {
    FILE *file;
    char *name;
    size_t line;
    size_t column;
    char last;
    size_t ntoken;
  } File;

  #define BUFFER_LENGTH 256

  extern File *source;

  extern bool print_tokens;
  extern bool print_symbol_table;
  extern bool print_syntax_tree;

  typedef enum { K_STMT, K_EXP } NodeKind;
  typedef enum { K_DECL = 8, K_MAIN, K_IF, K_FOR, K_ASSIGN, K_GETS, K_PUTS } StmtKind;
  typedef enum { K_OP = 16, K_CONST, K_ICONST, K_DCONST, K_VAR } ExpKind;
  typedef enum { K_VOID = 32, K_INTEGER, K_BOOL, K_DECIMAL, K_CHAR } ExpType;

  #define MAX_CHILDREN 5

  typedef struct treeNode {
    struct treeNode *child[MAX_CHILDREN];
    struct treeNode *sibling;
    size_t line_number;
    NodeKind nodeKind;
    union {
      StmtKind stmt;
      ExpKind exp;
    } kind;
    union {
      TokenType op;
      int iVal;
      double dVal;
      char *name;
      struct {
        char *name;
        TokenType type;
      } var;
    } attr;
    ExpType type;
  } TreeNode;
#endif
