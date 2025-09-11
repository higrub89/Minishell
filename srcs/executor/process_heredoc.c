/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   process_heredoc.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhiguita <rhiguita@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 22:15:34 by rhiguita          #+#    #+#             */
/*   Updated: 2025/09/11 23:48:43 by rhiguita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/builtins.h"
#include "../inc/executor.h"
#include "../inc/expander.h"

int	process_heredoc_line(t_redirection *redir, t_struct *mini,
		int pipe_write_fd)
{
	char	*line;
	char	*expanded_line;

	line = readline("> ");
	if (!line)
	{
		print_heredoc_warning(redir);
		return (1);
	}
	if (ft_strcmp(line, redir->file) == 0)
	{
		free(line);
		return (1);
	}
	if (redir->expand_heredoc_content)
	{
		expanded_line = expand_heredoc_line(line, mini);
		free(line);
		line = expanded_line;
	}
	ft_putendl_fd(line, pipe_write_fd);
	free(line);
	return (0);
}

int	fill_heredoc_pipe(t_redirection *redir, t_struct *mini,
		int write_fd)
{
	while (1)
	{
		if (process_heredoc_line(redir, mini, write_fd))
			break ;
	}
	return (0);
}

void	heredoc_child(t_redirection *redir, t_struct *mini, int *pipe_fds)
{
	int	exit_code;

	close(pipe_fds[0]);
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_IGN);
	exit_code = fill_heredoc_pipe(redir, mini, pipe_fds[1]);
	close(pipe_fds[1]);
	exit(exit_code);
}

int	heredoc_parent(t_command *cmd, t_struct *mini, int *pipe_fds,
		pid_t pid)
{
	int	status;

	close(pipe_fds[1]);
	waitpid(pid, &status, 0);
	if (WIFSIGNALED(status) && WTERMSIG(status) == SIGINT)
	{
		mini->last_exit_status = 130;
		close(pipe_fds[0]);
		return (1);
	}
	else if (WIFEXITED(status) && WEXITSTATUS(status) != 0)
	{
		close(pipe_fds[0]);
		return (1);
	}
	if (cmd->heredoc_fd != -1)
		close(cmd->heredoc_fd);
	cmd->heredoc_fd = pipe_fds[0];
	return (0);
}

int	handle_single_heredocs(t_command *cmd, t_redirection *redir,
		t_struct *mini)
{
	int		pipe_fds[2];
	pid_t	pid;

	if (pipe(pipe_fds) == -1)
		return (perror("minishell: pipe"), 1);
	pid = fork();
	if (pid == -1)
	{
		close(pipe_fds[0]);
		close(pipe_fds[1]);
		return (perror("minishell: fork"), 1);
	}
	if (pid == 0)
		heredoc_child(redir, mini, pipe_fds);
	else
		return (heredoc_parent(cmd, mini, pipe_fds, pid));
	return (0);
}
