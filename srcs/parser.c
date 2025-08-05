#include "../inc/parser.h"
#include "../inc/expander.h"
#include "../inc/executor.h"
#include "../libft/inc/ft_printf.h"
#include "../libft/inc/libft.h"

extern int g_last_exit_status;

static int procces_heredoc_input(char *delimiter, bool expand_content, char **envp) // <-- AHORA
{
    int pipe_fds[2];
    char *line;
    char *line_to_process; // Nueva variable para la línea expandida o literal
    int ret_fd;

    if (pipe(pipe_fds) == -1)
    {
        perror("minishell: pipe for heredoc");
        return (-1);
    }
    ret_fd = pipe_fds[0]; // Guarda el FD de lectura para devolver

    while(1)
    {
        line = readline("> ");
        if (!line) // EOF (Ctrl+D)
        {
            ft_putstr_fd("minishel: warning: here-document delimited by end-of-file (wanted '",
                STDERR_FILENO);
            ft_putstr_fd(delimiter, STDERR_FILENO); // El 'delimiter' ya es el valor procesado
            ft_putstr_fd("')\n", STDERR_FILENO);
            close(pipe_fds[1]); // Cierra el extremo de escritura
            return (-2);
        }

        // Determina la línea para comparar y escribir en el pipe
        if (expand_content)
        {
            // Si el contenido del heredoc debe expandirse, expande la línea leída.
            line_to_process = expand_single_string(line, envp, g_last_exit_status);
            if (!line_to_process) // Error de malloc en expansión
            {
                free(line);
                close(pipe_fds[1]);
                return (-1);
            }
        }
        else
        {
            // Si el contenido del heredoc NO debe expandirse, usa la línea leída tal cual.
            // Creamos una copia para mantener la consistencia de free() después.
            line_to_process = ft_strdup(line);
            if (!line_to_process)
            {
                free(line);
                close(pipe_fds[1]);
                return (-1);
            }
        }

        // Compara la línea procesada (expandida o literal) con el delimitador final.
        if (ft_strcmp(line_to_process, delimiter) == 0)
        {
            free(line_to_process);
            free(line); // La línea original de readline
            break; // Delimitador encontrado
        }

        // Escribe la línea procesada (expandida o literal) en el pipe.
        write(pipe_fds[1], line_to_process, ft_strlen(line_to_process));
        write(pipe_fds[1], "\n", 1); // ¡Añadir el salto de línea!

        free(line_to_process); // Libera la cadena line_to_process (strdup o expand_single_string)
        free(line); // Libera la cadena original de readline
    }
    close(pipe_fds[1]); // Cierra el extremo de escritura del pipe
    return (ret_fd); // Devuelve el extremo de lectura
}

/*
static int handle_command_heredocs(t_command *cmd, char **envp)
{
  (void)envp;
  t_redirection *current_redir = cmd->redirs;
  t_redirection *last_heredoc_redir = NULL;

  while (current_redir)
  {
    if (current_redir->type == REDIR_HEREDOC)
    {
      last_heredoc_redir = current_redir;
    }
    current_redir = current_redir->next;
  }
  if (last_heredoc_redir)
  {
    if (cmd->heredoc_fd != -1)
    {
      close(cmd->heredoc_fd);
      cmd->heredoc_fd = -1;
    }
    cmd->heredoc_fd = procces_heredoc_input(last_heredoc_redir->file);
    if (cmd->heredoc_fd == -1)
    {
      return (-1);
    }
  }
  return (0);
}*/

