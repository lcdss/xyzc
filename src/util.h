#ifndef XYZ_COMPILER_UTIL_H
  #define XYZ_COMPILER_UTIL_H

  #include <ctype.h>
  #include "globals.h"

  const char * token_type_to_str(int token_type);

  const char * token_to_str(Token *token);

  char * str_trim(char *str);

  char * str_clone(char *str);

  bool is_relacional_op(TokenType token);

  bool is_arithmetic_op(TokenType token);

  Token * make_token(void);

  File * make_file(FILE *file, char *name);

  TreeNode * make_stmt_node(StmtKind kind);

  TreeNode * make_exp_node(ExpKind kind);

  void print_tree(TreeNode *tree);

  char * get_line(size_t line);

  char * file_get_contents(char *filename);

  size_t file_put_contents(char *filename, char *data);
#endif
