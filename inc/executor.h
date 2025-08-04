#ifndef EXECUTOR_H
# define EXECUTOR_H

# include "../libft/inc/libft.h"
# include "command.h"
# include <readline/readline.h>
# include <sys/wait.h>
# include <sys/types.h>
# include <signal.h>

int execute_commands(t_command *commands, char **envp, int *last_exit_status_ptr);
int handle_redirecctions_in_child(t_command *cmd);

#endif