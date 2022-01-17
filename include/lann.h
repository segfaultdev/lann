#ifndef __LANN_H__
#define __LANN_H__

#include <stdint.h>
#include <stdio.h>

#define LANN_MAX_SIZE  8
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
  ln_type_struct,
  ln_type_func
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
  
  ln_word_plus,
  ln_word_minus,
  ln_word_aster,
  ln_word_slash,
  ln_word_percent,
  ln_word_and,
  ln_word_or,
  ln_word_xor,
  ln_word_tilde,
  ln_word_exclam,
  
  ln_word_inc,
  ln_word_dec,
  
  ln_word_equal,
  ln_word_noteq,
  ln_word_grteq,
  ln_word_leseq,
  ln_word_great,
  ln_word_less,
  
  ln_word_bool_and,
  ln_word_bool_or,
  
  ln_word_set,
  
  ln_word_paren_l,
  ln_word_paren_r,
  
  ln_word_brack_l,
  ln_word_brack_r,
  
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
void ln_parse_expr(int pass, ln_cont_t *cont);
void ln_parse(void);

void ln_push_int(uint32_t val);
void ln_push_str(const char *str);

void ln_label(const char *name);
void ln_call(const char *name);
void ln_ret(void);

void ln_add(void);   // +
void ln_sub(void);   // -
void ln_mul(void);   // *
void ln_div(void);   // /
void ln_mod(void);   // %
void ln_and(void);   // &
void ln_or(void);    // |
void ln_xor(void);   // ^
void ln_not(void);   // ~
void ln_neg(void);   // !
void ln_equal(void); // ==
void ln_noteq(void); // !=
void ln_grteq(void); // >=
void ln_leseq(void); // <=
void ln_great(void); // >
void ln_less(void);  // <

#endif
