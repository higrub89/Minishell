#ifndef PARSER_H
# define PARSER_H

# include "libft.h"
# include "token.h"
# include "lexer.h"
# include "token_utils.h"
# include "command_utils.h"

// prototipo de la función pricipal del parser.
t_command *parse_input(t_token *token_list);

#endif