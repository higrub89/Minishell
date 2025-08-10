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

# include <stdbool.h>

//Tipos de tokens.
typedef enum e_token_type
{
  WORD,       //Una palabra normal.
  PIPE,       //operador |
  IN,   //operador <
  OUT,  //operador >
  APPE_OUT, //operador >>
  HEREDOC,    //operador <<

} t_token_type;

//Estructura para un token individual.
typedef struct s_token
{
  char  *value;
  t_token_type type;
  struct s_token *next;
  struct s_token *prev;
} t_token;

//Tipos de redirección
typedef enum e_redir_type
{
  REDIR_IN,     // <
  REDIR_OUT,    // >
  REDIR_APPEND, // >>
  REDIR_HEREDOC, // <<
} t_redir_type;

// Estructura para las redirecciones.
typedef struct s_redirection
{
  t_redir_type type;
  char    *file;
  int heredoc_fd;
  bool  expand_heredoc_content;
  bool  heredoc_error;
  struct s_redirection *next;
} t_redirection;

// Estructura para un comando.
typedef struct s_command
{
  char    **args;
  int num_args;
  t_redirection *redirections;
  int num_redirections;
  char *full_path;
  struct s_command *next;
  struct s_command *prev;
} t_command;

typedef struct s_minishell
{
  char **envp;
  int last_exit_status;
} t_struct;

t_struct *init_minishell_struct(char **envp_original);
void cleanup_minishell(t_struct *mini);
void free_commands(t_command *head);
void free_tokens(t_token *head);

#endif