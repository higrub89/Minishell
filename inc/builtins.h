
#ifndef BUILTINS_H
# define BUILTINS_H

# include "minishell.h"
# include "executor.h"
# include "env_utils.h"
# include <sys/stat.h>

// Prototipos para los builtins
int     ft_unset(t_struct *mini, char **args);
int     ft_pwd(t_struct *mini); // Se cambia para devolver int y fijar exit status
int     ft_env(t_struct *mini); // Se cambia para devolver int y fijar exit status
int     ft_cd(t_struct *mini, char **args);
int     ft_export(t_struct *mini, char **args);
int     ft_echo(char **args); // No necesita t_struct si no afecta el estado
int     ft_exit(t_struct *mini, char **args); // Para futuras implementaciones

#endif

