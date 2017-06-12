#include <ctype.h>
#include "lexer.h"

typedef enum {
  S_START, S_INID, S_INLNUM, S_INDNUM, S_INSTRING,
  S_INVARIABLE, S_INLINECOMMENT, S_INMULTILINECOMMENT, S_DONE
} StateType;

static char buffer[BUFFER_LENGTH];
static size_t buffer_size = 0;

static struct {
  const char *name;
  TokenType type;
} keywords[MAX_RESERVED_WORDS] = {
  {"main", T_MAIN}, {"if", T_IF}, {"then", T_THEN}, {"else", T_ELSE},
  {"for", T_FOR}, {"gets", T_GETS}, {"puts", T_PUTS}, {"int", T_INT},
  {"char", T_CHAR}, {"dec", T_DECIMAL}
};

static TokenType reserved_lookup(char *str)
{
  for (int i = 0; i < MAX_RESERVED_WORDS; i++) {
    if (!strcmp(str, keywords[i].name)) {
      return keywords[i].type;
    }
  }

  return T_ERROR;
}

static void lex_error(const char *message, ...)
{
  va_list args;

  va_start(args, message);

  fprintf(stderr, "\n%s>>> Lexical error:%s ", K_RED, K_NRM);
  vfprintf(stderr, message, args);
  fprintf(stderr, " at %lu:%lu", source->line, source->column);
  fprintf(stderr, "\n%s", buffer);

  for (size_t i = 0; i < source->column; i++) {
    printf(" ");
  }

  printf("%s^%s\n", K_CYN, K_NRM);

  va_end(args);

  exit(-1);
}

static char read_char(void)
{
  if (source->column >= buffer_size) {
    source->line++;

    if (fgets(buffer, BUFFER_LENGTH - 1, source->file)) {
      if (print_tokens) {
        fprintf(stdout, "%4lu: %s\n", source->line, str_trim(buffer));
      }

      buffer_size = strlen(buffer);
      source->column = 0;

      return buffer[source->column++];
    }

    return EOF;
  }

  return buffer[source->column++];
}

static void unread_char(void)
{
  source->column--;
}

static char peek_char(void)
{
  char c = read_char();
  unread_char();

  return c;
}

static bool is_next(char n)
{
  return peek_char() == n;
}

static bool is_prev(int n)
{
  return n == buffer[source->column - 2];
}

