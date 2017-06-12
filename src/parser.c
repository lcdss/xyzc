#include <limits.h>
#include "parser.h"

static Token *token;

static TreeNode * read_stmt_sequence(void);
static TreeNode * read_statement(void);
static TreeNode * read_decl_stmt(void);
static TreeNode * read_int_decl_stmt(void);
static TreeNode * read_char_decl_stmt(void);
static TreeNode * read_dec_decl_stmt(void);
static TreeNode * read_main_stmt(void);
static TreeNode * read_if_stmt(void);
static TreeNode * read_for_stmt(void);
static TreeNode * read_assign_stmt(void);
static TreeNode * read_gets_stmt(void);
static TreeNode * read_puts_stmt(void);
static TreeNode * read_expr(void);
static TreeNode * read_simple_expr(void);
static TreeNode * read_term(void);
static TreeNode * read_factor(void);
static TreeNode * read_variable(void);

static void syntax_error(const char *message, ...)
{
  va_list args;

  va_start(args, message);

  fprintf(stderr, "\n%s>>> Syntax error:%s ", K_MAG, K_NRM);
  vfprintf(stderr, message, args);
  fprintf(stderr, " at %lu:%lu\n", token->line, token->column - token->slen + 1);
  fprintf(stderr, "\n%s", get_line(token->line));

  for (size_t i = 0; i < token->column - token->slen; i++) {
    printf(" ");
  }

  printf("%s^%s\n", K_CYN, K_NRM);

  va_end(args);

  exit(-1);
}

static void match(TokenType expected)
{
  if (token->type == expected) {
    token = read_token();

    return;
  }

  syntax_error("Expected a \"%s\", but \"%s\" was given", token_type_to_str(expected), token_to_str(token));
}

TreeNode * read_stmt_sequence(void)
{
  if (token->type == T_CURLY_CLOSE) {
    return NULL;
  }

  TreeNode *t = read_statement();
  TreeNode *p = t;

  while (token->type != T_EOF && token->type != T_ELSE && token->type != T_CURLY_CLOSE) {
    TreeNode *q;

    q = read_statement();

    if (t == NULL) {
      t = p = q;
    } else {
      p->sibling = q;
      p = q;
    }
  }

  return t;
}

TreeNode * read_statement(void)
{
  TreeNode *t = NULL;

  switch (token->type) {
  case T_INT:
  case T_CHAR:
  case T_DECIMAL:
    t = read_decl_stmt();
    match(T_SEMICOLON);
    break;
  case T_IF:
    t = read_if_stmt();
    break;
  case T_FOR:
    t = read_for_stmt();
    break;
  case T_VARIABLE:
  case T_INC:
  case T_DEC:
    t = read_assign_stmt();
    match(T_SEMICOLON);
    break;
  case T_GETS:
    t = read_gets_stmt();
    break;
  case T_PUTS:
    t = read_puts_stmt();
    break;
  default:
    syntax_error("Statement: Unexpected token \"%s\"", token_to_str(token));
    token = read_token();
    break;
  }

  return t;
}

static TreeNode * read_decl_stmt(void)
{
  if (token->type == T_CHAR) {
    return read_char_decl_stmt();
  } else if (token->type == T_DECIMAL) {
    return read_dec_decl_stmt();
  }

  return read_int_decl_stmt();
}

static TreeNode * read_int_decl_stmt(void)
{
  TreeNode *t = make_stmt_node(K_DECL);
  TreeNode *p;
  unsigned int i = 0;

  match(T_INT);

  p = read_variable();
  p->attr.var.type = T_INT;
  t->attr.op = T_INT;
  t->child[i++] = p;

  if (token->type == T_ASSIGN_EQUAL) {
    match(token->type);
    p->child[0] = read_expr();
  }

  while (token->type == T_COMMA) {
    match(T_COMMA);

    p = read_variable();
    p->attr.var.type = T_INT;
    t->attr.op = T_INT;
    t->child[i++] = p;

    if (token->type == T_ASSIGN_EQUAL) {
      match(token->type);
      p->child[0] = read_expr();
    }
  }

  return t;
}

