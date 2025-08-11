#ifndef EXECUTOR_H
# define EXECUTOR_H

# include "minishell.h"
# include <sys/wait.h>
# include <sys/types.h>
# include <signal.h>

int execute_commands(t_command *commands, t_struct *mini);
int handle_redirecctions_in_child(t_command *cmd);

int is_builtin(char *cmd_name);
int execute_builtin(t_command *cmd, t_struct *mini);
#endif