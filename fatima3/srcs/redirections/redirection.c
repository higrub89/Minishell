/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhiguita <rhiguita@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 05:54:06 by rhiguita          #+#    #+#             */
/*   Updated: 2025/09/03 05:54:12 by rhiguita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/redirection.h"

char	*get_expanded_line(char *line, t_redirection *heredoc, t_struct *mini)
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

int	process_heredoc_input(t_command *cmd_list, t_struct *mini)
{
	t_redirection	*redir;
	t_command		*cmd;

	cmd = cmd_list;
	while (cmd)
	{
		redir = cmd->redirections;
		while (redir)
		{
			if (redir->type == REDIR_HEREDOC)
			{
				if (handle_single_heredoc(redir, &cmd->heredoc_fd, mini))
					return (1);
			}
			redir = redir->next;
		}
		cmd = cmd->next;
	}
	return (0);
}
