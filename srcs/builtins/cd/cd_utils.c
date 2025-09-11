/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd_utils.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatigarc <fatigarc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 06:50:40 by fatigarc          #+#    #+#             */
/*   Updated: 2025/09/03 11:00:02 by fatigarc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../inc/builtins.h"

int		is_valid_directory(const char *path);
int		update_pwd_after_chdir(t_struct *mini);
void	update_pwd_env(t_struct *mini, const char *new_path);

int	is_valid_directory(const char *path)
{
	struct stat	info;

	if (stat(path, &info) != 0)
		return (-1);
	if (stat(path, &info) == 0 && S_ISDIR(info.st_mode))
		return (1);
	return (0);
}

int	update_pwd_after_chdir(t_struct *mini)
{
	char	*cwd;

	cwd = getcwd(NULL, 0);
	if (!cwd)
	{
		perror("minishell: cd: getcwd failed");
		mini->last_exit_status = 1;
		return (1);
	}
	update_pwd_env(mini, cwd);
	free(cwd);
	mini->last_exit_status = 0;
	return (0);
}

void	update_pwd_env(t_struct *mini, const char *new_path)
{
	char	*oldpwd;

	oldpwd = get_env_value("PWD", mini);
	if (oldpwd)
	{
		ft_setenv_var(mini, "OLDPWD", oldpwd);
		free(oldpwd);
	}
	ft_setenv_var(mini, "PWD", new_path);
}
