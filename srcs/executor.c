#include "../inc/executor.h"
#include "../inc/libft.h"

extern char **environ;

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
    ft_strcp
  }
}