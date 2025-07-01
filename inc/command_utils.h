#ifndef COMMAND_UTILS_H
# define COMMAND_UTILS_H

# include "command.h"

t_command *create_command_node(void);
int add_arg_to_command(t_command *cmd, char *arg_value);
t_redirection *create_redirection_node(t_redir_type type, char *file_name);
void  add_redir_to_command(t_command *cmd, t_redirection *new_redir);
void free_commands(t_command *head);

#endif