//Función principal del parser.
t_command *parse_input(t_token *token_list, char **envp)
{
  t_command *head_cmd = NULL;
  t_command *current_cmd = NULL;
  t_command *temp;
  t_redirection *new_redir;
  t_token *current_token = token_list;
  t_redir_type redir_enum_type;  // -- Declaración de la variable de mapeo.

  if (!token_list)
    return (NULL);
  while (current_token)
  {
    if (!current_cmd)
    {
      current_cmd = create_command_node();
      if (!current_cmd)
      {
        free_tokens(token_list);
        free_commands(head_cmd);
        return (NULL);
      }
      current_cmd->heredoc_fd = -1;
      current_cmd->heredoc_error = 0;
      if (!head_cmd)
        head_cmd = current_cmd;
      else
      {
        temp = head_cmd;
        while (temp->next)
          temp = temp->next;
        temp->next = current_cmd;
      }
    }
    if (current_token->type == WORD)
    {
      if (add_arg_to_command(current_cmd, current_token->value) != 0)
      {
        free_tokens(token_list);
        free_commands(head_cmd);
        return (NULL);
      }
      current_token = current_token->next; 
    }
    else if (current_token->type >= IN && current_token->type <= HEREDOC) // Es una redirección
    {
      if   (!current_token->next || current_token->next->type == PIPE)
      {
         fprintf(stderr, "minishell: Syntax error near unexpected token '%s'\n",
          current_token->next ? current_token->next->value : "newline");
        free_tokens(token_list);
        free_commands(head_cmd);
        return (NULL);
      }
      if (current_token->next->type != WORD)
      {
        fprintf(stderr, "minishell: Syntax error near unexpected token '%s'\n",
          current_token->next->value);
        free_tokens(token_list);
        free_commands(head_cmd);
        return (NULL);
      }

      if (current_token->type == IN)
        redir_enum_type = REDIR_IN;
      else if (current_token->type == OUT)
        redir_enum_type = REDIR_OUT;
      else if (current_token->type == APPE_OUT)
        redir_enum_type = REDIR_APPEND;
      else if (current_token->type == HEREDOC)
        redir_enum_type = REDIR_HEREDOC;
      else
      {
        fprintf(stderr, "minishell: Parser error: unknow redirection type\n");
        free_tokens(token_list);
        free_commands(head_cmd);
        return (NULL);
      }
      char *file_name_or_delimiter = NULL;
      bool expand_heredoc_content = false; // flag para heredoc
      if (redir_enum_type == REDIR_HEREDOC)
      {
        char *raw_delimiter_token_value = current_token->next->value;
        size_t len = ft_strlen(raw_delimiter_token_value);

        if (len > 1 && raw_delimiter_token_value[0] == '\'' && 
            raw_delimiter_token_value[len - 1] == '\'')
        {
          file_name_or_delimiter = ft_substr(raw_delimiter_token_value, 1, len - 2);
          expand_heredoc_content = false; // contenido no se expande
        }
        else if (len > 1 && raw_delimiter_token_value[0] == '"' &&  
                  raw_delimiter_token_value[len - 1] == '"')
        {
          file_name_or_delimiter = ft_substr(raw_delimiter_token_value, 1, len - 2);
          expand_heredoc_content = false;
        }
        else // Sin comillas
        {
          file_name_or_delimiter = expand_single_string(raw_delimiter_token_value, envp, g_last_exit_status);
          expand_heredoc_content = true;
        }
        if (!file_name_or_delimiter) // ERROR MALLOC/EXPANSION
        {
          free_tokens(token_list);
          free_commands(head_cmd);
          return (NULL);
        }
        int fd_result = procces_heredoc_input(file_name_or_delimiter, expand_heredoc_content, envp);
        free(file_name_or_delimiter);
        if (fd_result == -1)
        {
          free(file_name_or_delimiter);
          free_tokens(token_list);
          free_commands(head_cmd);
          return (NULL);
        }
        else if(fd_result == -2)
        {
          current_cmd->heredoc_error = -1;
          if (current_cmd->heredoc_fd != -1)
            close(current_cmd->heredoc_fd);
          current_cmd->heredoc_fd = -1;
        }
        else
        {
          if (current_cmd->heredoc_fd != -1)
            close(current_cmd->heredoc_fd);
          current_cmd->heredoc_fd = fd_result;
        }
        /*if (current_cmd->heredoc_fd != -1)
        {
          close(current_cmd->heredoc_fd);
        }
        current_cmd->heredoc_fd = procces_heredoc_input(file_name_or_delimiter);
        if (current_cmd->heredoc_fd == -1)
        {
          free(file_name_or_delimiter);
          free_tokens(token_list);
          free_commands(head_cmd);
          return (NULL);
        }*/
      }
      else // PARA <, >, >>
      {
        // Para redirecciones, el fille es simplemente el valor del token
        file_name_or_delimiter = ft_strdup(current_token->next->value);
        if (!file_name_or_delimiter)
        {
          free_tokens(token_list);
          free_commands(head_cmd);
          return (NULL);
        }
        expand_heredoc_content = false;
      }
        // crear nodo new_redir una sola vez despues de procesar el valor.
      new_redir = create_redirection_node(redir_enum_type, file_name_or_delimiter);
      if (!new_redir)
      {
        free(file_name_or_delimiter); //liberar si falla la creación del nodo.
        if (redir_enum_type == REDIR_HEREDOC && current_cmd->heredoc_fd != -1)
        {
          close(current_cmd->heredoc_fd);
          current_cmd->heredoc_fd = -1;
        }
        free_tokens(token_list);
        free_commands(head_cmd);
        return (NULL);
      }
      // Asignar el flag solo para los heredocs 
      new_redir->expand_heredoc_content = expand_heredoc_content;
      add_redir_to_command(current_cmd, new_redir);
      current_token = current_token->next->next; // Avanza el operador y el nombre del.
    }
    else if (current_token->type == PIPE)
    {
      if ((!current_cmd->args && !current_cmd->redirs) ||
          (current_token->next && current_token->next->type == PIPE))
      {
        fprintf(stderr, "minishell: Syntax error near unexpected token '%s'\n", current_token->value);
        free_tokens(token_list);
        free_commands(head_cmd);
        return (NULL);
      }
      if (!current_token->next)
      {
        fprintf(stderr, "minishell: Syntax error near unexpected token 'newline'\n");
        free_tokens(token_list);
        free_commands(head_cmd);
        return (NULL);
      }
      current_cmd = NULL; // Pipe siempre indica el inicio de un nuevo comando.
      current_token = current_token->next;
    }
    else
    {
      fprintf(stderr, "minishell: parser error: unexpected token type '%s'\n", current_token->value);
      free_tokens(token_list);
      free_commands(head_cmd);
      return(NULL);
    }
  }
  /*
  if (current_cmd)
  {
    if (handle_command_heredocs(current_cmd, envp) == -1)
    {
      free_tokens(token_list);
      free_commands(head_cmd);
      return (NULL);
    }
  }*/
  return (head_cmd);
}