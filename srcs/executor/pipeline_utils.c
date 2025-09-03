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

void	setup_child_pipes(t_command *cmd, int *pipe_fds,
		int prev_pipe_in_fd)
{
	if (prev_pipe_in_fd != -1)
	{
		if (dup2_and_close(prev_pipe_in_fd, STDIN_FILENO) != 0)
			exit(1);
	}
	if (cmd->next)
	{
		if (dup2_and_close(pipe_fds[1], STDOUT_FILENO) != 0)
			exit(1);
	}
	if (cmd->next)
	{
		close(pipe_fds[0]);
	}
}

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

void	wait_for_children(pid_t *child_pids, int num_commands,
		t_struct *mini)
{
	int	status;
	int	i;

	i = 0;
	while (i < num_commands)
	{
		waitpid(child_pids[i], &status, 0);
		if (i == num_commands - 1)
		{
			if (WIFEXITED(status))
				mini->last_exit_status = WEXITSTATUS(status);
			else if (WIFSIGNALED(status))
				mini->last_exit_status = 128 + WTERMSIG(status);
		}
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
