#ifndef PARSER_H
# define PARSER_H

# include "token.h"
# include "command.h"

// prototipo de la función pricipal del parser.
t_command *parse_input(t_token *token_list);

#endif