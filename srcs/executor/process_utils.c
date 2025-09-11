/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhiguita <rhiguita@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 23:28:30 by rhiguita          #+#    #+#             */
/*   Updated: 2025/09/11 23:45:13 by rhiguita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/builtins.h"
#include "../inc/executor.h"

int	process_heredocs(t_command *commands, t_struct *mini)
{
	t_redirection	*redir;
	t_command		*cmd;

	cmd = commands;
	while (cmd)
	{
		redir = cmd->redirections;
		while (redir)
		{
			if (redir->type == REDIR_HEREDOC)
			{
				if (handle_single_heredocs(cmd, redir, mini) != 0)
					return (1);
			}
			redir = redir->next;
		}
		cmd = cmd->next;
	}
	return (0);
}

void	print_heredoc_warning(t_redirection *redir)
{
	char	buffer[256];

	ft_strlcpy(buffer,
		"minishell: warning: here-document delimited by end-of-file (wanted `",
		256);
	ft_strlcat(buffer, redir->file, 256);
	ft_strlcat(buffer, "')", 256);
	ft_putendl_fd(buffer, STDERR_FILENO);
}

pid_t	create_pipe_and_fork(t_command *cmd, int pipe_fd[2])
{
	if (cmd->next && pipe(pipe_fd) == -1)
		return (perror("pipe"), -1);
	return (fork());
}

void	child_process(t_command *cmd, int pipe_fd[2], int prev_fd,
		t_struct *mini)
{
	mini->is_piped = true;
	set_signals(CHILD);
	setup_child_pipes(cmd, pipe_fd, prev_fd);
	child_execute_command(cmd, mini);
}

void	parent_process(t_command *cmd, t_exec_data *d)
{
	parent_cleanup(cmd, d);
	d->cmd_idx++;
}
