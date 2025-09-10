/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_pwd.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatigarc <fatigarc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 04:38:03 by fatigarc          #+#    #+#             */
/*   Updated: 2025/09/03 09:28:20 by fatigarc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../inc/builtins.h"

static int	print_pwd_from_env(char **envp)
{
	int	i;

	i = 0;
	while (envp[i])
	{
		if (ft_strncmp(envp[i], "PWD=", 4) == 0)
		{
			ft_putendl_fd(envp[i] + 4, STDOUT_FILENO);
			return (0);
		}
		i++;
	}
	perror("minishell: pwd");
	return (1);
}

int	ft_pwd(t_struct *mini, char **args)
{
	char	*cwd;
	int		status;

	if (args[1] && args[1][0] == '-')
	{
		ft_putendl_fd(
			"minishell: pwd: options are not supported", STDERR_FILENO);
		mini->last_exit_status = 1;
		return (1);
	}
	cwd = getcwd(NULL, 0);
	if (!cwd)
	{
		status = print_pwd_from_env(mini->envp);
		mini->last_exit_status = status;
		return (status);
	}
	ft_putendl_fd(cwd, STDOUT_FILENO);
	free(cwd);
	mini->last_exit_status = 0;
	return (0);
}
