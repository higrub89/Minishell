/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_executor.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhiguita <rhiguita@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 04:37:14 by rhiguita          #+#    #+#             */
/*   Updated: 2025/09/04 03:15:40 by rhiguita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/executor.h"
#include "../inc/expander.h"

int	process_heredocs(t_command *commands, t_struct *mini)
{
	t_command		*cmd;
	t_redirection	*redir;

	cmd = commands;
	while (cmd)
	{
		redir = cmd->redirections;
		while (redir)
		{
			if (redir->type == REDIR_HEREDOC)
			{
				if (cmd->heredoc_fd != -1)
					close(cmd->heredoc_fd);
				cmd->heredoc_fd = read_heredoc_input(redir, mini);
				if (cmd->heredoc_fd == -1)
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

int	read_heredoc_input(t_redirection *redir, t_struct *mini)
{
	int	pipe_fds[2];

	if (pipe(pipe_fds) == -1)
		return (perror("minishell: pipe"), -1);
	while (1)
	{
		if (process_heredoc_line(redir, mini, pipe_fds[1]))
			break ;
	}
	close(pipe_fds[1]);
	return (pipe_fds[0]);
}
