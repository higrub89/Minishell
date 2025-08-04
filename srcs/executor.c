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
}