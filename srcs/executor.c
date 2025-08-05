#include "../inc/executor.h"
#include "../inc/expander.h"
#include "../inc/parser.h"
#include "../libft/inc/libft.h"

extern int g_last_exit_status;
/*
static char **get_paths(char **envp)
{
  int i = 0;
  while(envp[i])
  {
    if (ft_strncmp(envp[i], "PATH=", 5) == 0)
    {
      return (ft_split(envp[i] + 5, ':'));
    }
    i++;
  }
  return (ft_split("/bin:/usr/bin", ':'));
}

static char *find_cmd_path(const char *cmd, char **paths)
{
  char *full_path;
  char *temp_path;
  int i;

  if (!cmd)
    return (NULL);
    
  if (ft_strchr(cmd, '/') != NULL)
  {
    if (access(cmd, X_OK) == 0)
      return (ft_strdup(cmd));
    return (NULL);
  }
  if (!paths)
    return (NULL);

  i = 0;
  while (paths[i])
  {
    temp_path = ft_strjoin(paths[i], "/");
    if (!temp_path)
      return (NULL);
    full_path = ft_strjoin(temp_path, cmd);
    free(temp_path);
    if (!full_path)
      return (NULL);
    if (access(full_path, X_OK) == 0)
      return (full_path);
    free(full_path);
    i++;
  }
  return (NULL);
}

static void free_str_array(char **arr)
{
  if (!arr)
    return;
  int i;

  i = 0;
  while (arr[i])
  {
    free(arr[i]);
    i++;
  }
  free(arr);
}

/// funcion Builtings..

int execute_commands(t_command *commands, char **envp, int *last_exit_status_ptr)
{
  t_command *current_cmd;
  pid_t pid;
  int pipe_fds[2];
  int prev_pipe_in_fd = -1;
  pid_t *child_pids;
  int num_commands = 0;
  int i;

  current_cmd = commands;
  while(current_cmd)
  {
    num_commands++;
    current_cmd = current_cmd->next;
  }
  child_pids = ft_calloc(num_commands + 1, sizeof(pid_t));
  if (!child_pids)
  {
    perror("minishell: calloc");
    *last_exit_status_ptr = 1;
    if (commands && commands->heredoc_fd != -1)
    {
      close(commands->heredoc_fd);
      commands->heredoc_fd = -1;
    }
    return (1);
  }
  num_commands = 0;
  current_cmd = commands;
  while(current_cmd)
  {
    if (current_cmd->next)
    {
      if (current_cmd->heredoc_error == 1)
      {

      }
      if (pipe(pipe_fds) == -1)
      {
        perror("minishell: pipe");
        *last_exit_status_ptr = 1;
        if (prev_pipe_in_fd != -1)
          close(prev_pipe_in_fd);
        free(child_pids);
        return (1);
      }
    }
    pid = fork();
    if (pid == -1)
    {
      perror("minishell: fork");
      *last_exit_status_ptr = 1;
      if (prev_pipe_in_fd != -1)
        close(prev_pipe_in_fd);
      if (current_cmd->next)
      {
        close(pipe_fds[0]);
        close(pipe_fds[1]);
      }
      if (current_cmd->heredoc_fd != -1)
      {
        close(current_cmd->heredoc_fd);
        current_cmd->heredoc_fd = -1;
      }
      free(child_pids);
      return (1);
    }
    else if (pid == 0)
    {
      if (prev_pipe_in_fd != -1)
      {
        if (dup2(prev_pipe_in_fd, STDIN_FILENO) == -1)
        {
          perror("minishell: dup2 stdin child");
          exit (1);
        }
        close(prev_pipe_in_fd);
      }
      if (current_cmd->next)
      {
        if (dup2(pipe_fds[1], STDOUT_FILENO) == -1)
        {
          perror("minishell: dup2 stdout child");
          exit(1);
        }
      }
      if (current_cmd->next)
      {
          close(pipe_fds[0]);
          close(pipe_fds[1]);
      }
      if (handle_redirecctions_in_child(current_cmd) != 0)
      {
        exit(*last_exit_status_ptr);
      }
      if (current_cmd->args && current_cmd->args[0])
      {
        char **paths = get_paths(envp);
        char *cmd_path = find_cmd_path(current_cmd->args[0], paths);

        if (!cmd_path)
        {
          fprintf(stderr, "minishell: %s: command not found\n", current_cmd->args[0]);
          free_str_array(paths);
          exit(127);
        }
        execve(cmd_path, current_cmd->args, envp);
        perror("minishell: execve");
        free(cmd_path);
        free_str_array(paths);
        exit(126);
      }
      else
      {
        exit(0);
      }
    }
    else // prodceso padre..
    {
      child_pids[num_commands++] = pid;
      if (prev_pipe_in_fd != -1)
        close(prev_pipe_in_fd);
      if (current_cmd->next)
      {
        close(pipe_fds[1]);
        prev_pipe_in_fd = pipe_fds[0];
      }
      else
      {
        if (prev_pipe_in_fd != -1)
        {
          close(prev_pipe_in_fd);
          prev_pipe_in_fd = -1;
        }
      }
      if (current_cmd->heredoc_fd != -1)
      {
        close(current_cmd->heredoc_fd);
        current_cmd->heredoc_fd = -1;
      }
    }
    current_cmd = current_cmd->next;
  }
  int status;
  i = 0;
  while(i < num_commands)
  {
    waitpid(child_pids[i], &status, 0);
    if (i == num_commands - 1)
    {
      if (WIFEXITED(status))
        *last_exit_status_ptr = WEXITSTATUS(status);
      else if (WIFSIGNALED(status))
        *last_exit_status_ptr = 128 + WTERMSIG(status);
    }
    i++;
  }
  free(child_pids);
  return (0);
}*/



