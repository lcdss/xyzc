#ifndef XYZ_COMPILER_SYMTAB_H
  #define XYZ_COMPILER_SYMTAB_H

  #include "globals.h"

  #define SIZE 211
  #define SHIFT 4

  typedef struct LineListRec {
    size_t number;
    struct LineListRec *next;
  } *LineList;

  typedef struct SymbolListRec {
    char *name;
    TokenType type;
    LineList lines;
    unsigned int memloc;
    struct SymbolListRec *next;
  } *SymbolList;

  void st_insert(char *name, size_t line_number, TokenType type, unsigned int memloc);
  SymbolList st_lookup(char *name);
  void st_print(void);
#endif
