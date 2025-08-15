#ifndef MAIN_UTILS_H
# define MAIN_UTILS_H

# include "minishell.h"

int     init_minishell(t_struct *mini, char **envp_main);
void    cleanup_minishell(t_struct *mini);
int     ft_str_is_whitespace(const char *s); 

#endif