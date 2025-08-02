#ifndef EXECUTOR_H
# define EXECUTOR_H

# include "../libft/inc/libft.h"
# include "command.h"
# include <readline/readline.h>
# include <sys/wait.h>
# include <sys/types.h>
# include <signal.h>

//int execute_commands(t_command *head_cmd);
int handle_redirecctions(t_redirection *redirs, char **envp);
int execute_external_command(t_command *cmd, char **envp, int *last_exit_status);
char *find_command_path(char *cmd_name, char **envp);

#endif