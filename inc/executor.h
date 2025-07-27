#ifndef EXECUTOR_H
# define EXECUTOR_H

# include "../libft/inc/libft.h"
# include "command.h"
# include <sys/wait.h>
# include <sys/types.h>
# include <signal.h>

extern char **environ;
//int execute_commands(t_command *head_cmd);
int execute_external_command(t_command *cmd, int *last_exit_status);
char *find_command_path(char *cmd_name);

#endif