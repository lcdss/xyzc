#ifndef XYZ_COMPILER_ANALYZER_H
  #define XYZ_COMPILER_ANALYZER_H

  #include "globals.h"
  #include "symtab.h"
  #include "util.h"

  void build_symtab(TreeNode *syntax_tree);

  void type_check(TreeNode * syntax_tree);
#endif
