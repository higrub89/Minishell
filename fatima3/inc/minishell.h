/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fatigarc <fatigarc@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/06/20 13:39:22 by rhiguita          #+#    #+#             */
/*   Updated: 2025/09/04 00:05:08 by fatigarc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "../libft/inc/libft.h"
# include <readline/history.h>
# include <readline/readline.h>
# include <signal.h>
# include <stdbool.h>

typedef enum e_token_type
{
	WORD,
	PIPE,
	IN,
	OUT,
	APPE_OUT,
	HEREDOC,
}							t_token_type;

typedef struct s_token
{
	char					*value;
	t_token_type			type;
	struct s_token			*next;
	struct s_token			*prev;
}							t_token;

typedef enum e_redir_type
{
	REDIR_IN,
	REDIR_OUT,
	REDIR_APPEND,
	REDIR_HEREDOC,
}							t_redir_type;

typedef struct s_redirection
{
	t_redir_type			type;
	char					*file;
	bool					expand_heredoc_content;
	struct s_redirection	*next;
}							t_redirection;

typedef struct s_command
{
	char					**args;
	int						num_args;
	t_redirection			*redirections;
	int						num_redirections;
	char					*full_path;
	int						heredoc_fd;
	struct s_command		*next;
	struct s_command		*prev;
}							t_command;

typedef struct s_minishell
{
	char					**export_list;
	char					**envp;
	int						last_exit_status;
	bool					should_exit;
	bool					is_piped;
}							t_struct;

typedef enum e_mode
{
	INTERACTIVE,
	NON_INTERACTIVE,
	CHILD
}							t_mode;

void						set_signals(t_mode mode);
int							ft_str_is_whitespace(const char *s);

#endif