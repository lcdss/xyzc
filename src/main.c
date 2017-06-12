#include <unistd.h>
#include <getopt.h>
#include "globals.h"
#include "util.h"
#include "parser.h"
#include "analyzer.h"

File *source;
bool print_tokens = false;
bool print_symbol_table = false;
bool print_syntax_tree = false;

static char *source_filename;

static void usage(int exit_code)
{
  fprintf(exit_code ? stderr : stdout,
    "\n  Usage: xyzc [ -d ] [ -h ] <input_file>\n"
    "\n\n"
    "    -tokens          print all the tokens found.\n"
    "    -symtab          print the symbol table to stdout.\n"
    "    -syntaxtree      print the syntax tree to stdout.\n"
    "    -h               print this help.\n"
    "\n"
  );

  exit(exit_code);
}

static void parse_option(int argc, char **argv)
{
  while (true) {
    int opt = getopt(argc, argv, "t::s::h");

    if (opt == -1) {
      break;
    }

    switch (opt) {
    case 't':
      if (strcmp(optarg, "okens") == 0) {
        print_tokens = true;
      } else {
        usage(1);
      }
      break;
    case 's':
      if (strcmp(optarg, "ymtab") == 0 || strcmp(optarg, "yntaxtree") == 0) {
        if (strcmp(optarg, "ymtab") == 0) {
          print_symbol_table = true;
        } else {
          print_syntax_tree = true;
        }
      } else {
        usage(1);
      }
      break;
    case 'h':
    case '?':
      usage(0);
      break;
    default:
      usage(1);
      break;
    }
  }

  if (optind != argc - 1) {
    usage(1);
  }

  source_filename = argv[optind];
}

int main(int argc, char **argv)
{
  TreeNode *syntax_tree;

  parse_option(argc, argv);

  if (strchr(source_filename, '.') == NULL) {
    strcat(source_filename, ".xyz");
  }

  FILE *source_file = fopen(source_filename, "r");

  if (source_file == NULL) {
    fprintf(stderr, "File %s not found.\n", source_filename);
    exit(1);
  }

  source = make_file(source_file, source_filename);

  if (print_tokens) {
    fprintf(stdout, "\nOutput: %s\n", source_filename);
  }

  syntax_tree = parse();

  build_symtab(syntax_tree);
  type_check(syntax_tree);

  if (print_syntax_tree) {
    print_tree(syntax_tree);
  }

  free(syntax_tree);
  fclose(source->file);

  return 0;
}
