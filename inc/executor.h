/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rhiguita <rhiguita@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/03 05:03:55 by rhiguita          #+#    #+#             */
/*   Updated: 2025/09/03 05:03:58 by rhiguita         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXECUTOR_H
# define EXECUTOR_H

# include "builtins.h"
# include "env_utils.h"
# include "minishell.h"
# include <signal.h>
# include <sys/types.h>
# include <sys/wait.h>

// Estructura para los datos de ejecución del pipeline
typedef struct s_exec_data
{
	int			pipe_fd[2];
	int			prev_fd;
	pid_t		pid;
	pid_t		*child_pids;
	int			cmd_idx;
	t_struct	*mini;
}				t_exec_data;

// executor.c
pid_t			create_pipe_and_fork(t_command *cmd, int pipe_fd[2]);
int				execute_commands(t_command *commands, t_struct *mini);
void			child_process(t_command *cmd, int pipe_fd[2], int prev_fd,
					t_struct *mini);
void			parent_process(t_command *cmd, t_exec_data *d);
int				execute_pipeline(t_command *cmds, t_struct *mini,
					pid_t *child_pids, int num_commands);

// pipeline_utils.c
void			parent_cleanup(t_command *cmd, t_exec_data *d);
void			wait_for_children(pid_t *child_pids, int num_commands,
					t_struct *mini);
int				init_pipeline(t_command *commands, pid_t **child_pids_ptr);
void			handle_fork_error(t_command *current_cmd, t_exec_data *d);
void			setup_child_pipes(t_command *cmd, int *pipe_fds,
					int prev_pipe_in_fd);

// redirection_executor_handler
int				handle_redirections_in_child(t_command *cmd);
int				handle_single_redirection_only(t_command *cmd, t_struct *mini);
int				process_heredocs(t_command *commands, t_struct *mini);
int				dup2_and_close(int oldfd, int newfd);
int				open_redirection_file(t_redirection *redir);
int				handle_single_redirection(t_redirection *redir, int *last_fd);
int				handle_redir(t_redirection *redir, int *last_fd);
int				apply_final_fds(int last_in_fd, int last_out_fd);
int				process_redirections(t_command *cmd, int *last_in_fd,
					int *last_out_fd);
int				handle_redirections_in_child(t_command *cmd);
void			close_heredoc(t_command *cmd);
void			restore_fds(int stdin_fd, int stdout_fd);
void			print_heredoc_warning(t_redirection *redir);
int				process_heredoc_line(t_redirection *redir, t_struct *mini,
					int pipe_write_fd);
int				read_heredoc_input(t_redirection *redir, t_struct *mini);

// cmd_utils.c
void			add_slash_to_paths(char **paths);
char			*get_path_var(char **envp);
char			**get_paths(char **envp);
char			*check_direct_path(char *cmd);
void			child_execute_command(t_command *cmd, t_struct *mini);
int				handle_single_builtin(t_command *cmd, t_struct *mini);
char			*find_cmd_path(char **cmd_args, char **paths);
void			execute_external_command(t_command *cmd, t_struct *mini);

// builtin_handler.c
int				handle_single_builtin(t_command *cmd, t_struct *mini);
int				is_builtin(char *cmd_name);
int				execute_builtin(t_command *cmd, t_struct *mini);

#endif
