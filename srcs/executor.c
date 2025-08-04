#include "../inc/executor.h"
#include "../inc/expander.h"
#include "../inc/parser.h"
#include "../libft/inc/libft.h"

extern int g_last_exit_status;

static int open_redir_file(t_redirection *redir, int *last_exit_status_ptr)
{
  int fd;

  fd = -1;
  if (redir->type == REDIR_IN)
  {
    if (access(redir->file, F_OK) == -1)
    {
      fprintf(stderr, "minishell: %s: NO such file or directory\n", redir->file);
      *last_exit_status_ptr = 1;
      return (-1);
    }
    if (access(redir->file, R_OK) == -1)
    {
      fprintf(stderr, "minishell: %s: Permission denied\n", redir->file);
      *last_exit_status_ptr = 1;
      return (-1);
    }
    fd = open(redir->file, O_RDONLY);
  }
  else if (redir->type == REDIR_OUT) // >
  {
    fd = open(redir->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  }
  else if (redir->type == REDIR_APPEND)
  {
    fd = open(redir->file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
  }
  if (fd == -1)
  {
    perror("minishell");
    *last_exit_status_ptr = 1; 
  }
  return (fd);
}

static int apply_redirections(t_command *cmd, int *last_exit_status_ptr)
{
  t_redirection *redir;
  int fd;

  redir = cmd->redirs;
  while(redir)
  {
    if (redir->type == REDIR_IN || redir->type == REDIR_OUT || redir->type == REDIR_APPEND)
    {
      fd = open_redir_file(redir, last_exit_status_ptr);
      if (fd == -1)
        return (-1);
      if (redir->type == REDIR_IN)
      {
        if (dup2(fd, STDIN_FILENO) == -1)
        {
          perror("minishell: dup2 for stdin");
          close(fd);
          *last_exit_status_ptr = 1;
          return (-1);
        }
      }
      else
      {
        if (dup2(fd, STDOUT_FILENO) == -1)
        {
          perror("minishell: dup2 for stdout");
          close(fd);
          *last_exit_status_ptr = 1;
          return (-1);
        }
      }
      close(fd);
    }
    else if (redir->type == REDIR_HEREDOC)
    {
      if (dup2(cmd->heredoc_fd, STDIN_FILENO) == -1)
      {
        perror("minishell: dup2 for heredoc");
        *last_exit_status_ptr = 1;
        return (-1);
      }
      close(cmd->heredoc_fd);
    }
    redir = redir->next;
  }
  return (0);
}

char *find_command_path(char *cmd_name)
{
  char *path_env;
  char **paths;
  char *full_path;
  int i;

  if (!cmd_name)
    return (NULL);
  if (ft_strchr(cmd_name, '/') && access(cmd_name, F_OK | X_OK) == 0)
  {
    return (ft_strdup(cmd_name));
  }
  path_env = NULL;
  i = 0;
  while(environ[i]) // Buscar "PATH=" en el array global 'environ'
  {
    if (ft_strncmp(environ[i], "PATH=", 5) == 0)
    {
      
      path_env = environ[i] + 5; // obtener la cadena después de "PATH="
      break;
    }
    i++;
  }
  if (!path_env) // Si no encuentra la variable PTAH
    return (NULL);
  paths = ft_split(path_env, ':');
  if (!paths)
    return (NULL);
  i = 0;
  while (paths[i])
  {
    full_path = (char *)malloc(ft_strlen(paths[i]) + ft_strlen(cmd_name) + 2);
    if (!full_path)
    {
      i = 0;
      while (paths[i])
        free(paths[i++]);
      free(paths);
      return (NULL);
    }
    ft_strcpy(full_path, paths[i]);
    ft_strcat(full_path, "/");
    ft_strcat(full_path, cmd_name);

    if (access(full_path, F_OK | X_OK) == 0)
    {
      i = 0;
      while (paths[i])
        free(paths[i++]);
      free(paths);
      return (full_path);
    }
    free(full_path);
    i++;
  }
  i = 0;
  while (paths[i])
    free(paths[i++]);
  free(paths);
  return (NULL);
}

int execute_external_command(t_command *cmd, int *last_exit_status)
{
  char *cmd_path;
  pid_t pid;
  int status;

  if (!cmd || !cmd->args || !cmd->args[0])
  {
    *last_exit_status = 1;
    return (*last_exit_status);
  }

  cmd_path = find_command_path(cmd->args[0]);
  if (!cmd_path)
  {
    ft_putstr_fd("minishell: ", STDERR_FILENO);
    ft_putstr_fd(cmd->args[0], STDERR_FILENO);
    ft_putstr_fd(": command not found\n", STDERR_FILENO);
    *last_exit_status = 127;
    return (*last_exit_status);
  }

  pid = fork();
  if (pid == -1)
  {
    perror("minishell: fork");
    free(cmd_path);
    *last_exit_status = 1;
    return (*last_exit_status);
  }
  else if (pid == 0)
  {
    // Redirecciones , pipe, builting 
    if (handle_redirecctions(cmd->redirs) != 0)
    {
      free(cmd_path);
      exit (1);
    }
    execve(cmd_path, cmd->args, environ);

    perror("minishell: execve");
    free(cmd_path);
    exit(126);
  }
  else
  {
    if (waitpid(pid, &status, 0) == -1)
    {
      perror("minishell: waitpid");
      *last_exit_status = 1;
      free(cmd_path);
      return (*last_exit_status);
    }
    if (WIFEXITED(status))
    {
      *last_exit_status = WEXITSTATUS(status);
    }
    else if (WIFSIGNALED(status))
    {
      *last_exit_status = 128 + WTERMSIG(status);
      if (WTERMSIG(status) == SIGQUIT)
      {
        ft_putstr_fd("Quit (core dumped)\n", STDERR_FILENO);
      }
    }
    free(cmd_path);
  }
  return (*last_exit_status);
}