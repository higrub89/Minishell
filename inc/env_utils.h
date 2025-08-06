#ifndef ENV_UTILS_H
# define ENV_UTILS_H

# include "minishell.h"

char **ft_copy_str_array(char **arr);
void	free_str_array(char **arr);
char	*get_env_value(const char *name, char **envp);
int	ft_setenv_var(t_struct *mini, const char *key, const char *value);

#endif