static TreeNode * read_char_decl_stmt(void)
{
  TreeNode *t = make_stmt_node(K_DECL);
  TreeNode *p;
  unsigned int i = 0;

  match(T_CHAR);


  p = read_variable();
  p->attr.var.type = T_CHAR;
  t->child[i++] = p;
  t->attr.op = T_CHAR;

  match(T_PAREN_OPEN);
  match(T_LNUMBER);
  match(T_PAREN_CLOSE);

  if (token->type == T_ASSIGN_EQUAL) {
    match(token->type);
    p->child[0] = read_expr();
  }

  while (token->type == T_COMMA) {
    match(T_COMMA);

    p = read_variable();
    p->attr.var.type = T_CHAR;
    t->child[i++] = p;
    t->attr.op = T_CHAR;

    match(T_PAREN_OPEN);
    match(T_LNUMBER);
    match(T_PAREN_CLOSE);

    if (token->type == T_ASSIGN_EQUAL) {
      match(token->type);
      p->child[0] = read_expr();
    }
  }

  return t;
}

static TreeNode * read_dec_decl_stmt(void)
{
  TreeNode *t = make_stmt_node(K_DECL);
  TreeNode *p;
  unsigned int i = 0;

  match(T_DECIMAL);

  p = read_variable();
  p->attr.var.type = T_DECIMAL;
  t->attr.op = T_DECIMAL;
  t->child[i++] = p;

  match(T_PAREN_OPEN);
  match(T_DNUMBER);
  match(T_PAREN_CLOSE);

  if (token->type == T_ASSIGN_EQUAL) {
    match(token->type);
    p->child[0] = read_expr();
  }

  while (token->type == T_COMMA) {
    match(T_COMMA);

    p = read_variable();
    p->attr.var.type = T_DECIMAL;
    t->attr.op = T_DECIMAL;
    t->child[i++] = p;

    match(T_PAREN_OPEN);
    match(T_DNUMBER);
    match(T_PAREN_CLOSE);

    if (token->type == T_ASSIGN_EQUAL) {
      match(token->type);
      p->child[0] = read_expr();
    }
  }

  return t;
}

TreeNode * read_main_stmt(void)
{
  TreeNode *t = make_stmt_node(K_MAIN);

  match(T_MAIN);
  match(T_PAREN_OPEN);
  match(T_PAREN_CLOSE);
  match(T_CURLY_OPEN);

  t->child[0] = read_stmt_sequence();

  match(T_CURLY_CLOSE);

  return t;
}

TreeNode * read_if_stmt(void)
{
  TreeNode *t = make_stmt_node(K_IF);

  match(T_IF);
  match(T_PAREN_OPEN);

  t->child[0] = read_expr();

  match(T_PAREN_CLOSE);
  match(T_THEN);

  if (token->type == T_CURLY_OPEN) {
    match(T_CURLY_OPEN);

    t->child[1] = read_stmt_sequence();

    match(T_CURLY_CLOSE);
  } else {
    t->child[1] = read_statement();
  }

  if (token->type == T_ELSE) {
    match(T_ELSE);

    if (token->type == T_CURLY_OPEN) {
      match(T_CURLY_OPEN);

      t->child[2] = read_stmt_sequence();

      match(T_CURLY_CLOSE);
    } else {
      t->child[2] = read_statement();
    }
  }

  return t;
}

TreeNode * read_for_stmt(void)
{
  TreeNode *t = make_stmt_node(K_FOR);

  match(T_FOR);
  match(T_PAREN_OPEN);

  if (token->type == T_INT || token->type == T_CHAR || token->type == T_DECIMAL) {
    t->child[0] = read_decl_stmt();
  } else {
    t->child[0] = read_assign_stmt();
  }

  match(T_SEMICOLON);

  t->child[1] = read_expr();

  match(T_SEMICOLON);

  t->child[2] = read_assign_stmt();

  match(T_PAREN_CLOSE);

  if (token->type == T_CURLY_OPEN) {
    match(T_CURLY_OPEN);

    t->child[3] = read_stmt_sequence();

    match(T_CURLY_CLOSE);
  } else {
    t->child[3] = read_statement();
  }

  return t;
}

TreeNode * read_assign_stmt(void)
{
  TreeNode *t = make_stmt_node(K_ASSIGN);

  if (token->type == T_INC || token->type == T_DEC) {
    match(token->type);

    t->attr.name = str_clone(token->sval);
    t->child[0] = read_variable();
  } else {
    t->attr.name = str_clone(token->sval);
    t->child[0] = read_variable();

    if (token->type == T_INC || token->type == T_DEC) {
      match(token->type);
    } else {
      match(T_ASSIGN_EQUAL);

      t->child[1] = read_expr();
    }
  }

  return t;
}

