/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhiguita <rhiguita@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 01:59:31 by rhiguita          #+#    #+#             */
/*   Updated: 2025/09/03 01:59:33 by rhiguita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_H
# define LEXER_H

# include "minishell.h"

typedef struct s_lexer_ctx
{
	const char	*input;
	t_struct	*mini;
	t_token		**head;
	t_token		**current;
}				t_lexer_ctx;

// lexer.c
void			free_tokens(t_token *head);
int				lexer_syntax_error(char c, t_struct *mini, t_token *head);
int				lexer_handle_result(int i, t_struct *mini, t_token *head);
int				lexer_process_token(t_lexer_ctx *ctx, int i);
t_token			*lexer(const char *input, t_struct *mini);

// redirection_handler.c
int				handle_redirection_in(const char *input, int i, t_token **head,
					t_token **current);
int				handle_redirection_out(const char *input, int i, t_token **head,
					t_token **current);
int				lexer_handle_pipe(const char *input, int i, t_token **head,
					t_token **current);

// token_utils.c
t_token			*create_new_token(char *value, t_token_type type);
void			add_token_to_list(t_token **head, t_token **current,
					t_token *new_token);
void			free_tokens(t_token *head);

// words_handler.c
int				handle_word(t_lexer_ctx *ctx, int i);
int				skip_spaces(const char *input, int i);
int				extract_word(const char *input, int i, t_struct *mini);
int				handle_word_error(t_token **head, t_struct *mini);

#endif
