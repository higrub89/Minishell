#ifndef PARSER_H
# define PARSER_H

# include "minishell.h"
# include "expander.h"

// prototipo de la función pricipal del parser.
t_command *parse_input(t_token *token_list, t_struct *mini);
int add_arg_to_command(t_command *cmd, const char *arg);
t_command *create_command_node(void);
t_redirection *create_redirection_node(t_redir_type type, char *file);
void add_redir_to_command(t_command *cmd, t_redirection *new_redir);
void free_commands(t_command *head);

#endif