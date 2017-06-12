#include "analyzer.h"

static unsigned int location = 0;

static void type_error(const char *message, TreeNode *t, ...)
{
  va_list args;

  va_start(args, t);

  fprintf(stderr, "\n%s>>> Type error:%s ", K_YEL, K_NRM);
  vfprintf(stderr, message, args);
  fprintf(stderr, " at line %lu\n%s", t->line_number, K_NRM);
  fprintf(stderr, "%s", get_line(t->line_number));

  va_end(args);

  exit(-1);
}

static void null_proc(TreeNode *t)
{
  if (t == NULL) {
    return;
  }
}

static void transverse(TreeNode *t, void (*preProc)(TreeNode *), void (*postProc)(TreeNode *))
{
  if (t != NULL) {
    preProc(t);

    for (int i = 0; i < MAX_CHILDREN; i++) {
      transverse(t->child[i], preProc, postProc);
    }

    postProc(t);
    transverse(t->sibling, preProc, postProc);
  }
}

static void insert_node(TreeNode *t)
{
  switch (t->nodeKind) {
  case K_EXP:
    switch (t->kind.exp) {
    case K_VAR: {
      SymbolList symbol = st_lookup(t->attr.var.name);

      if (symbol && t->attr.var.type != T_UNKNOWN) {
        type_error("Variable \"%s\" is already declarated", t, t->attr.var.name);
      } else if (symbol) {
        t->attr.var.type = symbol->type;
        st_insert(t->attr.var.name, t->line_number, 0, 0);
      } else if (symbol == NULL && t->attr.var.type == T_UNKNOWN) {
        type_error("Variable \"%s\" not declarated but used", t, t->attr.var.name);
      } else {
        st_insert(t->attr.var.name, t->line_number, t->attr.var.type, location++);
      }
      break;
    }
    default:
      break;
    }
    break;
  default:
    break;
  }
}

void build_symtab(TreeNode *syntax_tree)
{
  transverse(syntax_tree, insert_node, null_proc);

  if (print_symbol_table) {
    st_print();
  }
}

static void check_type(TreeNode *t, ExpKind kind)
{
  if (t->nodeKind == K_EXP) {
    switch (t->kind.exp) {
    case K_CONST:
    case K_DCONST:
    case K_ICONST:
      if (t->kind.exp != kind) {
        type_error("Assignment of a wrong data type: expecting a \"%s\", but \"%s\" was given",
          t, token_type_to_str(kind), token_type_to_str(t->kind.exp)
          );
      }
      break;
    case K_VAR: {
      SymbolList symbol = st_lookup(t->attr.var.name);

      if ((symbol->type == T_INT && kind != K_ICONST) ||
        (symbol->type == T_CHAR && kind != K_CONST) ||
        (symbol->type == T_DECIMAL && kind != K_DCONST)
        ) {
        type_error("Assignment of a wrong data type: expecting a \"%s\", but \"%s\" was given",
          t, token_type_to_str(kind), token_type_to_str(symbol->type)
          );
      }
      break;
    }
    default:
      break;
    }
  }

  for (int i = 0; i < MAX_CHILDREN; i++) {
    if (t->child[i] != NULL) {
      check_type(t->child[i], kind);
    }
  }
}

static void check_node(TreeNode *t)
{
  switch (t->nodeKind) {
  case K_EXP:
    switch (t->kind.exp) {
    case K_OP:
      if (is_relacional_op(t->attr.op)) {
        t->type = K_BOOL;
      } else if (is_arithmetic_op(t->attr.op)) {
        t->type = K_INTEGER;
      }
    break;
  case K_CONST:
    t->type = K_CHAR;
    break;
  case K_ICONST:
    t->type = K_INTEGER;
    break;
  case K_DCONST:
    t->type = K_DECIMAL;
    break;
  default:
    break;
  }
  break;
case K_STMT:
  switch (t->kind.stmt) {
  case K_DECL:
    if (t->attr.op == T_INT) {
      check_type(t, K_ICONST);
    } else if (t->attr.op == T_CHAR) {
      check_type(t, K_CONST);
    } else if (t->attr.op == T_DECIMAL) {
      check_type(t, K_DCONST);
    }
    break;
  case K_ASSIGN:
    if (t->child[1] == NULL && t->child[0]->attr.var.type != T_INT) {
      type_error("incrementing/decrementing of non-integer value", t->child[0]);
    } else if (t->child[1] != NULL) {
      if (t->child[0]->attr.var.type == T_INT) {
        check_type(t->child[1], K_ICONST);
      } else if (t->child[0]->attr.var.type == T_CHAR) {
        check_type(t->child[1], K_CONST);
      } else if (t->child[0]->attr.var.type == T_DECIMAL) {
        check_type(t->child[1], K_DCONST);
      }
    }
    break;
  default:
    break;
  }
  break;
default:
  break;
}
}

void type_check(TreeNode *syntax_tree)
{
  transverse(syntax_tree, null_proc, check_node);
}
