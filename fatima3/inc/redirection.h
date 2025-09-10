/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhiguita <rhiguita@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 06:19:11 by rhiguita          #+#    #+#             */
/*   Updated: 2025/09/03 06:19:13 by rhiguita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REDIRECTION_H
# define REDIRECTION_H

# include "minishell.h"
# include "parser.h"
# include <signal.h>

int		process_heredoc_input(t_command *cmd_list, t_struct *mini);
int		heredoc_loop(t_redirection *heredoc, int pipe_fd, t_struct *mini);
int		write_heredoc_line(char *expanded_line, int pipe_fd, t_struct *mini);
int		handle_single_heredoc(t_redirection *redir, int *heredoc_fd,
			t_struct *mini);
char	*get_expanded_line(char *line, t_redirection *heredoc, t_struct *mini);
void	handle_sigint_heredoc(int sig);

#endif
