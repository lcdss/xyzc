#include "util.h"

#define INDENT indent_size += 2;
#define UNINDENT indent_size -= 2;

static unsigned int indent_size = 0;

const char * token_type_to_str(int token_type)
{
  switch (token_type) {
  case T_IF: return "if";
  case T_THEN: return "then";
  case T_ELSE: return "else";
  case T_FOR: return "for";
  case T_PUTS: return "puts";
  case T_GETS: return "gets";
  case T_MAIN: return "main";
  case K_INTEGER:
  case K_ICONST:
  case T_INT: return "int";
  case K_CONST:
  case T_CHAR: return "char";
  case K_DCONST:
  case T_DECIMAL: return "dec";
  case T_LNUMBER: return "literal integer";
  case T_DNUMBER: return "literal decimal";
  case T_STRING: return "literal char";
  case K_BOOL: return "bool";
  case T_VARIABLE: return "variable";
  case T_ASSIGN_EQUAL: return "=";
  case T_IS_EQUAL: return "==";
  case T_IS_NOT_EQUAL: return "!=";
  case T_IS_GREATER: return ">";
  case T_IS_GREATER_OR_EQUAL: return ">=";
  case T_IS_SMALLER: return "<";
  case T_IS_SMALLER_OR_EQUAL: return "<=";
  case T_BOOLEAN_OR: return "||";
  case T_BOOLEAN_AND: return "&&";
  case T_ARITH_MINUS: return "-";
  case T_ARITH_PLUS: return "+";
  case T_ARITH_MUL: return "*";
  case T_ARITH_DIV: return "/";
  case T_INC: return "++";
  case T_DEC: return "--";
  case T_PAREN_OPEN: return "(";
  case T_PAREN_CLOSE: return ")";
  case T_CURLY_OPEN: return "{";
  case T_CURLY_CLOSE: return "}";
  case T_SEMICOLON: return ";";
  case T_COMMA: return ",";
  case T_DOT: return ".";
  case T_EOF: return "EOF";
  default: return "UNKNOWN";
  }
}

const char * token_to_str(Token *token)
{
  switch (token->type) {
  case T_LNUMBER:
  case T_DNUMBER:
  case T_STRING:
  case T_VARIABLE: return token->sval;
  default: return token_type_to_str(token->type);
  }
}

char * str_trim(char *str)
{
  char *end;

  while (isspace((unsigned char) *str)) {
    str++;
  }

  if (*str == 0) {
    return str;
  }

  end = str + strlen(str) - 1;

  while (end > str && isspace((unsigned char) *end)) {
    end--;
  }

  *(end + 1) = 0;

  return str;
}

bool is_relacional_op(TokenType type)
{
  return type == T_IS_GREATER || type == T_IS_SMALLER ||
         type == T_IS_NOT_EQUAL || type == T_IS_EQUAL ||
         type == T_IS_GREATER_OR_EQUAL || type == T_IS_SMALLER_OR_EQUAL ||
         type == T_BOOLEAN_OR || type == T_BOOLEAN_AND;
}

bool is_arithmetic_op(TokenType type)
{
  return type == T_ARITH_MINUS || type == T_ARITH_PLUS ||
         type == T_ARITH_DIV || type == T_ARITH_MUL;
}

Token * make_token(void)
{
  Token *token = malloc(sizeof(Token));

  token->type = T_UNKNOWN;
  token->line = 0;
  token->column = 0;
  token->slen = 0;
  token->sval = NULL;
  token->file = NULL;

  return token;
}

File * make_file(FILE *file, char *name)
{
  File *f = malloc(sizeof(File));
  f->file = file;
  f->name = name;
  f->line = 0;
  f->column = 0;

  return f;
}

TreeNode * make_stmt_node(StmtKind kind)
{
  TreeNode *node = malloc(sizeof(TreeNode));

  for (int i = 0; i < MAX_CHILDREN; i++) {
    node->child[i] = NULL;
  }

  node->sibling = NULL;
  node->nodeKind = K_STMT;
  node->kind.stmt = kind;
  node->line_number = source->line;

  return node;
}

TreeNode * make_exp_node(ExpKind kind)
{
  TreeNode *t = malloc(sizeof(TreeNode));

  for (int i = 0; i < MAX_CHILDREN; i++) {
    t->child[i] = NULL;
  }

  if (kind == K_VAR) {
    t->attr.var.type = T_UNKNOWN;
  }

  t->sibling = NULL;
  t->nodeKind = K_EXP;
  t->kind.stmt = kind;
  t->line_number = source->line;
  t->type = K_VOID;

  return t;
}

