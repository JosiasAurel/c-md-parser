#include <stdio.h>
#include <stdlib.h>

#define true 1
#define false 0

enum TokenKind {
  TOKEN_BOLD,
  TOKEN_HASH,
  TOKEN_ITALIC,
  TOKEN_LIST,
  TOKEN_STRIKE
};

typedef struct {
  char mod;
  int idx;
  int count; // used only in the case of a #
} Modifier;

typedef struct {
  int count;
  Modifier* mod[100];
  int idx;
  int peek_idx;
} Stack;

void preprocess(FILE*, Stack*);
int modifier_in_stack(Stack*, char, int);
const char* get_tag(char, int, int);
void write_tag(FILE* file, const char* tag);
void htmlize(char*, char*, Stack*);

void push(Stack* stack, Modifier* item) {
  Modifier* item_ptr = (Modifier*)malloc(sizeof(Modifier));
  item_ptr->count = item->count;
  item_ptr->idx = item->idx;
  item_ptr->mod = item->mod;
  stack->mod[stack->idx] = item_ptr;
  stack->idx++;
  stack->peek_idx++;
}

Modifier peek(Stack* stack) {
  Modifier result;
  result.count = stack->mod[stack->idx - 1]->count;
  result.mod = stack->mod[stack->idx -1]->mod;
  result.idx = stack->mod[stack->idx - 1]->idx;
  return result;
}

Modifier pop(Stack* stack) {
  stack->idx--;
  Modifier result;
  result.count = stack->mod[stack->idx]->count;
  result.mod = stack->mod[stack->idx]->mod;
  result.idx = stack->mod[stack->idx]->idx;
  free(stack->mod[stack->idx]);
  return result;
}



void preprocess(FILE * file, Stack* stack) {

  int idx = 0;
  char c = getc(file);
  // this variable is used to check whether the previous
  // character was actually a newline
  // most useful for the # and - 
  // since we only want to render a heading or list on a new line
  char prev_char; 
  int render_block = true;
  while (c != EOF) {
    if (c == '#') {
      if (prev_char != '\n') render_block = false;
      Modifier mod = {.idx = idx, .mod = '#', .count = 1};
      if (stack->idx > 0) {
        Modifier item = pop(stack);
        if (item.mod == '#') item.count++;
        push(stack, &item);
      } else push(stack, &mod);
    } else if (c == '-') {
      Modifier mod = {.idx = idx, .mod = '-'};
      push(stack, &mod);
    } else if (c == '\n') {
      if (stack->idx > 0) {
        Modifier mod = peek(stack);
        if (mod.mod == '#' || mod.mod == '-') {
          Modifier a = peek(stack);
          int is_space = prev_char == ' ';
          if (render_block || a.idx == 0) {
            render_block = true;
            pop(stack);
          }
        }
      }
    } else if (c == '*' || c == '_' || c == '~') {
        if (stack->idx > 0) {
          Modifier prev = peek(stack);
          if (prev.mod == c) {
            pop(stack);
          } else {
            Modifier mod = {.idx = idx, .mod = c};
            push(stack, &mod);
          }
        } else {
          Modifier mod = {.idx = idx, .mod = c};
          push(stack, &mod);
        }
      }
      c = getc(file);
      if (c != '#' && c != '-') prev_char = c;
      idx++;
  }

}

void htmlize(char* input, char* output, Stack* stack) {
  FILE* in = fopen(input, "r");
  FILE* out = fopen(output, "w");

  enum TokenKind tokens[10];
  int idx = 0;
  int char_idx = 0;
  int count = 1; // used for the hashes
  char c = getc(in);

  while (c != EOF) {
    // printf("Next char = %c \n", c);
    
    if (modifier_in_stack(stack, c, char_idx)) {
      putc(c, out);
      c = getc(in);
      char_idx++;
      continue;
    }
    

    // printf("Char = %c \n", c);
    switch (c) {
      case '#': {
        tokens[idx] = TOKEN_HASH;
        count = 1;
        while (1) {
          c = getc(in);
          char_idx++;
          if (c != '#') break;
          count++; 
        }
        const char* result = get_tag('#', true, count);
        write_tag(out, result);
        idx++;
        // printf("Stopped at char = %c ", c);
        continue;
      }
      case '-': {
        tokens[idx] = TOKEN_LIST;
        const char* result = get_tag('-', true, 0);
        // printf("Tag = %s \n", result);
        write_tag(out, result);
        idx++;
        break;
      }
      case '*': {
        tokens[idx] = TOKEN_BOLD;
        int opener = !(tokens[idx-1] == TOKEN_BOLD);
        if (opener) {
          idx++;
        } else idx--;
        const char* result = get_tag('*', opener, 0);
        write_tag(out, result);
        break;
      }
      case '_': {
        tokens[idx] = TOKEN_ITALIC;
        int opener = !(tokens[idx-1] == TOKEN_ITALIC);
        if (opener) {
          idx++;
        } else idx--;
        const char* result = get_tag('_', opener, 0);
        write_tag(out, result);
        break;
      }
      case '~': {
        tokens[idx] = TOKEN_STRIKE; 
        int opener = !(tokens[idx-1] == TOKEN_STRIKE);
        if (opener) {
          idx++;
        } else idx--;
        const char* result = get_tag('~', opener, 0);
        write_tag(out, result);
      }
      case '\n': {
        // puts("GOT HERE");
        enum TokenKind prev = tokens[idx-1];
        char t;
        if (prev != TOKEN_LIST && prev != TOKEN_HASH) {
          // putc(c, out);
          write_tag(out, "<br />");
        } else {
          if (prev == TOKEN_HASH) t = '#';
          if (prev == TOKEN_LIST) t = '-';
        
          const char* result = get_tag(t, false, count);
          // printf("Tag = %s \n", result);
          write_tag(out, result);
          putc(c, out);
          idx--;
        }
        break;
      }
      default:
        putc(c, out);
        break;
    }
    // printf("Idx = %d\n", idx);
    c = getc(in);
    char_idx++;
  }

  fclose(in);
  fclose(out);
}

int modifier_in_stack(Stack* stack, char c, int char_idx) {
  for (int i = 0; i < stack->idx; i++) {
    Modifier* item = stack->mod[i];
    if (item->mod == c && item->idx == char_idx) return true;
  }
  return false;
}

void write_tag(FILE* file, const char* tag) {
  int idx = 0;
  // printf("Writing tag %s \n", tag);
  while (true) {
    putc(tag[idx], file);
    if (tag[idx+1] == '>') {
      putc('>', file);
      break;
    }
    idx++;
  }
}

const char* get_tag(char c, int opener, int count) {
  switch (c) {
    // char result[6];
    char* result;
    case '#': {
      if (opener) {
        sprintf(result, "<h%d>", count);
      } else sprintf(result, "</h%d>", count);
      return result;
      // return "<h1>";
    }
    case '*': {
      if (opener) return "<b>";
      return "</b>";
    }
    case '_': {
      if (opener) return "<i>";
      return "</i>";
    }
    case '-': {
      if (opener) return "<li>";
      return "</li>";
    }
    case '~': {
      if (opener) return "<strike>";
      return "</strike>";
    }
    default:
      return (char[2]){c, '\0'};
  }
}

int main(int argc, char* argv[]) {

  if (argc != 3) puts("Not enough or too many arguments");

  char* input_file = argv[1];
  char* output_file = argv[2];

  Stack stack = {.idx = 0, .peek_idx = 0};
  FILE* file = fopen(input_file, "r");
  preprocess(file, &stack);

  fclose(file);
  htmlize(input_file, output_file, &stack);
}