#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <lann.h>

static int ln_space(char c) {
  return (isspace(c) || c == ',' || c == ':' || c == ';');
}

static int ln_alnum(char c) {
  return (isalnum(c) || c == '_');
}

static int ln_isnum(const char *str) {
  char *ptr;
  strtol(str, &ptr, 0);
  
  return (*str && !(*ptr));
}

ln_word_t ln_read(void) {
  ln_word_t word;
  int index = 0;
  
  int in_string = 0;
  char c = ' ';
  
  while (c != EOF && ln_space(c)) c = fgetc(ln_input);
  int alnum = ln_alnum(c);
  
  while (c != EOF && ((!ln_space(c) && ln_alnum(c) == alnum) || in_string)) {
    if (c == '"') in_string = !in_string;
    word.data[index++] = c;
    
    c = fgetc(ln_input);
  }
  
  word.data[index] = '\0';
  
  while (c != EOF && ln_space(c)) c = fgetc(ln_input);
  ungetc(c, ln_input);
  
  if (!strcmp(word.data, "func")) word.type = ln_word_func;
  else if (!strcmp(word.data, "type")) word.type = ln_word_type;
  else if (!strcmp(word.data, "begin")) word.type = ln_word_begin;
  else if (!strcmp(word.data, "end")) word.type = ln_word_end;
  else if (!strcmp(word.data, "if")) word.type = ln_word_if;
  else if (!strcmp(word.data, "else")) word.type = ln_word_else;
  else if (!strcmp(word.data, "while")) word.type = ln_word_while;
  else if (!strcmp(word.data, "for")) word.type = ln_word_for;
  else if (!strcmp(word.data, "ptr")) word.type = ln_word_ptr;
  else if (!strcmp(word.data, "struct")) word.type = ln_word_struct;
  else if (ln_isnum(word.data)) word.type = ln_word_num;
  else if (word.data[0] == '"') word.type = ln_word_str;
  else if (alnum) word.type = ln_word_ident;
  
  return word;
}

ln_word_t ln_peek(void) {
  LANN_SAVE();
  ln_word_t word = ln_read();
  
  LANN_RESTORE();
  return word;
}

int ln_expect(int type, ln_word_t *word) {
  ln_word_t temp = ln_peek();
  if (word) *word = temp;
  
  if (temp.type == type) {
    ln_read();
    return 1;
  }
  
  return 0;
}
