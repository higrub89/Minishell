/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   words_handler.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhiguita <rhiguita@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 01:17:26 by rhiguita          #+#    #+#             */
/*   Updated: 2025/09/03 01:17:32 by rhiguita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../inc/lexer.h"

int	skip_spaces(const char *input, int i)
{
	while (input[i] && (input[i] == ' ' || input[i] == '\t'))
		i++;
	return (i);
}

int	extract_word(const char *input, int i, t_struct *mini)
{
	char	in_quote;

	in_quote = 0;
	while (input[i])
	{
		if (in_quote)
		{
			if (input[i] == in_quote)
				in_quote = 0;
		}
		else if (input[i] == '\'' || input[i] == '"')
			in_quote = input[i];
		else if (ft_strchr(" \t<>|", input[i]))
			break ;
		i++;
	}
	if (in_quote)
	{
		fprintf(stderr, "minishell: Syntax error: unclosed quote\n");
		mini->last_exit_status = 258;
		return (-1);
	}
	return (i);
}

int	handle_word_error(t_token **head, t_struct *mini)
{
	perror("minishell: malloc failed for word_value");
	free_tokens(*head);
	mini->last_exit_status = 1;
	return (-1);
}

int	handle_word(t_lexer_ctx *ctx, int i)
{
	int		start;
	int		token_len;
	char	*word_value;
	t_token	*new_token;

	start = i;
	i = extract_word(ctx->input, i, ctx->mini);
	if (i < 0)
		return (-1);
	token_len = i - start;
	word_value = ft_substr(ctx->input, start, token_len);
	if (!word_value)
		return (handle_word_error(ctx->head, ctx->mini));
	new_token = create_new_token(word_value, WORD);
	free(word_value);
	if (!new_token)
		return (-1);
	add_token_to_list(ctx->head, ctx->current, new_token);
	return (i);
}
