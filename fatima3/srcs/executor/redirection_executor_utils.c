/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhiguita <rhiguita@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 04:20:06 by rhiguita          #+#    #+#             */
/*   Updated: 2025/09/03 04:20:08 by rhiguita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/builtins.h"
#include "../inc/executor.h"

void	setup_child_pipes(t_command *cmd, int *pipe_fds, int prev_pipe_in_fd)
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

int	process_redirections(t_command *cmd, int *last_in_fd,
		int *last_out_fd)
{
	t_redirection	*redir;

	redir = cmd->redirections;
	while (redir)
	{
		if (redir->type != REDIR_HEREDOC)
		{
			if ((redir->type == REDIR_IN && handle_redir(redir, last_in_fd))
				|| ((redir->type == REDIR_OUT || redir->type == REDIR_APPEND)
					&& handle_redir(redir, last_out_fd)))
				return (1);
		}
		redir = redir->next;
	}
	return (0);
}

int	handle_redirections_in_child(t_command *cmd)
{
	int	last_in_fd;
	int	last_out_fd;

	if (cmd->heredoc_fd != -1)
		last_in_fd = cmd->heredoc_fd;
	else
		last_in_fd = -1;
	last_out_fd = -1;
	if (process_redirections(cmd, &last_in_fd, &last_out_fd))
		return (1);
	return (apply_final_fds(last_in_fd, last_out_fd));
}

void	close_heredoc(t_command *cmd)
{
	if (cmd->heredoc_fd != -1)
	{
		close(cmd->heredoc_fd);
		cmd->heredoc_fd = -1;
	}
}

int	handle_single_redirection_only(t_command *cmd, t_struct *mini)
{
	int	orig_stdin;
	int	orig_stdout;
	int	result;

	orig_stdin = dup(STDIN_FILENO);
	orig_stdout = dup(STDOUT_FILENO);
	if (orig_stdin == -1 || orig_stdout == -1)
		return (perror("minishell: dup original fds"), 1);
	result = handle_redirections_in_child(cmd);
	restore_fds(orig_stdin, orig_stdout);
	if (cmd->heredoc_fd != -1)
	{
		close(cmd->heredoc_fd);
		cmd->heredoc_fd = -1;
	}
	mini->last_exit_status = result;
	return (result);
}
