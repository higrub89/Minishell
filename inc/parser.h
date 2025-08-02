#ifndef PARSER_H
# define PARSER_H

# include "../libft/inc/ft_printf.h"
# include "token.h"
# include "lexer.h"
# include "token_utils.h"
# include "command_utils.h"
# include "command.h"

// prototipo de la función pricipal del parser.
t_command *parse_input(t_token *token_list, char **envp);
void free_commands(t_command *head);

#endif