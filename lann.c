#include <string.h>
#include <stdio.h>
#include <lann.h>

FILE *ln_input;
FILE *ln_output;

int main(int argc, const char **argv) {
  ln_input = stdin;
  ln_output = stdout;
  
  for (int i = 1; i < argc; i++) {
    if (!strcmp(argv[i], "-r") || !strcmp(argv[i], "--run")) {
      // TODO
    }
  }
  
  ln_parse();
  return 0;
}
