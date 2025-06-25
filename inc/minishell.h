/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhiguita <rhiguita@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/20 13:39:22 by rhiguita          #+#    #+#             */
/*   Updated: 2025/06/20 14:58:16 by rhiguita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "../libft/inc/libft.h"
# include "../libft/inc/ft_printf.h"
# include "../libft/inc/get_next_line.h"

# include <errno.h>
# include <readline/readline.h>
# include <readline/history.h>


//Estructura Enum para los tipos de tokens.
typedef enum s_token_type
{
  WORD,       //Una palabra normal.
  PIPE,       //operador |
  REDIR_IN,   //operador <
  REDIR_OUT,  //operador >
  APPEND_OUT, //operador >>
  HEREDOC,    //operador <<

} t_token_type;

//Estructura para un token individual.
typedef struct s_token
{
  char  *value;
  t_token_type type;
  struct s_token *next;
} t_token;

int main(int argc, char **argv, char **env);

//lexer.c
t_token *create_new_token(char *value, t_token_type type);
t_token *lexer(char *input_line);
void add_token_to_list(t_token **head, t_token **current, t_token *new_token);
void free_tokens(t_token *head);

#endif
