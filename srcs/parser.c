#include "../inc/parser.h"
#include "../inc/expander.h"
#include "../libft/inc/ft_printf.h"

//Función principal del parser.
t_command *parse_input(t_token *token_list)
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
      if (!current_token->next ||
          (current_token->next->type != WORD &&
            current_token->next->type != IN &&
            current_token->next->type != OUT &&
            current_token->next->type != APPE_OUT &&
            current_token->next->type != HEREDOC &&
            current_token->next->type != PIPE))
      {
         fprintf(stderr, "minishell: Syntax error near unexpected token '%s'\n",
          current_token->next ? current_token->next->value : "newline");
        free_tokens(token_list);
        free_commands(head_cmd);
        return (NULL);
      }
      if (!current_token->next || current_token->next->type != WORD)
      {
        fprintf(stderr, "minishell: Syntax error near unexpected token '%s'\n",
          current_token->next ? current_token->next->value: "newline");
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
      new_redir = create_redirection_node(redir_enum_type, current_token->next->value);
      if (!new_redir)
      {
        free_tokens(token_list);
        free_commands(head_cmd);
        return (NULL);
      }
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
  return (head_cmd);
}