/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_cleanup.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhiguita <rhiguita@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 02:06:11 by rhiguita          #+#    #+#             */
/*   Updated: 2025/09/03 02:06:15 by rhiguita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/parser.h"

void	*set_syntax_error(const char *token_value, t_struct *mini)
{
	fprintf(stderr, "minishell: syntax error near unexpected token `%s'\n",
		token_value);
	mini->last_exit_status = 258;
	return (NULL);
}

void	free_args(char **args)
{
	int	i;

	if (!args)
		return ;
	i = 0;
	while (args[i])
		free(args[i++]);
	free(args);
}

void	free_redirections(t_redirection *redir)
{
	t_redirection	*next_redir;

	while (redir)
	{
		next_redir = redir->next;
		if (redir->file)
			free(redir->file);
		free(redir);
		redir = next_redir;
	}
}

void	free_commands(t_command *head)
{
	t_command	*cmd_tmp;

	while (head)
	{
		cmd_tmp = head;
		head = head->next;
		free_args(cmd_tmp->args);
		free_redirections(cmd_tmp->redirections);
		if (cmd_tmp->full_path)
			free(cmd_tmp->full_path);
		if (cmd_tmp->heredoc_fd != -1)
			close(cmd_tmp->heredoc_fd);
		free(cmd_tmp);
	}
}
