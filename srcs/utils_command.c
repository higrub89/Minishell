#include "../inc/command.h"
#include "libft/inc/libft.h"

// Función para crear e inicializar un nuevo nodo.
t_command *create_command_node(void)
{
  t_command *new_cmd;

  new_cmd = (t_command*)ft_calloc(1, sizeof(t_command)); // Incializamos a NULL;
  if (!new_cmd)
  {
    perror("minishell: malloc failed for command node");
    return (NULL);
  }
  return (new_cmd);
}

//Función para añadir un argumento string al array args de un t_command.
int add_arg_to_command(t_command *cmd, char *arg_value)
{
  int current_arg_count;
  size_t old_total_bytes;
  size_t new_total_bytes;
  char **tem_args;
  char *new_arg_str;

  if (!cmd || !arg_value)
    return (1);
  current_arg_count = 0;
  if (cmd->args)
  {
    while (cmd->args[current_arg_count])
      current_arg_count++;
    old_total_bytes = sizeof(char *) * (current_arg_count + 1);
  }
  else
    old_total_bytes = 0;
  new_total_bytes = sizeof(char *) * (current_arg_count + 2);
  tem_args = (char **)ft_realloc(cmd->args, old_total_bytes, new_total_bytes);
  if (!tem_args)
  {
    perror("minishell: ft_realloc failed for command args");
    return (1);
  }
  new_arg_str = ft_strdup(arg_value);
  if (!new_arg_str)
    return (1);
  tem_args[current_arg_count] = new_arg_str;
  tem_args[current_arg_count + 1] = NULL;
  cmd->args = tem_args;
  return (0);
}

//Función para crear e inicializar un nuevo nodo t_redirection.
t_redirection *create_redirection_node(t_redir_type type, char *file_name)
{
  t_redirection *new_redir;

  new_redir = (t_redirection *)ft_calloc(1, sizeof(t_redirection));
  if (!new_redir)
  {
    perror("minishell: malloc failed for redirection node");
    return (NULL);
  }
  new_redir->type = type;
  new_redir->file = ft_strdup(file_name);
  if (!new_redir->file)
  {
    perror("minishell: ft_strdup failed for redirection file");
    free(new_redir);
    return (NULL);
  }
  new_redir->next = NULL;
  return (new_redir);
}

//Función para añadir una direccion a la lista de redirecciones de un t_command.
void  add_redir_to_command(t_command *cmd, t_redirection *new_redir)
{
  t_redirection *temp;

  if (!cmd || !new_redir)
    return;
  if (!cmd->redirs) // si es la primera redirección.
  {
    cmd->redirs = new_redir;
  }
  else
  {
    temp = cmd->redirs;
    while (temp->next)
      temp = temp->next;
    temp->next = new_redir;
  }
}

// FUnción para liberar toda la estructura de comandos.
void free_commands(t_command *head)
{
  t_command *current_cmd;
  t_command *next_cmd;
  t_redirection *current_redir;
  t_redirection *next_redir;
  int i;

  current_cmd = head;
  while (current_cmd)
  {
    next_cmd = current_cmd->next;
    if (current_cmd->args) // liberar el array de argumentos.
    {
      i = 0;
      while (current_cmd->args[i])
      {
        free(current_cmd->args[i]);
          i++;
      }
      free(current_cmd->args);
    }
    current_redir = current_cmd->redirs; //Liberar la lista de redirecciones.
    while (current_redir)
    {
      next_redir = current_redir->next;
      if (current_redir->file)
        free(current_redir->file); // Liberar el nombre del archivo de redireccion.
      free(current_redir); //Liberra el nodo de redireccion.
      current_redir = next_redir;
    }
    free(current_cmd); //Liberar el nodo del comando.
    current_cmd = next_cmd;
  }
}