#include "../inc/parser.h"
#include "../libft/inc/ft_printf.h"

//Función principal del parser.
t_command *parse_input(t_token *token_list)
{
  t_command *head_cmd = NULL;
  t_command *current_cmd = NULL;
  t_command *temp;
  t_redirection *new_redir;
  t_token *current_token = token_list;

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
    else if (current_token->type >= IN && current_token->type <= APPE_OUT) // Es una redirección
    {
      if (!current_token->next || current_token->next->type != WORD)
      {
        fprintf(stderr, "minishell: Syntax error near unexpected token '%s'\n",
          current_token->next ? current_token->next->value : "newline");
        free_tokens(token_list);
        free_commands(head_cmd);
        return (NULL);
      }
      new_redir = create_redirection_node(current_token->type, current_token->next->value);
      if (!new_redir)
      {
        free_tokens(token_list);
        free_commands(head_cmd);
        return (NULL);
      }
      add_redir_to_command(current_cmd, new_redir);
      current_token = current_token->next->next;
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
        fprintf(stderr, "minishell: Syntax error near expected token '%s'\n", current_token->value);
        free_tokens(token_list);
        free_commands(head_cmd);
        return (NULL);
      }
      current_cmd = NULL;
      current_token = current_token->next;
    }
    else
    {
      fprintf(stderr, "minishell: parser error: unexpected token type %d\n", current_token->type);
      free_tokens(token_list);
      free_commands(head_cmd);
      return(NULL);
    }
  }
  return (head_cmd);
}