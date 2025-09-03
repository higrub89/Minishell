/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhiguita <rhiguita@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 06:03:35 by rhiguita          #+#    #+#             */
/*   Updated: 2025/09/03 06:03:37 by rhiguita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/expander.h"
#include "../inc/redirection.h"

static volatile sig_atomic_t	g_last_signal = 0;

void	handle_sigint_heredoc(int sig)
{
	(void)sig;
	g_last_signal = 1;
}

char	*get_expanded_line(char *line, t_redirection *heredoc,
		t_struct *mini)
{
	char	*expanded_line;

	if (heredoc->expand_heredoc_content)
		expanded_line = expand_heredoc_line(line, mini);
	else
		expanded_line = ft_strdup(line);
	free(line);
	return (expanded_line);
}

int	write_heredoc_line(char *expanded_line, int pipe_fd, t_struct *mini)
{
	if (!expanded_line)
	{
		perror("minishell: malloc failed during heredoc expansion");
		mini->last_exit_status = 1;
		return (1);
	}
	write(pipe_fd, expanded_line, ft_strlen(expanded_line));
	write(pipe_fd, "\n", 1);
	free(expanded_line);
	return (0);
}

int	heredoc_loop(t_redirection *heredoc, int pipe_fd, t_struct *mini)
{
	char	*line;

	while (1)
	{
		line = readline("> ");
		if (g_last_signal)
		{
			free(line);
			mini->last_exit_status = 130;
			return (1);
		}
		if (!line)
		{
			break ;
		}
		if (ft_strcmp(line, heredoc->file) == 0)
		{
			free(line);
			break ;
		}
		if (write_heredoc_line(get_expanded_line(line, heredoc, mini), pipe_fd,
				mini))
			return (1);
	}
	return (0);
}

int	handle_single_heredoc(t_redirection *redir, int *heredoc_fd, t_struct *mini)
{
	int	pipe_fd[2];

	signal(SIGINT, handle_sigint_heredoc);
	g_last_signal = 0;
	if (pipe(pipe_fd) == -1)
		return (perror("minishell: pipe"), 1);
	if (heredoc_loop(redir, pipe_fd[1], mini))
	{
		close(pipe_fd[0]);
		close(pipe_fd[1]);
		return (1);
	}
	signal(SIGINT, SIG_DFL);
	close(pipe_fd[1]);
	if (*heredoc_fd != -1)
		close(*heredoc_fd);
	*heredoc_fd = pipe_fd[0];
	return (0);
}
