/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipeline_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhiguita <rhiguita@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 04:05:09 by rhiguita          #+#    #+#             */
/*   Updated: 2025/09/03 04:05:13 by rhiguita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/builtins.h"
#include "../inc/executor.h"

void	parent_cleanup(t_command *cmd, t_exec_data *d)
{
	d->child_pids[d->cmd_idx] = d->pid;
	if (d->prev_fd != -1)
		close(d->prev_fd);
	if (cmd->next)
	{
		close(d->pipe_fd[1]);
		d->prev_fd = d->pipe_fd[0];
	}
	else
		d->prev_fd = -1;
	if (cmd->heredoc_fd != -1)
	{
		close(cmd->heredoc_fd);
		cmd->heredoc_fd = -1;
	}
}

static void	handle_child_status(t_command *cmd, int status, t_struct *mini,
		int is_last)
{
	if (WIFEXITED(status))
	{
		mini->last_exit_status = WEXITSTATUS(status);
		if (WEXITSTATUS(status) == 127 && cmd && cmd->args[0])
		{
			ft_putstr_fd("minishell: ", STDERR_FILENO);
			ft_putstr_fd(cmd->args[0], STDERR_FILENO);
			ft_putendl_fd(": command not found", STDERR_FILENO);
		}
	}
	else if (WIFSIGNALED(status))
		mini->last_exit_status = 128 + WTERMSIG(status);
	if (!is_last)
		mini->last_exit_status = 0;
}

void	wait_for_children(t_command *cmds, pid_t *child_pids,
		int num_commands, t_struct *mini)
{
	int			status;
	int			i;
	t_command	*current_cmd;

	i = 0;
	current_cmd = cmds;
	while (i < num_commands)
	{
		waitpid(child_pids[i], &status, 0);
		handle_child_status(current_cmd, status, mini, i == num_commands - 1);
		if (current_cmd)
			current_cmd = current_cmd->next;
		i++;
	}
}

int	init_pipeline(t_command *commands, pid_t **child_pids_ptr)
{
	t_command	*current_cmd;
	int			num_commands;

	num_commands = 0;
	current_cmd = commands;
	while (current_cmd)
	{
		num_commands++;
		current_cmd = current_cmd->next;
	}
	*child_pids_ptr = ft_calloc(num_commands + 1, sizeof(pid_t));
	if (!*child_pids_ptr)
	{
		perror("minishell: calloc child_pids");
		return (-1);
	}
	return (num_commands);
}

void	handle_fork_error(t_command *current_cmd, t_exec_data *d)
{
	perror("minishell: fork");
	d->mini->last_exit_status = 1;
	if (d->prev_fd != -1)
		close(d->prev_fd);
	if (current_cmd && current_cmd->next)
	{
		close(d->pipe_fd[0]);
		close(d->pipe_fd[1]);
	}
	if (current_cmd && current_cmd->heredoc_fd != -1)
	{
		close(current_cmd->heredoc_fd);
		current_cmd->heredoc_fd = -1;
	}
	free(d->child_pids);
}
