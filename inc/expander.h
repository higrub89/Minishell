#ifndef EXPANDER_H
# define EXPANDER_H

# include "parser.h"
# include "token.h"
# include "command.h"

# define INITIAL_SB_CAPACITY 128

typedef struct s_string_builder
{
  char *buffer;
  size_t length;
  size_t capacity;
} t_string_builder;

char  *expand_single_string(char *original_str, t_struct *mini);
void expand_variables(t_command *commands, t_struct *mini);


# endif