#include "../libft/inc/ft_printf.h"
#include "../inc/executor.h"
#include <stdio.h>

int handle_redirecctions_in_child(t_command *cmd)
{
 t_redirection *current_redir;
 int input_fd_to_use = -1;
 int fd;

 current_redir = cmd->redirs;
 while (current_redir)
 {
  if (current_redir->type == REDIR_IN)
  {
    if (input_fd_to_use != -1)
      close(input_fd_to_use);
    input_fd_to_use = open(current_redir->file, O_RDONLY);
    if (input_fd_to_use == -1)
    {
      perror("minishell");
      return (1);
    }
  }
  current_redir = current_redir->next;
}
if (input_fd_to_use == -1 && cmd->heredoc_fd != -1)
{
  input_fd_to_use = cmd->heredoc_fd; 
}
current_redir = cmd->redirs;
while(current_redir)
{
  if (current_redir->type == REDIR_OUT)
  {
    fd = open(current_redir->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
    {
      perror("minishell");
      if (input_fd_to_use != -1 && input_fd_to_use != cmd->heredoc_fd)
        close(input_fd_to_use);
      return (1);
    }
    if (dup2(fd, STDOUT_FILENO) == -1)
    {
      perror("minishell: dup2 failed for output");
      close(fd);
      if (input_fd_to_use != -1 && input_fd_to_use != cmd->heredoc_fd)
        close(input_fd_to_use);
      return (1);
    }
    close(fd);
  }
  else if (current_redir->type == REDIR_APPEND)
  {
    fd = open(current_redir->file, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1)
    {
      perror("minishell");
      if (input_fd_to_use != -1 && input_fd_to_use != cmd->heredoc_fd)
        close(input_fd_to_use);
      return (1);
    }
    if (dup2(fd, STDOUT_FILENO) == -1)
    {
      perror("minishell: dup2 failed for append");
      close (fd);
      if (input_fd_to_use != -1 && input_fd_to_use != cmd->heredoc_fd)
        close(input_fd_to_use);
      return (1);
    }
    close (fd);
  }
  current_redir = current_redir->next;
 }
 if (input_fd_to_use != -1)
 {
  if (dup2(input_fd_to_use, STDIN_FILENO) == -1)
  {
    perror("minishell: dup2 for final heredoc");
    close(input_fd_to_use);
    return (1);
  }
  close(input_fd_to_use);
 }
 return (0);
}

