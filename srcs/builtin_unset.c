#include "../inc/builtins.h"

static void remove_var_from_envp(char **envp, char *name_var)
{
	int	i;
	int	j;

	i = 0;
	while (envp[i])
	{
		j = 0;
		while (envp[i][j] && envp[i][j] != '=')
			j++;
		if (ft_strncmp(envp[i], name_var, j) == 0 && envp[i][j] == '=')
		{
			free(envp[i]);
			while (envp[i + 1])
			{
				envp[i] = envp[i + 1];
				i++;
			}
			envp[i] = NULL;
			return;
		}
		i++;
	}
}

int ft_unset(t_struct *mini, char **args)
{
    int	i;

	i = 1;
	if (!args[1])
	{
		mini->last_exit_status = 0;
		return (0);
	}
	while (args[i])
	{
		if (args[i][0] == '-') // ← Detecta opción
		{
			ft_putstr_fd("minishell: unset: ", STDERR_FILENO);
			ft_putstr_fd(args[i], STDERR_FILENO);
			ft_putendl_fd(": options are not supported", STDERR_FILENO);
			mini->last_exit_status = 1;
		}
		else
			remove_var_from_envp(mini->envp, args[i]);
		i++;
	}
    // Si no hubo errores, exit_status debe ser 0
	if (mini->last_exit_status != 1)
		mini->last_exit_status = 0;
	return (mini->last_exit_status);
}
