#ifndef REDIRECTION_H
# define REDIRECTION_H

# include "minishell.h"
# include "parser.h"
# include  <signal.h>

int proccess_heredoc_input(t_command *cmd, t_struct *mini);

#endif