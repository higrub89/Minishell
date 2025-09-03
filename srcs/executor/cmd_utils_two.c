/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_utils_two.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhiguita <rhiguita@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 04:26:54 by rhiguita          #+#    #+#             */
/*   Updated: 2025/09/03 04:26:56 by rhiguita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/executor.h"

char	*find_cmd_path(char **cmd_args, char **paths)
{
	int		i;
	char	*full_path;

	if (!cmd_args || !cmd_args[0])
		return (NULL);
	if (ft_strchr(cmd_args[0], '/'))
		return (check_direct_path(cmd_args[0]));
	if (!paths)
		return (NULL);
	i = 0;
	while (paths[i])
	{
		full_path = ft_strjoin(paths[i], cmd_args[0]);
		if (!full_path)
			return (NULL);
		if (access(full_path, X_OK) == 0)
			return (full_path);
		free(full_path);
		i++;
	}
	return (NULL);
}

void	execute_external_command(t_command *cmd, t_struct *mini)
{
	char	**paths;
	char	*cmd_path;

	paths = get_paths(mini->envp);
	cmd_path = find_cmd_path(cmd->args, paths);
	if (!cmd_path)
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		ft_putstr_fd(cmd->args[0], STDERR_FILENO);
		ft_putendl_fd(": command not found", STDERR_FILENO);
		free_str_array(paths);
		exit(127);
	}
	execve(cmd_path, cmd->args, mini->envp);
	perror("minishell: execve failed");
	free(cmd_path);
	free_str_array(paths);
	exit(126);
}

void	child_execute_command(t_command *cmd, t_struct *mini)
{
	if (handle_redirections_in_child(cmd) != 0)
		exit(1);
	if (!cmd->args || !cmd->args[0])
		exit(0);
	if (cmd->args[0][0] == '\0')
	{
		ft_putstr_fd("minishell: ", STDERR_FILENO);
		ft_putstr_fd(cmd->args[0], STDERR_FILENO);
		ft_putendl_fd(": command not found", STDERR_FILENO);
		exit(127);
	}
	if (is_builtin(cmd->args[0]))
	{
		execute_builtin(cmd, mini);
		exit(mini->last_exit_status);
	}
	execute_external_command(cmd, mini);
}
