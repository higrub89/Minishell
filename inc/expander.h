#ifndef EXPANDER_H
# define EXPANDER_H

# include "minishell.h"

# define INITIAL_SB_CAPACITY 128

typedef struct s_string_builder
{
  char *buffer;
  size_t length;
  size_t capacity;
} t_string_builder;

void expand_variables(t_command *cmd_list, t_struct *mini);
char  *expand_string(char *original_str, t_struct *mini);

# endif