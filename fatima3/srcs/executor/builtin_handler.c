/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_handler.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhiguita <rhiguita@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 04:04:17 by rhiguita          #+#    #+#             */
/*   Updated: 2025/09/03 04:04:22 by rhiguita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/executor.h"
#include "../inc/builtins.h"

int	handle_single_builtin(t_command *cmd, t_struct *mini)
{
	int	orig_stdin;
	int	orig_stdout;

	set_signals(NON_INTERACTIVE);
	orig_stdin = dup(STDIN_FILENO);
	orig_stdout = dup(STDOUT_FILENO);
	if (orig_stdin == -1 || orig_stdout == -1)
		return (perror("minishell: dup original fds"), 1);
	if (handle_redirections_in_child(cmd) != 0)
	{
		restore_fds(orig_stdin, orig_stdout);
		close_heredoc(cmd);
		return (1);
	}
	execute_builtin(cmd, mini);
	restore_fds(orig_stdin, orig_stdout);
	close_heredoc(cmd);
	return (0);
}

int	is_builtin(char *cmd_name)
{
	if (!cmd_name)
		return (0);
	if (ft_strcmp(cmd_name, "echo") == 0)
		return (1);
	if (ft_strcmp(cmd_name, "cd") == 0)
		return (1);
	if (ft_strcmp(cmd_name, "pwd") == 0)
		return (1);
	if (ft_strcmp(cmd_name, "export") == 0)
		return (1);
	if (ft_strcmp(cmd_name, "unset") == 0)
		return (1);
	if (ft_strcmp(cmd_name, "env") == 0)
		return (1);
	if (ft_strcmp(cmd_name, "exit") == 0)
		return (1);
	return (0);
}

int	execute_builtin(t_command *cmd, t_struct *mini)
{
	if (!cmd || !cmd->args || !cmd->args[0])
	{
		mini->last_exit_status = 1;
		return (1);
	}
	if (ft_strcmp(cmd->args[0], "echo") == 0)
		return (ft_echo(cmd->args));
	else if (ft_strcmp(cmd->args[0], "cd") == 0)
		return (ft_cd(mini, cmd->args));
	else if (ft_strcmp(cmd->args[0], "pwd") == 0)
		return (ft_pwd(mini, cmd->args));
	else if (ft_strcmp(cmd->args[0], "export") == 0)
		return (ft_export(mini, cmd->args));
	else if (ft_strcmp(cmd->args[0], "unset") == 0)
		return (ft_unset(mini, cmd->args));
	else if (ft_strcmp(cmd->args[0], "env") == 0)
		return (ft_env(mini, cmd->args));
	else if (ft_strcmp(cmd->args[0], "exit") == 0)
		return (ft_exit(mini, cmd->args));
	mini->last_exit_status = 1;
	return (1);
}
