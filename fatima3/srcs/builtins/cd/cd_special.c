/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_special.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatigarc <fatigarc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 06:50:21 by fatigarc          #+#    #+#             */
/*   Updated: 2025/09/07 10:54:39 by fatigarc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../inc/builtins.h"

int	go_home(t_struct *mini);
int	go_to_oldpwd(t_struct *mini);
int	go_to_parent(t_struct *mini);

int	go_home(t_struct *mini)
{
	char	*home;

	home = get_env_value("HOME", mini);
	if (!home)
	{
		ft_putendl_fd("minishell: cd: HOME not set", STDERR_FILENO);
		free(home);
		mini->last_exit_status = 1;
		return (1);
	}
	if (chdir(home) != 0)
	{
		perror("minishell: cd");
		free(home);
		mini->last_exit_status = 1;
		return (1);
	}
	free(home);
	return (update_pwd_after_chdir(mini));
}

int	go_to_oldpwd(t_struct *mini)
{
	char	*oldpwd;

	oldpwd = get_env_value("OLDPWD", mini);
	if (!oldpwd)
	{
		ft_putendl_fd("minishell: cd: OLDPWD not set", STDERR_FILENO);
		free(oldpwd);
		mini->last_exit_status = 1;
		return (1);
	}
	if (chdir(oldpwd) != 0)
	{
		perror("minishell: cd");
		free(oldpwd);
		mini->last_exit_status = 1;
		return (1);
	}
	ft_putendl_fd(oldpwd, STDOUT_FILENO);
	free (oldpwd);
	return (update_pwd_after_chdir(mini));
}

int	go_to_parent(t_struct *mini)
{
	if (chdir("..") != 0)
	{
		perror("minishell: cd");
		mini->last_exit_status = 1;
		return (1);
	}
	return (update_pwd_after_chdir(mini));
}
