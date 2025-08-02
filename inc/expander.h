#ifndef EXPANDER_H
# define EXPANDER_H

# include "parser.h"
# include "token.h"
# include "command.h"

# define INITIAL_SB_CAPACITY 32 

typedef struct s_string_builder
{
  char *buffer;
  size_t length;
  size_t capacity;
} t_string_builder;

char  *expand_single_string(char *original_str, char **envp, int last_exit_status);
void expand_variables(t_command *commands, char **envp, int *last_exit_status);


# endif