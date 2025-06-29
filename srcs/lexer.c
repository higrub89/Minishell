/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhiguita <rhiguita@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/24 16:34:07 by rhiguita          #+#    #+#             */
/*   Updated: 2025/06/24 16:34:10 by rhiguita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell.h"

t_token *create_new_token(char *value, t_token_type type)
{
  t_token *new_token = (t_token *)malloc(sizeof(t_token));
  if (!new_token)
  {
    perror("malloc failed in create_new_token");
    exit(EXIT_FAILURE);
  }

  new_token->value = ft_strdup(value);
  if (!new_token->value)
  {
    perror("ft_strdup failed in create_new_token");
    exit(EXIT_FAILURE);
  }
  new_token->type = type;
  new_token->next = NULL;
  return (new_token);
}

t_token *lexer(char *input_line)
{
  t_token *head = NULL;     //cabeza de la lista de tokens.
  t_token *current = NULL;  //Puntero para añadir tokens al final de la lista.
  int i = 0;

  if(!input_line)
    return (NULL);

  while(input_line[i])
  {
    while (input_line[i] && (input_line[i] == ' ' || input_line[i] == '\t')) //Saltar espacios.
      i++;
    if (!input_line[i]) //Si terminamos de leer la linea despues de espacios.
      break ;
    if(input_line[i] == '<')
    {
      if (input_line[i+1] == '<')
      {
        add_token_to_list(&head, &current, create_new_token("<<", HEREDOC));
        i += 2; //avanzo dos posiciones.
      }
      else
      {
        add_token_to_list(&head, &current, create_new_token("<", REDIR_IN));
        i += 1; //avanzo una posición.
      }
    }
    else if(input_line[i] == '>')
    {
      if (input_line[i+1] == '>')
      {
        add_token_to_list(&head, &current, create_new_token(">>", APPEND_OUT));
        i += 2;
      }
      else
      {
        add_token_to_list(&head, &current, create_new_token(">", REDIR_OUT));
        i += 1;
      }
    }
    else if(input_line[i] == '|')
    {
      add_token_to_list(&head, &current, create_new_token("|", PIPE));
      i += 1;
    }
    else
    {
      int start = i; //Guardo el inicio del token actual.

      while(input_line[i] && input_line[i] != ' ' && input_line[i] != '\t' 
          && input_line[i] != '<' && input_line[i] != '>'  && input_line[i] != '|')
      {
        if (input_line[i] == '\'' || input_line[i] == '\"')
        {
          char quote_char = input_line[i];
          i++;
          while (input_line[i] && input_line[i] !=  quote_char)
            i++;
          if (input_line[i] == quote_char)
            i++;
          else
          {
            // Aquí debo manejar el caso cuando falta la comilla de cierre..
            fprintf(stderr, "Debes manejar Este caso, llamado 'Prom string two PS2'\n");
            free_tokens(head);
            return (NULL);
          }
        }
        else
        {
          i++; // Es un caracter sin comillas que forma parte de la palabra!.
        }
      }
      int token_len = i - start;
      char *word_value = (char *)malloc(token_len + 1);
      if (!word_value)
      {
        perror("malloc failed for word_value");
        free_tokens(head);
        return (NULL);
      }
      ft_strncpy(word_value, &input_line[start], token_len);
      word_value[token_len] = '\0';
      add_token_to_list(&head, &current, create_new_token(word_value, WORD));
      free(word_value);
    }
  }
  return (head);
}

void add_token_to_list(t_token **head, t_token **current, t_token *new_token)
{
  if (!*head)
  {
    *head = new_token;
    *current = new_token;
  }
  else
  {
    (*current)->next = new_token;
    *current = new_token;
  }
}

void free_tokens(t_token *head)
{
  t_token *tmp;
  while (head)
  {
    tmp = head;
    head = head->next;
    free(tmp->value);
    free(tmp);
  }
}