/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_env.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatigarc <fatigarc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 04:38:20 by fatigarc          #+#    #+#             */
/*   Updated: 2025/09/04 04:48:38 by fatigarc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../inc/builtins.h"

int	ft_env(t_struct *mini, char **args)
{
	int	i;

	i = 0;
	if (!mini->envp[0])
	{
		ft_putendl_fd("minishell: env: No such file or directory",
			STDOUT_FILENO);
		return (mini->last_exit_status = 0, 0);
	}
	if (args[1])
	{
		ft_putstr_fd("minishell: env: too many arguments\n", STDERR_FILENO);
		mini->last_exit_status = 1;
		return (1);
	}
	while (mini->envp[i])
	{
		if (ft_strchr(mini->envp[i], '='))
			ft_putendl_fd(mini->envp[i], STDOUT_FILENO);
		i++;
	}
	mini->last_exit_status = 0;
	return (0);
}