TreeNode * read_gets_stmt(void)
{
  TreeNode *t = make_stmt_node(K_GETS);
  unsigned int i = 0;

  match(T_GETS);
  match(T_PAREN_OPEN);

  t->child[i++] = read_variable();

  while (token->type == T_COMMA) {
    match(T_COMMA);

    t->child[i++] = read_variable();
  }

  match(T_PAREN_CLOSE);
  match(T_SEMICOLON);

  return t;
}

TreeNode * read_puts_stmt(void)
{
  TreeNode *t = make_stmt_node(K_PUTS);

  match(T_PUTS);
  match(T_PAREN_OPEN);

  if (token->type == T_VARIABLE || token->type == T_STRING) {
    match(token->type);
  } else {
    syntax_error("Expected a \"variable\" ou \"literal char\", but %s was given", token_type_to_str(token->type));
  }

  while (token->type == T_ARITH_PLUS) {
    match(T_ARITH_PLUS);

    if (token->type == T_VARIABLE || token->type == T_STRING) {
      match(token->type);
    } else {
      syntax_error("Expected a \"variable\" ou \"literal char\", but %s was given", token_type_to_str(token->type));
    }
  }

  match(T_PAREN_CLOSE);
  match(T_SEMICOLON);

  return t;
}

TreeNode * read_expr(void)
{
  TreeNode *t;

  t = read_simple_expr();

  while (is_relacional_op(token->type)) {
    TreeNode *p = make_exp_node(K_OP);
    p->child[0] = t;
    p->attr.op = token->type;
    t = p;

    match(token->type);

    t->child[1] = read_simple_expr();
  }

  return t;
}

TreeNode * read_simple_expr(void)
{
  TreeNode *t;

  t = read_term();

  while (token->type == T_ARITH_PLUS || token->type == T_ARITH_MINUS) {
    TreeNode *p = make_exp_node(K_OP);
    p->child[0] = t;
    p->attr.op = token->type;
    t = p;

    match(token->type);

    t->child[1] = read_term();
  }

  return t;
}

TreeNode * read_term(void)
{
  TreeNode *t;

  t = read_factor();

  while (token->type == T_ARITH_MUL || token->type == T_ARITH_DIV) {
    TreeNode *p = make_exp_node(K_OP);
    p->child[0] = t;
    p->attr.op = token->type;
    t = p;

    match(token->type);

    p->child[1] = read_factor();
  }

  return t;
}

TreeNode * read_factor(void)
{
  TreeNode *t = NULL;

  switch (token->type) {
  case T_LNUMBER:
    t = make_exp_node(K_ICONST);

    if (token->type == T_LNUMBER) {
      t->attr.iVal = atoi(str_clone(token->sval));
    }

    match(T_LNUMBER);
    break;
  case T_DNUMBER:
    t = make_exp_node(K_DCONST);

    if (token->type == T_DNUMBER) {
      t->attr.dVal = atof(str_clone(token->sval));
    }

    match(T_DNUMBER);
    break;
  case T_STRING:
    t = make_exp_node(K_CONST);

    if (token->type == T_STRING) {
      t->attr.name = str_clone(token->sval);
    }

    match(T_STRING);
    break;
  case T_VARIABLE:
    t = read_variable();
    break;
  case T_PAREN_OPEN:
    match(T_PAREN_OPEN);
    t = read_expr();
    match(T_PAREN_CLOSE);
    break;
  default:
    syntax_error("Unexpected token \"%s\"", token_to_str(token));
    token = read_token();
    break;
  }

  return t;
}

static TreeNode * read_variable(void)
{
  TreeNode *t = make_exp_node(K_VAR);

  if (token->type == T_VARIABLE) {
    t->attr.var.name = str_clone(token->sval);
  }

  match(T_VARIABLE);

  return t;
}

TreeNode * parse(void)
{
  TreeNode *t;

  token = read_token();

  t = read_main_stmt();

  if (token->type != T_EOF) {
    syntax_error("Expected EOF, but \"%s\" was given.", token_to_str(token));
  }

  return t;
}
