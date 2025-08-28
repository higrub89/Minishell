#ifndef REDIRECTION_H
# define REDIRECTION_H

# include "minishell.h"
# include "parser.h"
# include  <signal.h>

int	process_heredoc_input(t_command *cmd_list, t_struct *mini);

#endif