/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection_handler.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhiguita <rhiguita@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 01:17:43 by rhiguita          #+#    #+#             */
/*   Updated: 2025/09/03 01:17:56 by rhiguita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/lexer.h"

int	handle_redirection_in(const char *input, int i, t_token **head,
		t_token **current)
{
	t_token	*new_token;

	if (input[i + 1] == '<')
	{
		new_token = create_new_token("<<", HEREDOC);
		if (!new_token)
			return (-1);
		add_token_to_list(head, current, new_token);
		return (i + 2);
	}
	new_token = create_new_token("<", IN);
	if (!new_token)
		return (-1);
	add_token_to_list(head, current, new_token);
	return (i + 1);
}

int	handle_redirection_out(const char *input, int i, t_token **head,
		t_token **current)
{
	t_token	*new_token;

	if (input[i + 1] == '>')
	{
		new_token = create_new_token(">>", APPE_OUT);
		if (!new_token)
			return (-1);
		add_token_to_list(head, current, new_token);
		return (i + 2);
	}
	new_token = create_new_token(">", OUT);
	if (!new_token)
		return (-1);
	add_token_to_list(head, current, new_token);
	return (i + 1);
}

int	lexer_handle_pipe(const char *input, int i, t_token **head, t_token **current)
{
	t_token	*new_token;

	(void)input;
	new_token = create_new_token("|", PIPE);
	if (!new_token)
		return (-1);
	add_token_to_list(head, current, new_token);
	return (i + 1);
}
