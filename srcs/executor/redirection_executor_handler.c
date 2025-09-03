/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_executor_handler.c                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhiguita <rhiguita@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 04:05:25 by rhiguita          #+#    #+#             */
/*   Updated: 2025/09/03 04:05:27 by rhiguita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/executor.h"

int	dup2_and_close(int oldfd, int newfd)
{
	if (oldfd == newfd)
		return (0);
	if (dup2(oldfd, newfd) == -1)
	{
		perror("minishell: dup2");
		close(oldfd);
		return (1);
	}
	close(oldfd);
	return (0);
}

int	open_redirection_file(t_redirection *redir)
{
	if (redir->type == REDIR_IN)
		return (open(redir->file, O_RDONLY));
	else if (redir->type == REDIR_OUT)
		return (open(redir->file, O_WRONLY | O_CREAT | O_TRUNC, 0644));
	else if (redir->type == REDIR_APPEND)
		return (open(redir->file, O_WRONLY | O_CREAT | O_APPEND, 0644));
	return (-1);
}

int	handle_single_redirection(t_redirection *redir, int *last_fd)
{
	int	new_fd;

	if (*last_fd != -1)
		close(*last_fd);
	new_fd = open_redirection_file(redir);
	if (new_fd == -1)
	{
		perror(redir->file);
		return (-1);
	}
	*last_fd = new_fd;
	return (0);
}

int	handle_redir(t_redirection *redir, int *last_fd)
{
	if (handle_single_redirection(redir, last_fd) == -1)
	{
		if (*last_fd != -1)
			close(*last_fd);
		return (1);
	}
	return (0);
}

int	apply_final_fds(int last_in_fd, int last_out_fd)
{
	if (last_in_fd != -1 && dup2_and_close(last_in_fd, STDIN_FILENO))
		return (1);
	if (last_out_fd != -1 && dup2_and_close(last_out_fd, STDOUT_FILENO))
		return (1);
	return (0);
}
