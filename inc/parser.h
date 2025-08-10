#ifndef PARSER_H
# define PARSER_H

# include "../libft/inc/ft_printf.h"
# include "minishell.h"

// prototipo de la función pricipal del parser.
t_command *parse_input(t_token *token_list, t_struct *mini);
int add_arg_to_command(t_command *cmd, const char *arg);
t_command *create_command_node(void);
t_redirection *create_redirection_node(t_redir_type type, char *file);
void add_redir_to_command(t_command *cmd, t_redirection *new_redir);

int proccess_heredoc,input(t_command *cmd, t_struct *mini);
int handle_redirecctions_in_child(t_command *cmd);

#endif