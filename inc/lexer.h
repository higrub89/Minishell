#ifndef LEXER_H
# define LEXER_H

# include "../libft/inc/libft.h"
# include "minishell.h"

t_token *lexer(const char *input_line, t_struct *mini);

#endif