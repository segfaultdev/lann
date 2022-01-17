#include <stdlib.h>
#include <string.h>
#include <lann.h>

// TODO: pass type with expressions

ln_cont_t ln_global = (ln_cont_t){NULL, NULL, 0};

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
    
    if (strlen(ident.data) >= 6) {
      char *ptr = ident.data;
      
      type->type = ln_type_int;
      type->has_sign = 1;
      
      if (*ptr == 'u') {
        type->has_sign = 0;
        ptr++;
      }
      
      if (!memcmp(ptr, "int", 3)) {
        char *tmp;
        ptr += 3;
        
        type->size = strtol(ptr, &tmp, 10) >> 3;
        if (ptr == tmp) type->size = LANN_MAX_SIZE;
        
        ptr = tmp;
        
        if (!strcmp(ptr, "_t")) {
          return 1;
        }
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

// const., decl, lambda, ()
void ln_parse_expr_0(int pass, ln_cont_t *cont) {
  printf("lann: ln_parse_expr_0()\n");
  
  ln_word_t word;
  ln_type_t type;
  
  if (ln_expect(ln_word_ident, &word)) {
    if (ln_expect(ln_word_paren_l, NULL)) {
      printf("lann: ln_parse_expr_0() -> ident(function call)\n");
      
      while (!ln_expect(ln_word_paren_r, NULL)) {
        ln_parse_expr(pass, cont);
      }
      
      // TODO: stuff
    } else {
      printf("lann: ln_parse_expr_0() -> ident(variable)\n");
      // TODO: stuff
    }
  } else if (ln_expect(ln_word_begin, NULL)) {
    printf("lann: ln_parse_expr_0() -> begin(block)\n");
    
    while (!ln_expect(ln_word_end, NULL)) {
      ln_parse_expr(pass, cont);
    }
  } else if (ln_expect(ln_word_paren_l, NULL)) {
    printf("lann: ln_parse_expr_0() -> parentheses\n");
    ln_parse_expr(pass, cont);
    
    if (!ln_expect(ln_word_paren_r, NULL)) {
      printf("lann: missing closing parentheses\n");
      exit(1);
    }
  } else if (ln_parse_type(&type)) {
    printf("lann: ln_parse_expr_0() -> type(declaration)\n");
    
    if (!ln_expect(ln_word_ident, &word)) {
      printf("lann: invalid identifier in local declaration: '%s'\n", word.data);
      exit(1);
    }
    
    // TODO: add variable
    
    if (ln_expect(ln_word_set, NULL)) {
      ln_parse_expr(pass, cont);
      // TODO: do something with value
    }
  } else if (ln_expect(ln_word_num, &word)) {
    printf("lann: ln_parse_expr_0() -> number\n");
    // TODO: stuff
  }
}

// ++, --, -, ~, !, []
void ln_parse_expr_1(int pass, ln_cont_t *cont) {
  printf("lann: ln_parse_expr_1()\n");
  ln_parse_expr_0(pass, cont);
}

// &, |, ^
void ln_parse_expr_2(int pass, ln_cont_t *cont) {
  printf("lann: ln_parse_expr_2()\n");
  ln_parse_expr_1(pass, cont);
  
  ln_word_t word;
  
  while (ln_expect(ln_word_and, &word) || ln_expect(ln_word_or, &word) || ln_expect(ln_word_xor, &word)) {
    ln_parse_expr_1(pass, cont);
    
    if (pass) {
      if (word.type == ln_word_and) ln_and();
      else if (word.type == ln_word_or) ln_or();
      else if (word.type == ln_word_xor) ln_xor();
    }
  }
}

// *, /, %
void ln_parse_expr_3(int pass, ln_cont_t *cont) {
  printf("lann: ln_parse_expr_3()\n");
  ln_parse_expr_2(pass, cont);
  
  ln_word_t word;
  
  while (ln_expect(ln_word_aster, &word) || ln_expect(ln_word_slash, &word) || ln_expect(ln_word_percent, &word)) {
    ln_parse_expr_2(pass, cont);
    
    if (pass) {
      if (word.type == ln_word_aster) ln_mul();
      else if (word.type == ln_word_slash) ln_div();
      else if (word.type == ln_word_percent) ln_mod();
    }
  }
}

// +, -
void ln_parse_expr_4(int pass, ln_cont_t *cont) {
  printf("lann: ln_parse_expr_4()\n");
  ln_parse_expr_3(pass, cont);
  
  ln_word_t word;
  
  while (ln_expect(ln_word_plus, &word) || ln_expect(ln_word_minus, &word)) {
    ln_parse_expr_3(pass, cont);
    
    if (pass) {
      if (word.type == ln_word_plus) ln_add();
      else if (word.type == ln_word_minus) ln_sub();
    }
  }
}

// ==, !=, >=, <=, >, <
void ln_parse_expr_5(int pass, ln_cont_t *cont) {
  printf("lann: ln_parse_expr_5()\n");
  ln_parse_expr_4(pass, cont);
  
  while (ln_expect(ln_word_equal, NULL) || ln_expect(ln_word_noteq, NULL) || ln_expect(ln_word_grteq, NULL) || ln_expect(ln_word_leseq, NULL) || ln_expect(ln_word_great, NULL) || ln_expect(ln_word_less, NULL)) {
    ln_parse_expr_4(pass, cont); // TODO: actually get the token and do stuff
  }
}

// &&, ||
void ln_parse_expr_6(int pass, ln_cont_t *cont) {
  printf("lann: ln_parse_expr_6()\n");
  ln_parse_expr_3(pass, cont);
  
  ln_word_t word;
  
  while (ln_expect(ln_word_bool_and, &word) || ln_expect(ln_word_bool_or, &word)) {
    ln_parse_expr_3(pass, cont);
    
    if (pass) {
      if (word.type == ln_word_plus) ln_add();
      else if (word.type == ln_word_minus) ln_sub();
    }
  }
}

void ln_parse_expr(int pass, ln_cont_t *cont) {
  ln_parse_expr_6(pass, cont);
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
      
      for (int i = 0; i < ln_global.count; i++) {
        if (!strcmp(ln_global.vars[i].name, ident.data)) {
          printf("lann: function already exists: '%s'\n", ident.data);
          exit(1);
        }
      }
      
      ln_global.vars = realloc(ln_global.vars, (ln_global.count + 1) * sizeof(ln_decl_t));
      
      ln_global.vars[ln_global.count].type.type = ln_type_func;
      ln_global.vars[ln_global.count].type.size = LANN_MAX_SIZE;
      
      strcpy(ln_global.vars[ln_global.count++].name, ident.data);
      
      ln_cont_t cont;
      cont.parent = &ln_global;
      
      cont.vars = NULL;
      cont.count = 0;
      
      LANN_SAVE();
      ln_parse_expr(0, &cont);
      
      LANN_RESTORE();
      ln_parse_expr(1, &cont);
    } else {
      ln_word_t word = ln_peek();
      if (!strlen(word.data)) break;
      
      printf("lann: invalid word, expected 'type', 'func' or type name: '%s'\n", word.data);
      exit(1);
    }
  }
}
