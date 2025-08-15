#ifndef LEXER_H
# define LEXER_H

# include "minishell.h"

t_token *lexer(const char *input_line, t_struct *mini);
void free_tokens(t_token *head);

#endif