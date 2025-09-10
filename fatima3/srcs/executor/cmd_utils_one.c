/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhiguita <rhiguita@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 04:04:55 by rhiguita          #+#    #+#             */
/*   Updated: 2025/09/03 04:05:00 by rhiguita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/executor.h"

void	add_slash_to_paths(char **paths)
{
	int		i;
	char	*tmp;
	size_t	len;

	i = 0;
	while (paths && paths[i])
	{
		len = ft_strlen(paths[i]);
		if (paths[i][len - 1] != '/')
		{
			tmp = ft_strjoin(paths[i], "/");
			if (!tmp)
			{
				perror("minishell: malloc error in add_slash_to_path");
				return ;
			}
			free(paths[i]);
			paths[i] = tmp;
		}
		i++;
	}
}

char	*get_path_var(char **envp)
{
	int		i;

	i = 0;
	while (envp && envp[i])
	{
		if (ft_strncmp(envp[i], "PATH=", 5) == 0)
			return (envp[i] + 5);
		i++;
	}
	return (NULL);
}

char	**get_paths(char **envp)
{
	char	*path_var;
	char	**paths;

	path_var = get_path_var(envp);
	if (!path_var || *path_var == '\0')
		return (NULL);
	paths = ft_split(path_var, ':');
	if (!paths)
		return (NULL);
	add_slash_to_paths(paths);
	return (paths);
}

char	*check_direct_path(char *cmd)
{
	if (access(cmd, X_OK) == 0)
		return (ft_strdup(cmd));
	return (NULL);
}
