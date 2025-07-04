#ifndef EXECUTOR_H
# define EXECUTOR_H

# include "../libft/inc/libft.h"
# include <sys/wait.h>

int execute_commands(t_command *head_cmd);
char *find_command_path(char *cmd_name, char **envp);

#endif