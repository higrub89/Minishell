/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_unset.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatigarc <fatigarc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 04:38:33 by fatigarc          #+#    #+#             */
/*   Updated: 2025/09/03 11:18:09 by fatigarc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../inc/builtins.h"

static void	remove_var_from_envp(char **envp, char *name_var)
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
			return ;
		}
		i++;
	}
}

static void	unset_option_error(t_struct *mini, char *arg)
{
	ft_putstr_fd("minishell: unset: ", STDERR_FILENO);
	ft_putstr_fd(arg, STDERR_FILENO);
	ft_putendl_fd(": options are not supported", STDERR_FILENO);
	mini->last_exit_status = 1;
}

static void	unset_path(t_struct *mini)
{
	free_str_array(mini->envp);
	mini->envp = malloc(sizeof(char *));
	if (!mini->envp)
	{
		mini->last_exit_status = 1;
		return ;
	}
	mini->envp[0] = NULL;
}

int	ft_unset(t_struct *mini, char **args)
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
		if (args[i][0] == '-')
			unset_option_error(mini, args[i]);
		else if (ft_strcmp(args[i], "PATH") == 0)
			unset_path(mini);
		else
		{
			remove_var_from_envp(mini->envp, args[i]);
			mini->export_list = remove_from_export_list(
					mini, args[i]);
		}
		i++;
	}
	if (mini->last_exit_status != 1)
		mini->last_exit_status = 0;
	return (mini->last_exit_status);
}
