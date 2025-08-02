#ifndef EXPANDER_H
# define EXPANDER_H

# include "parser.h"
# include "token.h"

char  *expand_single_string(char *original_str, char **envp, int last_exit_status);
void expand_variables(t_command *commands, char **envp, int *last_exit_status);


# endif