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
  if (!new_token)
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
  }
  return (head);
}