/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatigarc <fatigarc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 02:16:59 by rhiguita          #+#    #+#             */
/*   Updated: 2025/09/07 11:46:20 by fatigarc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/parser.h"

t_token	*process_redirection(t_token *tok, t_command *cmd, t_struct *mini)
{
	t_redirection	*new_redir;
	t_redir_type	type;

	if (tok->type == IN)
		type = REDIR_IN;
	else if (tok->type == OUT)
		type = REDIR_OUT;
	else if (tok->type == APPE_OUT)
		type = REDIR_APPEND;
	else
		type = REDIR_HEREDOC;
	if (!tok->next || tok->next->type != WORD)
	{
		if (tok->next)
			return (set_syntax_error(tok->next->value, mini));
		return (set_syntax_error("newline", mini));
	}
	new_redir = create_redirection_node(type, tok->next->value);
	if (!new_redir)
		return (NULL);
	add_redir_to_command(cmd, new_redir);
	return (tok->next->next);
}

t_token	*handle_pipe(t_command **cmd, t_token *tok, t_struct *mini,
		t_command *head)
{
	if ((*cmd)->num_args == 0 && (*cmd)->num_redirections == 0)
		return (free_commands(head), set_syntax_error("|", mini));
	tok = tok->next;
	if (!tok || tok->type == PIPE)
	{
		free_commands(head);
		if (tok)
			return (set_syntax_error(tok->value, mini));
		return (set_syntax_error("newline", mini));
	}
	(*cmd)->next = create_command_node();
	if (!(*cmd)->next)
		return (free_commands(head), NULL);
	(*cmd)->next->prev = *cmd;
	*cmd = (*cmd)->next;
	return (tok);
}

t_command	*finalize_cmds(t_command *head, t_command *curr, t_struct *mini)
{
	if (curr->num_args == 0 && curr->num_redirections == 0)
		return (free_commands(head), set_syntax_error("newline", mini));
	return (head);
}

t_token	*dispatch_token(t_token *tok, t_command **curr, t_struct *mini,
		t_command *head)
{
	if (tok->type == WORD)
	{
		if (!add_arg_to_command(*curr, tok->value))
			return (free_commands(head), NULL);
		tok = tok->next;
	}
	else if (tok->type >= IN && tok->type <= HEREDOC)
		tok = process_redirection(tok, *curr, mini);
	else if (tok->type == PIPE)
		tok = handle_pipe(curr, tok, mini, head);
	return (tok);
}

t_command	*parse_input(t_token *token_list, t_struct *mini)
{
	t_command	*cmd_head;
	t_command	*current_cmd;
	t_token		*tok;

	if (!token_list)
		return (NULL);
	cmd_head = create_command_node();
	if (!cmd_head)
		return (NULL);
	if (token_list->type == PIPE)
		return (free_commands(cmd_head), set_syntax_error("|", mini));
	current_cmd = cmd_head;
	tok = token_list;
	while (tok)
	{
		tok = dispatch_token(tok, &current_cmd, mini, cmd_head);
	}
	return (finalize_cmds(cmd_head, current_cmd, mini));
}
