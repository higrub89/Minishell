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
