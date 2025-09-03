#include "../inc/builtins.h"

int ft_env(t_struct *mini, char **args)
{
	int	i;

	i = 0;
	if (args[1]) // Si hay argumentos después de "env"
	{
		ft_putstr_fd("minishell: env: too many arguments\n", STDERR_FILENO);
		mini->last_exit_status = 1;
		return (1);
	}
	while (mini->envp[i])
	{
		 // Solo imprime variables que contienen '='
		if (ft_strchr(mini->envp[i], '='))
			ft_putendl_fd(mini->envp[i], STDOUT_FILENO);
		i++;
	}
	mini->last_exit_status = 0;
	return (0);
}
