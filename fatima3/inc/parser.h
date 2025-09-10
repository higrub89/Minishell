/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhiguita <rhiguita@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 02:24:50 by rhiguita          #+#    #+#             */
/*   Updated: 2025/09/03 02:24:54 by rhiguita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSER_H
# define PARSER_H

# include "expander.h"
# include "minishell.h"

// parser.c
t_token			*process_redirection(t_token *tok, t_command *cmd,
					t_struct *mini);
t_token			*handle_pipe(t_command **cmd, t_token *tok, t_struct *mini,
					t_command *head);
t_command		*finalize_cmds(t_command *head, t_command *curr,
					t_struct *mini);
t_token			*dispatch_token(t_token *tok, t_command **curr, t_struct *mini,
					t_command *head);
t_command		*parse_input(t_token *token_list, t_struct *mini);

// parser_utils.c
int				add_arg_to_command(t_command *cmd, const char *arg);
t_command		*create_command_node(void);
t_redirection	*create_redirection_node(t_redir_type type, char *file);
void			add_redir_to_command(t_command *cmd, t_redirection *new_redir);
char			*remove_quotes(char *str);

// parser_cleanup.c
void			*set_syntax_error(const char *token_value, t_struct *mini);
void			free_args(char **args);
void			free_redirections(t_redirection *redir);
void			free_commands(t_command *head);

#endif