char * str_clone(char *str)
{
  size_t length = strlen(str) + 1;
  char *clone;

  if (str == NULL) {
    return NULL;
  }

  clone = malloc(length);

  strcpy(clone, str);

  return clone;
}

static void print_spaces(void)
{
  for (unsigned int i = 0; i < indent_size; i++) {
    fprintf(stdout, " ");
  }
}

void print_tree(TreeNode *tree)
{
  INDENT;

  while (tree != NULL) {
    print_spaces();

    if (tree->nodeKind == K_STMT) {
      switch (tree->kind.stmt) {
      case K_MAIN:
        fprintf(stdout, "%sMain%s\n", K_RED, K_NRM);
        break;
      case K_IF:
        fprintf(stdout, "%sIf%s\n", K_RED, K_NRM);
        break;
      case K_FOR:
        fprintf(stdout, "%sFor%s\n", K_RED, K_NRM);
        break;
      case K_DECL:
        fprintf(stdout, "%sDeclaration%s: %s\n", K_GRN, K_NRM, token_type_to_str(tree->attr.op));
        break;
      case K_ASSIGN:
        fprintf(stdout, "%sAssign%s: %s\n", K_YEL, K_NRM, tree->attr.name);
        break;
      case K_GETS:
        fprintf(stdout, "%sGets%s\n", K_BLU, K_NRM);
        break;
      case K_PUTS:
        fprintf(stdout, "%sPuts%s\n", K_BLU, K_NRM);
        break;
      default:
        fprintf(stdout, "Unknow StmtNode kind\n");
        break;
      }
    } else if (tree->nodeKind == K_EXP) {
      switch (tree->kind.exp) {
      case K_OP:
        fprintf(stdout, "%sOp%s (%s): %s\n", K_MAG, K_NRM, token_type_to_str(tree->type), token_type_to_str(tree->attr.op));
        break;
      case K_CONST:
        fprintf(stdout, "%sLiteral const%s: %s\n", K_CYN, K_NRM, tree->attr.name);
        break;
      case K_ICONST:
        fprintf(stdout, "%sInteger const%s: %d\n", K_CYN, K_NRM, tree->attr.iVal);
        break;
      case K_DCONST:
        fprintf(stdout, "%sDecimal const%s: %f\n", K_CYN, K_NRM, tree->attr.dVal);
        break;
      case K_VAR:
        fprintf(stdout, "%sVariable%s (%s): %s\n", K_CYN, K_NRM, token_type_to_str(tree->attr.var.type), tree->attr.var.name);
        break;
      default:
        fprintf(stdout, "Unknown ExpNode kind\n");
        break;
      }
    } else {
      fprintf(stdout, "Unknown node kind\n");
    }

    for (int i = 0; i < MAX_CHILDREN; i++) {
      print_tree(tree->child[i]);
    }

    tree = tree->sibling;
  }

  UNINDENT;
}

char * get_line(size_t line)
{
  FILE *temp = source->file;
  char *buffer = malloc(sizeof(BUFFER_LENGTH));
  size_t i = 0;

  fseek(temp, 0, SEEK_SET);

  do {
    fgets(buffer, BUFFER_LENGTH - 1, source->file);
  } while (++i < line);

  return buffer;
}

char * file_get_contents(char *filename)
{
  char *buffer = NULL;
  long int string_size;
  size_t read_size;
  FILE *fp = fopen(filename, "r");

  if (fp) {
    fseek(fp, 0, SEEK_END);
    string_size = ftell(fp);
    rewind(fp);

    buffer = malloc(sizeof(char) * ((size_t) string_size + 1));

    read_size = fread(buffer, sizeof(char), (size_t) string_size, fp);
    buffer[string_size] = '\0';

    if ((size_t) string_size != read_size) {
      free(buffer);
      buffer = NULL;
    }

    fclose(fp);
  }

  return buffer;
}

size_t file_put_contents(char *filename, char *data)
{
  size_t string_size;
  size_t writed_size = 0;
  FILE *fp = fopen(filename, "w+");

  string_size = strlen(data);
  writed_size = fwrite(data, sizeof(char), string_size, fp);
  fclose(fp);

  return writed_size;
}
