#ifndef XYZ_COMPILER_SCAN_H
  #define XYZ_COMPILER_SCAN_H

  #include "globals.h"
  #include "util.h"

  #define MAX_LEXEME_LENGTH 32

  char * get_line(size_t line);

  Token * read_token(void);
#endif
