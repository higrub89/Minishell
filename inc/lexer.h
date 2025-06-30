#ifndef LEXER_H
# define LEXER_H

# include "token.h"

t_token *lexer(char *input_line); //función principal lexer.
t_token *create_new_token(char *value, t_token_type type);
t_token *lexer(char *input_line);
void add_token_to_list(t_token **head, t_token **current, t_token *new_token);
void free_tokens(t_token *head);

#endif