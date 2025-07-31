#include "../libft/inc/ft_printf.h"
#include "../inc/executor.h"
#include <stdio.h>

int handle_redirecctions(t_redirection *redirs)
{
 t_redirection *current_redir;
 char *line;
 int heredoc_pipe_read_fd;
 int pipe_fds[2];

 heredoc_pipe_read_fd = -1; //Inicializar a -1 (ningún heredoc activo)
 current_redir = redirs;
 while (current_redir)
 {
  if (current_redir->type == REDIR_HEREDOC)
  {
    if (heredoc_pipe_read_fd == -1)
    {
      close(heredoc_pipe_read_fd);
      heredoc_pipe_read_fd = -1; //Resetear..
    }
    if (pipe(pipe_fds) == -1)
    {
      perror("minishell: pipe failed for heredoc");
      return (1);
    }
    while (1)
    {
      line = readline("> ");
      if (!line)
      {
        ft_putstr_fd("minishell: warning: here-document delimited by end-of-file (wanted '", STDERR_FILENO);
        ft_putstr_fd(current_redir->file, STDERR_FILENO);
        ft_putstr_fd("')\n", STDERR_FILENO);
        break; 
      }
      if (ft_strcmp(line, current_redir->file) == 0)
      {
        free(line);
        break; //delimitador encontrado
      }
      dprintf(pipe_fds[1], "%s\n", line);
      free(line);
    }
    close(pipe_fds[1]);
    heredoc_pipe_read_fd = pipe_fds[0];
  }
  current_redir = current_redir->next;
 }
 current_redir = redirs;
 while (current_redir)
 {
  int fd;
  if (current_redir->type == REDIR_IN)
  {
    fd = open(current_redir->file, O_RDONLY);
    if (fd == -1)
    {
      perror("minishell");
      if (heredoc_pipe_read_fd != -1)
        close(heredoc_pipe_read_fd);
      return (1); // Fallo el hijo debe salir.
    }
    if (dup2(fd, STDIN_FILENO) == -1)
    {
      perror("minishell: dup2 failed for input");
      close(fd);
      if (heredoc_pipe_read_fd != -1)
        close(heredoc_pipe_read_fd);
      return (1); // Fallo el hijo debe salir.
    }
    close(fd);
    if (heredoc_pipe_read_fd != -1)
    {
      close(heredoc_pipe_read_fd);
      heredoc_pipe_read_fd  = -1;
    }
  }
  else if (current_redir->type == REDIR_OUT)
  {
    fd = open(current_redir->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
    {
      perror("minishell");
      return (1);
    }
    if (dup2(fd, STDOUT_FILENO) == -1)
    {
      perror("minishell: dup2 failed for output");
      close(fd);
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
      return (1);
    }
    if (dup2(fd, STDOUT_FILENO) == -1)
    {
      perror("minishell: dup2 failed for append");
      close (fd);
      return (1);
    }
    close (fd);
  }
  current_redir = current_redir->next;
 }
 if (heredoc_pipe_read_fd != -1)
 {
  if (dup2(heredoc_pipe_read_fd, STDIN_FILENO) == -1)
  {
    perror("minishell: dup2 for final heredoc");
    close(heredoc_pipe_read_fd);
    return (1);
  }
  close(heredoc_pipe_read_fd);
 }
 return (0);
}

