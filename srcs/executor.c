#include "../inc/executor.h"
#include "../libft/inc/libft.h"


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