#include "../inc/executor.h"
#include "../inc/expander.h"
#include "../inc/parser.h"
#include "../libft/inc/libft.h"

extern int g_last_exit_status;

static char **get_paths(char **envp)
{
    int i = 0;
    while(envp[i])
    {
        if (ft_strncmp(envp[i], "PATH=", 5) == 0)
        {
            return (ft_split(envp[i] + 5, ':'));
        }
        i++;
    }
    return (ft_split("/bin:/usr/bin", ':'));
}

static char *find_cmd_path(const char *cmd, char **paths)
{
    char *full_path;
    char *temp_path;
    int i;

    if (!cmd)
        return (NULL);

    if (ft_strchr(cmd, '/') != NULL)
    {
        if (access(cmd, X_OK) == 0)
            return (ft_strdup(cmd));
        return (NULL);
    }
    if (!paths)
        return (NULL);

    i = 0;
    while (paths[i])
    {
        temp_path = ft_strjoin(paths[i], "/");
        if (!temp_path)
            return (NULL);
        full_path = ft_strjoin(temp_path, cmd);
        free(temp_path);
        if (!full_path)
            return (NULL);
        if (access(full_path, X_OK) == 0)
            return (full_path);
        free(full_path);
        i++;
    }
    return (NULL);
}

static void free_str_array(char **arr)
{
    if (!arr)
        return;
    int i;

    i = 0;
    while (arr[i])
    {
        free(arr[i]);
        i++;
    }
    free(arr);
}

