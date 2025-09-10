/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatigarc <fatigarc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 04:37:08 by fatigarc          #+#    #+#             */
/*   Updated: 2025/09/07 10:49:44 by fatigarc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../inc/builtins.h"

int	ft_cd_error(t_struct *mini, char *msg, char *path);
int	ft_cd_path(t_struct *mini, char *path);
int	ft_cd(t_struct *mini, char **args);

int	ft_cd_error(t_struct *mini, char *msg, char *path)
{
	ft_putstr_fd("minishell: cd: ", STDERR_FILENO);
	if (path)
		ft_putstr_fd(path, STDERR_FILENO);
	if (msg)
	{
		ft_putstr_fd(": ", STDERR_FILENO);
		ft_putendl_fd(msg, STDERR_FILENO);
	}
	else
	{
		ft_putstr_fd(": ", STDERR_FILENO);
		ft_putendl_fd(strerror(errno), STDERR_FILENO);
	}
	mini->last_exit_status = 1;
	return (1);
}

int	ft_cd_path(t_struct *mini, char *path)
{
	int	check;

	check = is_valid_directory(path);
	if (check == -1)
		return (ft_cd_error(mini, "No such file or directory", path));
	if (check == 0)
		return (ft_cd_error(mini, "Not a directory", path));
	if (chdir(path) == 0)
		return (update_pwd_after_chdir(mini));
	return (ft_cd_error(mini, NULL, path));
}

int	ft_cd(t_struct *mini, char **args)
{
	char	*target;
	char	*cwd;

	target = args[1];
	if (!target || target[0] == '\0' || (ft_strncmp(target, "~", 2) == 0))
		return (go_home(mini));
	if (ft_strncmp(target, "-", 2) == 0)
		return (go_to_oldpwd(mini));
	if (ft_strncmp(target, "..", 3) == 0)
		return (go_to_parent(mini));
	if (ft_strncmp(target, ".", 2) == 0 || ft_strncmp(target, "./", 3) == 0)
	{
		cwd = getcwd(NULL, 0);
		if (!cwd)
			return (ft_cd_error(mini, "getcwd failed", NULL));
		update_pwd_env(mini, cwd);
		free(cwd);
		mini->last_exit_status = 0;
		return (0);
	}
	return (ft_cd_path(mini, target));
}
