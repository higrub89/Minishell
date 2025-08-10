#ifndef PARSER_H
# define PARSER_H

# include "../libft/inc/ft_printf.h"

# include "lexer.h"
# include "minishell.h"

// prototipo de la función pricipal del parser.
t_command *parse_input(t_token *token_list, t_struct *mini);
void free_commands(t_command *head);

#endif