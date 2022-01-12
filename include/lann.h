#ifndef __LANN_H__
#define __LANN_H__

#include <stdint.h>
#include <stdio.h>

#define LANN_MAX_SIZE  sizeof(size_t)
#define LANN_IDENT_LEN 40

#define LANN_SAVE()    size_t pos = ftell(ln_input)
#define LANN_RESTORE() fseek(ln_input, pos, SEEK_SET)

typedef struct ln_word_t ln_word_t;

typedef struct ln_type_t ln_type_t;
typedef struct ln_decl_t ln_decl_t;

typedef struct ln_cont_t ln_cont_t;

enum {
  ln_type_int,
  ln_type_ptr,
  ln_type_struct
};

enum {
  ln_word_func,
  ln_word_type,
  ln_word_begin,
  ln_word_end,
  ln_word_if,
  ln_word_else,
  ln_word_while,
  ln_word_for,
  ln_word_ptr,
  ln_word_struct,
  
  ln_word_ident,
  ln_word_num,
  ln_word_str
};

// word/token
struct ln_word_t {
  char data[LANN_IDENT_LEN];
  int type;
};

struct ln_type_t {
  int type, size;
  
  // ln_type_int
  int has_sign;
  
  // ln_type_ptr
  ln_type_t *ptr;
  
  // ln_type_struct
  ln_decl_t *childs;
  int count;
};

// type and variable declarations
struct ln_decl_t {
  ln_type_t type;
  char name[LANN_IDENT_LEN];
  
  // only in variables
  void *data;
};

// context
struct ln_cont_t {
  ln_cont_t *parent;
  
  ln_decl_t *vars;
  int count;
};

extern FILE *ln_input;
extern FILE *ln_output;

extern ln_cont_t ln_global;

extern ln_decl_t *ln_types;
extern int ln_type_count;

ln_word_t ln_read(void); // read a word and consume it
ln_word_t ln_peek(void); // read a word but leave it

int ln_expect(int type, ln_word_t *word); // read a word, but only consume it if it's of the specified type, in which case it will also return 1

int  ln_parse_type(ln_type_t *ptr);
int  ln_parse_expr(int pass, ln_cont_t *cont);
void ln_parse(void);

#endif
