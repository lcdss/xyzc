#include "symtab.h"
#include "util.h"

static SymbolList hashTable[SIZE];

static int hash(char *key)
{
  int temp = 0;

  for (int i = 0; key[i] != '\0'; i++) {
    temp = ((temp << SHIFT) + key[i]) % SIZE;
  }

  return temp;
}

void st_insert(char *name, size_t line_number, TokenType type, unsigned int memloc)
{
  int hash_key = hash(name);
  SymbolList symbol = hashTable[hash_key];

  while (symbol != NULL && strcmp(name, symbol->name) != 0) {
    symbol = symbol->next;
  }

  if (symbol == NULL) {
    symbol = (SymbolList) malloc(sizeof(struct SymbolListRec));

    symbol->name = name;
    symbol->type = type;
    symbol->memloc = memloc;
    symbol->lines = (LineList) malloc(sizeof(struct LineListRec));

    symbol->lines->number = line_number;
    symbol->lines->next = NULL;
    symbol->next = hashTable[hash_key];
    hashTable[hash_key] = symbol;
  } else {
    LineList line = symbol->lines;

    while (line->next != NULL) {
      line = line->next;
    }

    line->next = (LineList) malloc(sizeof(struct LineListRec));

    line->next->number = line_number;
    line->next->next = NULL;
  }
}

SymbolList st_lookup(char *name)
{
  int hash_key = hash(name);

  SymbolList symbol = hashTable[hash_key];

  while (symbol != NULL && strcmp(name, symbol->name) != 0) {
    symbol = symbol->next;
  }

  return symbol;
}

void st_print(void)
{
  fprintf(stdout, "\n\n");
  fprintf(stdout, "%s           Symbol Table          %s\n", K_CYN, K_NRM);
  fprintf(stdout, "%sVariable Name  Type  Line Numbers%s\n", K_MAG, K_NRM);
  fprintf(stdout, "%s=============  ====  ============%s\n", K_YEL, K_NRM);

  for (int i = 0; i < SIZE; ++i) {
    if (hashTable[i] != NULL) {
      SymbolList symbol = hashTable[i];

      while (symbol != NULL) {
        LineList line = symbol->lines;

        fprintf(stdout, "%s%-14s%s ", K_GRN, symbol->name, K_NRM);
        fprintf(stdout, "%s%-5s%s ", K_RED, token_type_to_str(symbol->type), K_NRM);

        while (line != NULL) {
          fprintf(stdout, "%2lu ", line->number);
          line = line->next;
        }

        fprintf(stdout, "\n");

        symbol = symbol->next;
      }
    }
  }
}