// Función para ejecutar comandos
int execute_commands(t_command *commands, char **envp, int *last_exit_status_ptr)
{
    t_command *current_cmd;
    pid_t pid;
    int pipe_fds[2];
    int prev_pipe_in_fd = -1;
    pid_t *child_pids;
    int num_commands = 0;
    int i;

    current_cmd = commands;
    while (current_cmd)
    {
        num_commands++;
        current_cmd = current_cmd->next;
    }

    child_pids = ft_calloc(num_commands + 1, sizeof(pid_t));
    if (!child_pids)
    {
        perror("minishell: calloc");
        *last_exit_status_ptr = 1;
        // Los heredoc_fd abiertos son gestionados por parse_input al cerrar los previos
        // y por el padre después del fork.
        return (1);
    }

    num_commands = 0; // Reiniciar para usar como índice de child_pids
    current_cmd = commands;
    while (current_cmd)
    {
        // En Bash, un warning de heredoc no cambia el exit status.
        // La bandera `heredoc_error` se usa para que `handle_redirecctions_in_child`
        // sepa que no debe usar un `heredoc_fd` inválido para la entrada estándar.
        // No es necesario modificar el exit status del shell aquí solo por el warning.

        if (current_cmd->next) // Si no es el último comando en una tubería
        {
            if (pipe(pipe_fds) == -1)
            {
                perror("minishell: pipe");
                *last_exit_status_ptr = 1;
                if (prev_pipe_in_fd != -1)
                    close(prev_pipe_in_fd);
                free(child_pids);
                return (1);
            }
        }

        pid = fork();
        if (pid == -1)
        {
            perror("minishell: fork");
            *last_exit_status_ptr = 1;
            if (prev_pipe_in_fd != -1)
                close(prev_pipe_in_fd);
            if (current_cmd->next)
            {
                close(pipe_fds[0]);
                close(pipe_fds[1]);
            }
            // El heredoc_fd del comando actual se cerrará en el padre más abajo
            // o ya fue cerrado en parse_input si fue sobrescrito.
            free(child_pids);
            return (1);
        }
        else if (pid == 0) // Proceso hijo
        {
            // Configurar entrada de la tubería previa
            if (prev_pipe_in_fd != -1)
            {
                if (dup2(prev_pipe_in_fd, STDIN_FILENO) == -1)
                {
                    perror("minishell: dup2 stdin child");
                    exit(1);
                }
                close(prev_pipe_in_fd); // Cerrar el FD del pipe que se duplicó
            }

            // Configurar salida a la siguiente tubería (si existe)
            if (current_cmd->next)
            {
                if (dup2(pipe_fds[1], STDOUT_FILENO) == -1)
                {
                    perror("minishell: dup2 stdout child");
                    exit(1);
                }
            }

            // Cerrar los extremos del pipe que no se usarán en el hijo
            if (current_cmd->next)
            {
                close(pipe_fds[0]);
                close(pipe_fds[1]);
            }

            // Gestionar todas las redirecciones (incluido el heredoc)
            // handle_redirecctions_in_child usará cmd->heredoc_fd solo si heredoc_error es 0
            if (handle_redirecctions_in_child(current_cmd) != 0)
            {
                exit(1); // Error durante el manejo de redirecciones
            }

            if (current_cmd->args && current_cmd->args[0])
            {
                char **paths = get_paths(envp);
                char *cmd_path = find_cmd_path(current_cmd->args[0], paths);

                if (!cmd_path)
                {
                    fprintf(stderr, "minishell: %s: command not found\n", current_cmd->args[0]);
                    free_str_array(paths);
                    exit(127); // Command not found
                }
                execve(cmd_path, current_cmd->args, envp);
                perror("minishell: execve"); // Esto solo se ejecuta si execve falla
                free(cmd_path);
                free_str_array(paths);
                exit(126); // Command not executable
            }
            else // Caso de comando vacío (ej. solo redirecciones sin comando explícito)
            {
                exit(0);
            }
        }
        else // Proceso padre
        {
            child_pids[num_commands++] = pid; // Guardar el PID del hijo
            if (prev_pipe_in_fd != -1)
                close(prev_pipe_in_fd); // Cerrar el extremo de lectura del pipe anterior
            if (current_cmd->next)
            {
                close(pipe_fds[1]); // Cerrar el extremo de escritura del pipe actual
                prev_pipe_in_fd = pipe_fds[0]; // El extremo de lectura se convierte en la entrada para el siguiente comando
            }
            else // Es el último comando en la tubería
            {
                if (prev_pipe_in_fd != -1)
                {
                    close(prev_pipe_in_fd); // Asegurarse de cerrar el último pipe de entrada
                    prev_pipe_in_fd = -1;
                }
            }

            // Muy importante: Si el comando actual tiene un heredoc_fd abierto,
            // el padre debe cerrarlo para evitar fugas de descriptores de archivo.
            // El hijo ya lo habrá duplicado si era necesario.
            if (current_cmd->heredoc_fd != -1)
            {
                close(current_cmd->heredoc_fd);
                current_cmd->heredoc_fd = -1; // Marcar como cerrado
            }
        }
        current_cmd = current_cmd->next; // Pasar al siguiente comando
    }

    // Esperar a que todos los procesos hijos terminen
    int status;
    i = 0;
    while (i < num_commands)
    {
        waitpid(child_pids[i], &status, 0);
        if (i == num_commands - 1) // El estado de salida del último comando en la tubería es el que importa
        {
            if (WIFEXITED(status))
                *last_exit_status_ptr = WEXITSTATUS(status);
            else if (WIFSIGNALED(status))
                *last_exit_status_ptr = 128 + WTERMSIG(status);
        }
        i++;
    }
    free(child_pids); // Liberar el array de PIDs
    return (0);
}