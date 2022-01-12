#include <stdlib.h>
#include <string.h>
#include <lann.h>

ln_decl_t *ln_types = NULL;
int ln_type_count = 0;

int ln_parse_type(ln_type_t *type) {
  printf("lann: ln_parse_type()\n");
  
  LANN_SAVE();
  ln_word_t ident;
  
  if (ln_expect(ln_word_ptr, NULL)) {
    printf("lann: ln_parse_type() -> ptr\n");
    
    type->type = ln_type_ptr;
    type->size = LANN_MAX_SIZE;
    
    type->ptr = malloc(sizeof(ln_type_t));
    
    if (!ln_parse_type(type->ptr)) {
      LANN_RESTORE();
      return 0;
    }
  } else if (ln_expect(ln_word_struct, NULL)) {
    printf("lann: ln_parse_type() -> struct\n");
    
    while (!ln_expect(ln_word_end, NULL)) {
      ln_type_t child_type;
      
      if (!ln_parse_type(&child_type)) {
        printf("lann: invalid type\n");
        exit(1);
      }
      
      if (!ln_expect(ln_word_ident, &ident)) {
        printf("lann: invalid name for struct member: '%s'\n", ident.data);
        exit(1);
      }
    }
  } else if (ln_expect(ln_word_ident, &ident)) {
    printf("lann: ln_parse_type() -> ident(custom type)\n");
    
    for (int i = 0; i < ln_type_count; i++) {
      if (!strcmp(ln_types[i].name, ident.data)) {
        *type = ln_types[i].type;
        return 1;
      }
    }
    
    printf("lann: type does not exist: '%s'\n", ident.data);
    exit(1);
  } else {
    LANN_RESTORE();
    return 0;
  }
  
  return 1;
}

int ln_parse_expr(int pass, ln_cont_t *cont) {
  printf("lann: ln_parse_expr()\n");
  // TODO: expressions
  
  return 1;
}

void ln_parse(void) {
  while (!feof(ln_input)) {
    printf("lann: ln_parse()\n");
    
    if (ln_expect(ln_word_type, NULL)) {
      printf("lann: ln_parse() -> type\n");
      
      ln_word_t ident;
      ln_type_t type;
      
      if (!ln_expect(ln_word_ident, &ident)) {
        printf("lann: invalid name for type: '%s'\n", ident.data);
        exit(1);
      }
      
      for (int i = 0; i < ln_type_count; i++) {
        if (!strcmp(ln_types[i].name, ident.data)) {
          printf("lann: type already exists: '%s'\n", ident.data);
          exit(1);
        }
      }
      
      if (!ln_parse_type(&type)) {
        printf("lann: invalid type\n");
        exit(1);
      }
      
      ln_types = realloc(ln_types, (ln_type_count + 1) * sizeof(ln_decl_t));
      
      ln_types[ln_type_count].type = type;
      strcpy(ln_types[ln_type_count++].name, ident.data);
    } else if (ln_expect(ln_word_func, NULL)) {
      printf("lann: ln_parse() -> func\n");
      ln_word_t ident;
      
      if (!ln_expect(ln_word_ident, &ident)) {
        printf("lann: invalid name for function: '%s'\n", ident.data);
        exit(1);
      }
      
      // TODO: check if function exists
      
      if (!ln_parse_expr(1, &ln_global)) {
        printf("lann: invalid expression\n");
        exit(1);
      }
    }
  }
}
