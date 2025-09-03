/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhiguita <rhiguita@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/24 16:34:07 by rhiguita          #+#    #+#             */
/*   Updated: 2025/09/03 01:18:23 by rhiguita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/lexer.h"

int	lexer_syntax_error(char c, t_struct *mini, t_token *head)
{
	fprintf(stderr, "minishell: syntax error near unexpected token `%c'\n", c);
	mini->last_exit_status = 258;
	free_tokens(head);
	return (0);
}

int	lexer_handle_result(int i, t_struct *mini, t_token *head)
{
	if (i < 0)
	{
		free_tokens(head);
		if (mini->last_exit_status == 0)
			mini->last_exit_status = 1;
		return (0);
	}
	return (1);
}

int	lexer_process_token(t_lexer_ctx *ctx, int i)
{
	if (ctx->input[i] == '<')
		return (handle_redirection_in(ctx->input, i, ctx->head, ctx->current));
	else if (ctx->input[i] == '>')
		return (handle_redirection_out(ctx->input, i, ctx->head, ctx->current));
	else if (ctx->input[i] == '|')
		return (handle_pipe(ctx->input, i, ctx->head, ctx->current));
	return (handle_word(ctx, i));
}

static t_token	*lexer_loop(t_lexer_ctx *ctx)
{
	int		i;

	i = 0;
	while (ctx->input[i])
	{
		i = skip_spaces(ctx->input, i);
		if (!ctx->input[i])
			break ;
		if (ctx->input[i] == '&' || ctx->input[i] == ';' || ctx->input[i] == '('
			|| ctx->input[i] == ')')
		{
			if (!lexer_syntax_error(ctx->input[i], ctx->mini, *(ctx->head)))
				return (NULL);
		}
		i = lexer_process_token(ctx, i);
		if (!lexer_handle_result(i, ctx->mini, *(ctx->head)))
			return (NULL);
	}
	return (*(ctx->head));
}

t_token	*lexer(const char *input, t_struct *mini)
{
	t_token		*head;
	t_token		*current;
	t_lexer_ctx	ctx;

	head = NULL;
	current = NULL;
	if (!input)
		return (NULL);
	ctx.input = input;
	ctx.head = &head;
	ctx.current = &current;
	ctx.mini = mini;
	return (lexer_loop(&ctx));
}