Token * read_token(void)
{
  char lexeme[MAX_LEXEME_LENGTH + 1];
  unsigned int lexeme_index = 0;
  Token *token = make_token();
  StateType state = S_START;

  while (state != S_DONE) {
    bool save = true;
    char current_char = read_char();

    switch (state) {
    case S_START:
      if (isdigit(current_char)) {
        state = S_INLNUM;
      } else if (islower(current_char)) {
        state = S_INID;
      } else if (current_char == '&') {
        if (is_next('&')) {
          state = S_DONE;
          token->type = T_BOOLEAN_AND;
          source->column++;
          lexeme[lexeme_index++] = current_char;
          lexeme[lexeme_index++] = '&';
          save = false;
          break;
        }
        state = S_INVARIABLE;
      } else if (current_char == '"') {
        state = S_INSTRING;
      } else if (isspace(current_char)) {
        save = false;
      } else {
        state = S_DONE;

        switch (current_char) {
        case EOF:
          token->type = T_EOF;
          save = false;
          break;
        case '=':
          if (is_next('=')) {
            source->column++;
            token->type = T_IS_EQUAL;
            lexeme[lexeme_index++] = current_char;
            lexeme[lexeme_index++] = '=';
            save = false;
            break;
          }
          token->type = T_ASSIGN_EQUAL;
          break;
        case '<':
          if (is_next('=')) {
            source->column++;
            token->type = T_IS_SMALLER_OR_EQUAL;
            lexeme[lexeme_index++] = current_char;
            lexeme[lexeme_index++] = '=';
            save = false;
            break;
          }
          token->type = T_IS_SMALLER;
          break;
        case '>':
          if (is_next('=')) {
            source->column++;
            token->type = T_IS_GREATER_OR_EQUAL;
            lexeme[lexeme_index++] = current_char;
            lexeme[lexeme_index++] = '=';
            save = false;
            break;
          }
          token->type = T_IS_GREATER;
          break;
        case '!':
          if (is_next('=')) {
            source->column++;
            token->type = T_IS_NOT_EQUAL;
            lexeme[lexeme_index++] = current_char;
            lexeme[lexeme_index++] = '=';
            save = false;
            break;
          }
          lex_error("Expected the character '=', but '%c' was given", current_char);
          break;
        case '+':
          if (is_next('+')) {
            source->column++;
            token->type = T_INC;
            lexeme[lexeme_index++] = current_char;
            lexeme[lexeme_index++] = '+';
            save = false;
            break;
          }
          token->type = T_ARITH_PLUS;
          break;
        case '-':
          if (is_next('-')) {
            source->column++;
            token->type = T_DEC;
            lexeme[lexeme_index++] = current_char;
            lexeme[lexeme_index++] = '-';
            save = false;
            break;
          }
          token->type = T_ARITH_MINUS;
          break;
        case '|':
          if (is_next('|')) {
            source->column++;
            token->type = T_BOOLEAN_OR;
            lexeme[lexeme_index++] = current_char;
            lexeme[lexeme_index++] = '|';
            save = false;
            break;
          }
          lex_error("Expected a '|' but, '%c' was given", read_char());
          break;
        case '*': token->type = T_ARITH_MUL; break;
        case '/':
          if (is_next('/')) {
            save = false;
            state = S_INLINECOMMENT;
            break;
          } else if (is_next('*')) {
            save = false;
            state = S_INMULTILINECOMMENT;
          }
          token->type = T_ARITH_DIV;
          break;
        case '(': token->type = T_PAREN_OPEN; break;
        case ')': token->type = T_PAREN_CLOSE; break;
        case '{': token->type = T_CURLY_OPEN; break;
        case '}': token->type = T_CURLY_CLOSE; break;
        case ';': token->type = T_SEMICOLON; break;
        case ',': token->type = T_COMMA; break;
        case '.': token->type = T_DOT; break;
        default:
          lex_error("Unknown character '%c'", current_char);
          break;
        }
      }
      break;
    case S_INID:
      if (!islower(current_char)) {
        unread_char();
        save = false;
        state = S_DONE;
        token->type = T_ID;
      }
      break;
    case S_INVARIABLE:
      if (is_prev('&') && !islower(current_char)) {
        lex_error("Expected a lowercase letter, but '%c' was given", current_char);
      } else if (!isalnum(current_char)) {
        unread_char();
        save = false;
        state = S_DONE;
        token->type = T_VARIABLE;
      }
      break;
    case S_INLINECOMMENT:
      if (current_char == '\n') {
        state = S_START;
      }
      save = false;
      break;
    case S_INMULTILINECOMMENT:
      if (is_prev('*') && current_char == '/') {
        state = S_START;
      } else if (current_char == EOF) {
        lex_error("You forget to close the multiline comment.");
      }
      save = false;
      break;
    case S_INLNUM:
      if (current_char == '.') {
        state = S_INDNUM;
      } else if (!isdigit(current_char)) {
        unread_char();
        save = false;
        state = S_DONE;
        token->type = T_LNUMBER;
      }
      break;
    case S_INDNUM:
      if (!isdigit(current_char)) {
        unread_char();
        save = false;
        state = S_DONE;
        token->type = T_DNUMBER;
      }
      break;
    case S_INSTRING:
      if (current_char == '"') {
        state = S_DONE;
        token->type = T_STRING;
      } else if (current_char == EOF) {
        lex_error("Undetermined string literal");
      }
      break;
    case S_DONE:
    default:
      lex_error("Internal lexer error: state = %i", state);
      break;
    }

    if (save && lexeme_index <= MAX_LEXEME_LENGTH) {
      lexeme[lexeme_index++] = current_char;
    } else if (lexeme_index > MAX_LEXEME_LENGTH) {
      lex_error("Identifier should not be longer than %i characters", MAX_LEXEME_LENGTH);
    }

    if (state == S_DONE) {
      lexeme[lexeme_index] = '\0';

      if (token->type == T_ID) {
        token->type = reserved_lookup(lexeme);

        if (token->type == T_ERROR) {
          lex_error("Invalid keyword \"%s\"", lexeme);
        }
      }

      token->slen = strlen(lexeme);
      token->sval = str_clone(lexeme);
      token->line = source->line;
      token->column = source->column;
      token->file = source->file;
    }
  }

  if (print_tokens) {
    fprintf(stdout, "\t%s\n", token_to_str(token));
  }

  return token;
}
